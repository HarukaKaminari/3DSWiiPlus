#ifndef SSD_1306_H_
#define SSD_1306_H_


#include "Utility.h"


void SSD1306_Init();
void SSD1306_Refresh();

void SSD1306_DrawChar(u8 fontSize, s8 x, s8 y, bit c);
void SSD1306_DrawPixel(u8 x, u8 y, bit c);
void SSD1306_FillRect(u8 x, u8 y, u8 w, u8 h, bit c);


#endif	/* SSD_1306_H_ */
