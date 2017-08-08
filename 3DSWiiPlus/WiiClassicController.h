#ifndef WII_CLASSIC_CONTROLLER_H_
#define WII_CLASSIC_CONTROLLER_H_


#include "Utility.h"


#define WII_CLASSIC_CONTROLLER_ID	0x52


#define WII_CLASSIC_CONTROLLER_BUTTON_DUMMY	(1 << 0)
#define WII_CLASSIC_CONTROLLER_BUTTON_R		(1 << 1)
#define WII_CLASSIC_CONTROLLER_BUTTON_PLUS	(1 << 2)
#define WII_CLASSIC_CONTROLLER_BUTTON_HOME	(1 << 3)
#define WII_CLASSIC_CONTROLLER_BUTTON_MINUS	(1 << 4)
#define WII_CLASSIC_CONTROLLER_BUTTON_L		(1 << 5)
#define WII_CLASSIC_CONTROLLER_BUTTON_DOWN	(1 << 6)
#define WII_CLASSIC_CONTROLLER_BUTTON_RIGHT	(1 << 7)
#define WII_CLASSIC_CONTROLLER_BUTTON_UP	(1 << 8)
#define WII_CLASSIC_CONTROLLER_BUTTON_LEFT	(1 << 9)
#define WII_CLASSIC_CONTROLLER_BUTTON_ZR	(1 << 10)
#define WII_CLASSIC_CONTROLLER_BUTTON_X		(1 << 11)
#define WII_CLASSIC_CONTROLLER_BUTTON_A		(1 << 12)
#define WII_CLASSIC_CONTROLLER_BUTTON_Y		(1 << 13)
#define WII_CLASSIC_CONTROLLER_BUTTON_B		(1 << 14)
#define WII_CLASSIC_CONTROLLER_BUTTON_ZL	(1 << 15)


void WiiClassicController_Init();
void WiiClassicController_Read();
u8* WiiClassicController_GetPtr();
bit WiiClassicController_IsKeyDown(u16 key);
bit WiiClassicController_IsKeyHold(u16 key);
bit WiiClassicController_IsKeyUp(u16 key);
s8 WiiClassicController_GetLAnalogX();
s8 WiiClassicController_GetLAnalogY();
s8 WiiClassicController_GetRAnalogX();
s8 WiiClassicController_GetRAnalogY();



#endif	/* WII_CLASSIC_CONTROLLER_H_ */
