#ifndef UC1701X_H_
#define UC1701X_H_


#include "Utility.h"


void UC1701X_Init();
void UC1701X_Refresh();

void UC1701X_DrawChar(u8 fontSize, s8 x, s8 y, bit c);
void UC1701X_DrawPixel(u8 x, u8 y, bit c);
void UC1701X_FillRect(u8 x, u8 y, u8 w, u8 h, bit c);


#endif	/* UC1701X_H_ */
