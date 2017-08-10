// IST3932Driver.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "FontText.h"
#include "IST3932Driver.h"


#ifdef USE_12832
u8 VRAM[512];		// 12832的显存为512字节
#else
u8 VRAM[1024];		// 12864的显存为1K字节
#endif
u16 tmpBuf[0x10];	// 16*2的字模缓冲区


#ifndef ABS
#define ABS(a)	((a) > 0 ? (a) : -(a))
#endif


void LCM12864_Init(){
	memset(VRAM, 0, sizeof(VRAM));
	memset(tmpBuf, 0, sizeof(tmpBuf));
}

void LCM12864_Clear(){
	memset(VRAM, 0, sizeof(VRAM));
}

bool LCM12864_Refresh(int x, int y){
	return (VRAM[(x >> 3) + (y << 4)] & (1 << (x & 7))) != 0;
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
	//u16* tmpBuf = (u16*)(fontText + idx * sizeof(u16) * fontSize);
	for(int _=0;_<fontSize;++_){
		u8 lo = *(fontText + idx * sizeof(u16) * fontSize + _ * sizeof(u16) + 0);
		u8 hi = *(fontText + idx * sizeof(u16) * fontSize + _ * sizeof(u16) + 1);
		tmpBuf[_] = ((u16)lo << 8) | ((u16)hi);
	}
	//memcpy(tmpBuf, fontText + idx * sizeof(u16) * fontSize, sizeof(u16) * fontSize);

	u8 i;
	s8 page;
	if(x < -fontSize)x = -fontSize;
	if(x > 127)return;
	if(y < -fontSize)y = -fontSize;
#ifdef USE_12832
	if(y > 31)return;
#else
	if(y > 63)return;
#endif
	page = x >> 3;

	for(i=0;i<fontSize;++i){
		// 如果垂直方向小于0像素则不再进行绘制
		if(y + i < 0)continue;
		// 如果垂直方向超过32或64像素则不再进行绘制
#ifdef USE_12832
		if(y + i < 32){
#else
		if(y + i < 64){
#endif
			// 获得VRAM偏移量
			s16 vramOffset = ((y + i) << 4) + page;
			// 获得页内偏移量
			u8 offsetInPage = x & 7;
			// 如果page<0则第一次绘制忽略
			if(page + 0 >= 0){
				if(vramOffset >= 0){
					// 写数据
					if(c)
						VRAM[vramOffset] |= (tmpBuf[i] << offsetInPage) & 0xFF;
					else
						VRAM[vramOffset] &= ~(tmpBuf[i] << offsetInPage) & 0xFF;
				}
			}
			// 偏移到下一个页
			vramOffset++;
			if(page + 1 >= 0){
				// 如果水平方向超过128像素则不再进行绘制
				if(vramOffset >= ((y + i + 1) << 4))continue;
				if(vramOffset >= 0){
					if(c)
						VRAM[vramOffset] |= (tmpBuf[i] >> (8 - offsetInPage)) & 0xFF;
					else
						VRAM[vramOffset] &= ~(tmpBuf[i] >> (8 - offsetInPage)) & 0xFF;
				}
			}
			// 偏移到下一个页
			vramOffset++;
			if(page + 2 >= 0){
				// 如果水平方向超过128像素则不再进行绘制
				if(vramOffset >= ((y + i + 1) << 4))continue;
				if(vramOffset >= 0){
					if(c)
						VRAM[vramOffset] |= (tmpBuf[i] >> (16 - offsetInPage)) & 0xFF;
					else
						VRAM[vramOffset] &= ~(tmpBuf[i] >> (16 - offsetInPage)) & 0xFF;
				}
			}
		}
	}
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
				LCM12864_DrawChar(fontSize, chr - 0x20, (s8)x, (s8)y, c);
			x += (fontSize >> 1);
		}else{
			chr <<= 8;
			i++;
			chr |= str[i];
			i++;
			if(x >= -fontSize && x <= 127)
				LCM12864_DrawChar(fontSize, LCM12864_GetIdxFromASCII(chr), (s8)x, (s8)y, c);
			x += fontSize;
		}
	}
}

void LCM12864_DrawStringIdx(u8 fontSize, u8 idx, s16 x, s16 y, bit c){
	u8 i = 0;
	u16* str = (u16*)strings[idx];
	while(str[i] != 0xFFFF){
		u16 _idx = str[i];
		i++;
		if(x >= -fontSize && x <= 127)
			LCM12864_DrawChar(fontSize, _idx, (s8)x, (s8)y, c);
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
	if(x > 127)return;
#ifdef USE_12832
	if(y > 31)return;
#else
	if(y > 63)return;
#endif

	if(c)
		VRAM[(x >> 3) + (y << 4)] |= 1 << (x & 7);
	else
		VRAM[(x >> 3) + (y << 4)] &= ~(1 << (x & 7));
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
	u8 i, j;
	if(x > 127)return;
	if(x + w > 128)w = 128 - x;
#ifdef USE_12832
	if(y > 31)return;
	if(y + h > 32)h = 32 - y;
#else
	if(y > 63)return;
	if(y + h > 64)h = 64 - y;
#endif

	for(i=y;i<y+h;++i){
		u8 page0 = x >> 3;
		u8 page1 = (x + w) >> 3;
		u8 _x0 = x & 7;
		u8 _x1 = (x + w) & 7;
		if(page0 == page1){
			u8 mask = 0;
			for(j=_x0;j<_x1;++j){
				mask |= (1 << j);
			}
			if(c){
				VRAM[page0 + (i << 4)] |= mask;
			}else{
				VRAM[page0 + (i << 4)] &= ~mask;
			}
		}else{
			for(j=page0;j<=page1;++j){
				if(j == page0){
					if(c)
						VRAM[j + (i << 4)] |= (0xFF << _x0);
					else
						VRAM[j + (i << 4)] &= ~(0xFF << _x0);
				}else if(j == page1){
					if(c)
						VRAM[j + (i << 4)] |= (0xFF >> (8 - _x1));
					else
						VRAM[j + (i << 4)] &= ~(0xFF >> (8 - _x1));
				}else{
					if(c)
						VRAM[j + (i << 4)] = 0xFF;
					else
						VRAM[j + (i << 4)] = 0;
				}
			}
		}
	}
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
	u16* str = (u16*)strings[idx];
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
