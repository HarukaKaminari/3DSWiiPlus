#ifndef IAP_H_
#define IAP_H_


#include "Utility.h"


void IAP_Idle();
u8 IAP_ReadByte(u16 addr);
void IAP_ProgramByte(u16 addr, u8 dat);
void IAP_EraseSector(u16 addr);


#endif	/* IAP_H_ */
