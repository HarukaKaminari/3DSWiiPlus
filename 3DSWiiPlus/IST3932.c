#ifdef LCM_DRIVER_IST3932

#define USE_TABLE


#include "IST3932.h"
#include "LCM12864.h"
#include "SPI.h"


// 显存缓冲区数据格式
//		0		1		2		.	F
//   +-------+-------+-------+---+-------+
// 00|Byte000|Byte001|Byte002|...|Byte00F|
//   +-------+-------+-------+---+-------+
// 01|Byte010|Byte011|Byte012|...|Byte01F|
//   +-------+-------+-------+---+-------+
// ..|.......|.......|.......|...|.......|
//   +-------+-------+-------+---+-------+
// 1F|Byte1F0|Byte1F1|Byte1F2|...|Byte1FF|
//   +-------+-------+-------+---+-------+ <--- 12832到此为止
// ..|.......|.......|.......|...|.......|
//   +-------+-------+-------+---+-------+
// 3F|Byte3F0|Byte3F1|Byte3F2|...|Byte3FF|
//   +-------+-------+-------+---+-------+ <--- 12864到此为止


/*
设置行地址。显然row取值范围只能为0~31或63
*/
static void IST3932_SetRow(u8 row){
	LCM12864_SendCommand(row & 0xF);
	LCM12864_SendCommand(0x10 | (row >> 4));
}

/*
屏幕上电
*/
static void IST3932_Power(bit isOn){
	if(isOn == 0){
		LCM12864_SendCommand(0x2D);
	}else{
		LCM12864_SendCommand(0x2F);
	}
}

/*
设置偏压比。取值范围为5、1、0、2、3、4。貌似和对比度有关……
*/
static void IST3932_SetBias(u8 bias){
	LCM12864_SendCommand(0x30 | (bias & 0x7));
}

/*
睡眠模式
*/
static void IST3932_Sleep(bit isSleep){
	if(isSleep == 0)
		LCM12864_SendCommand(0x38);
	else
		LCM12864_SendCommand(0x39);
}

/*
未知
*/
static void IST3932_Osc(bit isOff){
	if(isOff == 0){
		LCM12864_SendCommand(0x3A);
	}else{
		LCM12864_SendCommand(0x3B);
	}
}

/*
开/关显示
*/
static void IST3932_Display(bit isOn){
	if(isOn == 0){
		LCM12864_SendCommand(0x3C);
	}else{
		LCM12864_SendCommand(0x3D);
	}
}

/*
设置起始地址。取值范围为0~64。貌似是用来设置垂直卷轴偏移量的
*/
static void IST3932_SetStartingLine(u8 line){
	LCM12864_SendCommand(0x40 | (line & 0xF));
	LCM12864_SendCommand(0x50 | (line >> 4));
}

/*
设置驱动显示。第一个参数代表是否硬件全屏垂直镜像，第二个参数与硬件全屏水平镜像有关（但是由于map问题会导致错乱，最好保持为0），第三个参数代表是否全亮，第四个参数代表是否反显
*/
static void IST3932_DriverControl(bit isVFlip, bit isADC, bit isDisplayAll, bit isNegative){
	u8 cmd = 0x60;
	if(isVFlip)cmd |= (1 << 3);
	if(isADC)cmd |= (1 << 2);
	if(isDisplayAll)cmd |= (1 << 1);
	if(isNegative)cmd |= (1 << 0);
	LCM12864_SendCommand(cmd);
}

/*
软复位
*/
static void IST3932_SoftReset(){
	LCM12864_SendCommand(0x76);
}

/*
设置占空比。取值范围为1~65。貌似和对比度有关
*/
static void IST3932_SetDuty(u8 duty){
	LCM12864_SendCommand(0x90 | (duty & 0xF));
	LCM12864_SendCommand(0xA0 | (duty >> 4));
}

/*
设置列地址。col取值范围为0~31
*/
static void IST3932_SetCol(u8 col){
	LCM12864_SendCommand(0xC0 | (col & 0x1F));
}

/*
设置对比度。取值范围为0~255
*/
static void IST3932_SetContrast(u8 contrast){
	LCM12864_SendCommand(0xB1);
	LCM12864_SendCommand(contrast);
}

/*
未知
*/
static void IST3932_SetFrameControl(u8 value0, u8 value1){
	LCM12864_SendCommand(0xB2);
	LCM12864_SendCommand(value0);
	LCM12864_SendCommand(value1);
}

/*
空指令。同时也是退出编程模式和IST模式的指令。不过，我并不打算用编程模式……
*/
static void IST3932_Nop(){
	LCM12864_SendCommand(0xE3);
}

/*
进入IST模式
*/
static void IST3932_BeginIST(){
	LCM12864_SendCommand(0x88);
	LCM12864_SendCommand(0x88);
	LCM12864_SendCommand(0x88);
	LCM12864_SendCommand(0x88);
}

/*
设置行交错模式
*/
static void IST3932_SetInterleave(bit isInterleave){
	if(isInterleave == 0){
		LCM12864_SendCommand(0x60);
	}else{
		LCM12864_SendCommand(0x61);
	}
}

/*
退出IST模式。等效于NOP
*/
static void IST3932_EndIST(){
	LCM12864_SendCommand(0xE3);
}

void IST3932_Init(){
	LCM12864_CS = 0;

	// 首先延时100ms令自动硬件复位完成
	Delay(100);
	// Set Power
	IST3932_Power(1);
	// Set Bias
	IST3932_SetBias(3);
	// Set Contrast
	IST3932_SetContrast(0xDF);
	// Set FrameControl
	IST3932_SetFrameControl(0x90, 0x01);
	// Set Duty
	IST3932_SetDuty(0x40);
	// Set Driver Display Control
	IST3932_DriverControl(0, 0, 0, 0);
	// Set Display ON
	IST3932_Display(1);
	// Set Interleave
	IST3932_BeginIST();
	IST3932_SetInterleave(0);
	IST3932_EndIST();

	LCM12864_CS = 1;
}

void IST3932_Refresh(){
	u8 x, y;
	LCM12864_CS = 0;
#ifdef USE_12832
	for(y=0;y<32;++y){
#else
	for(y=0;y<64;++y){
#endif
		IST3932_SetRow(y);
		IST3932_SetCol(0);
		for(x=0;x<16;++x){
			LCM12864_SendData(VRAM[15 - x + (y << 4)]);
		}
	}
	LCM12864_CS = 1;
}

/*
将字符缓冲区中的字符绘制到VRAM缓冲区
x最小取值为-fontSize，最大取值为127
y最小取值为-fontSize，最大取值为31或63
*/
void IST3932_DrawChar(u8 fontSize, s8 x, s8 y, bit c){
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

/*
在缓冲区任意位置画点
*/
void IST3932_DrawPixel(u8 x, u8 y, bit c){
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
在缓冲区任意位置填充矩形
*/
void IST3932_FillRect(u8 x, u8 y, u8 w, u8 h, bit c){
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


#endif
