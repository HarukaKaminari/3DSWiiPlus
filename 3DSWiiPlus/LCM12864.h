#ifndef LCM_12864_H_
#define LCM_12864_H_


#include "Utility.h"


sbit LCM12864_CS	= P1^0;	// 片选（/CS）
sbit LCM12864_CD	= P1^1;	// 指令/数据寄存器选择
sbit LCM12864_LED	= P1^2;	// 背光


//#define USE_12832


#ifdef USE_12832
extern u8 xdata VRAM[512];		// 12832的显存为512字节
#else
extern u8 xdata VRAM[1024];		// 12864的显存为1K字节
#endif
extern u16 xdata tmpBuf[0x10];	// 16*2的字模缓冲区


void LCM12864_SendCommand(u8 value);
void LCM12864_SendData(u8 value);

void LCM12864_Init();
void LCM12864_Clear();
void LCM12864_Refresh();

void LCM12864_Backlight(bit isOn);

void LCM12864_DrawChar(u8 fontSize, u16 idx, s8 x, s8 y, bit c);
void LCM12864_DrawString(u8 fontSize, const char* str, s16 x, s16 y, bit c);
void LCM12864_DrawStringIdx(u8 fontSize, u8 idx, s16 x, s16 y, bit c);
void LCM12864_DrawPixel(u8 x, u8 y, bit c);
void LCM12864_DrawLine(u8 x0, u8 y0, u8 x1, u8 y1, bit c);
void LCM12864_FillRect(u8 x, u8 y, u8 w, u8 h, bit c);
void LCM12864_DrawCircle(u8 x1, u8 y1, u8 r, bit c);
u16 LCM12864_GetStrLenFromIdx(u8 fontSize, u8 idx);


#endif	/* LCM_12864_H_ */
