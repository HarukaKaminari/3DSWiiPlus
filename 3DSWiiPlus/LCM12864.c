#include <STRING.H>
#include "LCM12864.h"
#if defined LCM_DRIVER_UC1701X
#include "UC1701X.h"
#elif defined LCM_DRIVER_IST3932
#include "IST3932.h"
#elif defined LCM_DRIVER_SSD1306
#include "SSD1306.h"
#endif
#include "SPI.h"
#include "FontText.h"


#ifndef ABS
#define ABS(a)	((a) > 0 ? (a) : -(a))
#endif


#ifdef USE_12832
u8 xdata VRAM[512];		// 12832的显存为512字节
#else
u8 xdata VRAM[1024];	// 12864的显存为1K字节
#endif
u16 xdata tmpBuf[0x10];	// 16*2的字模缓冲区


void LCM12864_SendCommand(u8 value){
	LCM12864_CS = 0;
	LCM12864_CD = 0;
	SPI_SendAndReceive(value);
}

void LCM12864_SendData(u8 value){
	LCM12864_CS = 0;
	LCM12864_CD = 1;
	SPI_SendAndReceive(value);
}

void LCM12864_Init(){
	P1M0 &= 0xF8;
	P1M1 &= 0xF8;	// 设置/CS、CD和LED为准双向
	
#if defined LCM_DRIVER_UC1701X
	UC1701X_Init();
#elif defined LCM_DRIVER_IST3932
	IST3932_Init();
#elif defined LCM_DRIVER_SSD1306
	SSD1306_Init();
#endif
}

void LCM12864_Clear(){
	memset(VRAM, 0, sizeof(VRAM));
}

void LCM12864_Refresh(){
#if defined LCM_DRIVER_UC1701X
	UC1701X_Refresh();
#elif defined LCM_DRIVER_IST3932
	IST3932_Refresh();
#elif defined LCM_DRIVER_SSD1306
	SSD1306_Refresh();
#endif
}

/*
打开/关闭背光
*/
void LCM12864_Backlight(bit isOn){
	LCM12864_LED = isOn;
}

// 仅供双字节字符使用。单字节字符可以直接计算出idx
static u16 LCM12864_GetIdxFromASCII(u16 ascii){
	u16 i;
	for(i=0;i<CHARACTER_COUNT;++i){
		if(fontIdx[i] == ascii)
			break;
	}
	return i + 0x60;
}

void LCM12864_DrawChar(u8 fontSize, u16 idx, s8 x, s8 y, bit c){
	memcpy(tmpBuf, fontText + idx * sizeof(u16) * fontSize, sizeof(u16) * fontSize);
#if defined LCM_DRIVER_UC1701X
	UC1701X_DrawChar(fontSize, x, y, c);
#elif defined LCM_DRIVER_IST3932
	IST3932_DrawChar(fontSize, x, y, c);
#elif defined LCM_DRIVER_SSD1306
	SSD1366_DrawChar(fontSize, x, y, c);
#endif
}

void LCM12864_DrawString(u8 fontSize, const char* str, s16 x, s16 y, bit c){
	u8 i = 0;
	while(1){
		u16 chr = (u16)str[i];
		if(chr == 0){
			return;
		}else if(chr < 0x80){
			i++;
			if(x >= -fontSize && x <= 127)
				LCM12864_DrawChar(fontSize, chr - 0x20, x, y, c);
			x += (fontSize >> 1);
		}else{
			chr <<= 8;
			i++;
			chr |= str[i];
			i++;
			if(x >= -fontSize && x <= 127)
				LCM12864_DrawChar(fontSize, LCM12864_GetIdxFromASCII(chr), x, y, c);
			x += fontSize;
		}
	}
}

void LCM12864_DrawStringIdx(u8 fontSize, u8 idx, s16 x, s16 y, bit c){
	u8 i = 0;
	u16* str = strings[idx];
	while(str[i] != 0xFFFF){
		u16 _idx = str[i];
		i++;
		if(x >= -fontSize && x <= 127)
			LCM12864_DrawChar(fontSize, _idx, x, y, c);
		// 索引值小于0x60的，都是单字节字符，宽度为一半
		if(_idx < 0x60){
			x += (fontSize >> 1);
		}else{
			x += fontSize;
		}
	}
}

/*
在缓冲区任意位置画点
*/
void LCM12864_DrawPixel(u8 x, u8 y, bit c){
#if defined LCM_DRIVER_UC1701X
	UC1701X_DrawPixel(x, y, c);
#elif defined LCM_DRIVER_IST3932
	IST3932_DrawPixel(x, y, c);
#elif defined LCM_DRIVER_SSD1306
	SSD1306_DrawPixel(x, y, c);
#endif
}

/*
在缓冲区任意位置画直线
*/
void LCM12864_DrawLine(u8 x0, u8 y0, u8 x1, u8 y1, bit c){
	s8 dx = x1 - x0;
	s8 dy = y1 - y0;
	s8 ux = ((dx > 0) << 1) - 1;	// x的增量方向，正负1
	s8 uy = ((dy > 0) << 1) - 1;	// y的增量方向，正负1
	s8 x = x0, y = y0, eps = 0;		// eps为累加误差

	dx = ABS(dx);
	dy = ABS(dy); 
	if(dx > dy){
		for(;x!=x1;x+=ux){
			LCM12864_DrawPixel(x, y, c);
			eps += dy;
			if((eps << 1) >= dx){
				y += uy;
				eps -= dx;
			}
		}
	}else{
		for(;y!=y1;y+=uy){
			LCM12864_DrawPixel(x, y, c);
			eps += dx;
			if((eps << 1) >= dy){
				x += ux;
				eps -= dy;
			}
		}
	}
}

/*
在缓冲区任意位置填充矩形
*/
void LCM12864_FillRect(u8 x, u8 y, u8 w, u8 h, bit c){
#if defined LCM_DRIVER_UC1701X
	UC1701X_FillRect(x, y, w, h, c);
#elif defined LCM_DRIVER_IST3932
	IST3932_FillRect(x, y, w, h, c);
#elif defined LCM_DRIVER_SSD1306
	SSD1306_FillRect(x, y, w, h, c);
#endif
}

/*
在缓冲区任意位置画圆
*/
void LCM12864_DrawCircle(u8 x1, u8 y1, u8 r, bit c){
	s16 x = 0, y = r;
	s8 d = 1 - r;
	while(y > x){
		LCM12864_DrawPixel(x1 + x, y1 + y, c);
		LCM12864_DrawPixel(x1 + y, y1 + x, c);
		LCM12864_DrawPixel(x1 - x, y1 + y, c);
		LCM12864_DrawPixel(x1 - y, y1 + x, c);
		LCM12864_DrawPixel(x1 - x, y1 - y, c);
		LCM12864_DrawPixel(x1 - y, y1 - x, c);
		LCM12864_DrawPixel(x1 + x, y1 - y, c);
		LCM12864_DrawPixel(x1 + y, y1 - x, c);
		if(d < 0){
			d += 2 * x + 3;
		}else{
			d += 2 * (x - y) + 5;
			y--;
		}
		x++;
	}
}

/*
从索引获得字符串长度
*/
u16 LCM12864_GetStrLenFromIdx(u8 fontSize, u8 idx){
	u16 ret = 0;
	u16* str = strings[idx];
	u16 i = 0;
	while(1){
		u16 cidx = str[i++];
		if(cidx == 0xFFFF)
			break;
		else if(cidx < 0x60)
			ret += (fontSize >> 1);
		else
			ret += fontSize;
	}
	return ret;
}
