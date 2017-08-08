#ifndef IST3932_H_
#define IST3932_H_


#include "Utility.h"


void IST3932_Init();
void IST3932_Refresh();

void IST3932_DrawChar(u8 fontSize, s8 x, s8 y, bit c);
void IST3932_DrawPixel(u8 x, u8 y, bit c);
void IST3932_FillRect(u8 x, u8 y, u8 w, u8 h, bit c);


#endif	/* IST3932_H_ */
