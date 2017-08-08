#ifndef CONTROLLER_H_
#define CONTROLLER_H_


#include "Utility.h"
#include "WiiClassicController.h"


void Controller_Init();
void Controller_Read();
bit Controller_IsConnected();
#define Controller_IsKeyDown(key)	WiiClassicController_IsKeyDown(key)
#define Controller_IsKeyHold(key)	WiiClassicController_IsKeyHold(key)
#define Controller_IsKeyUp(key)		WiiClassicController_IsKeyUp(key)
#define Controller_GetLAnalogX		WiiClassicController_GetLAnalogX
#define Controller_GetLAnalogY		WiiClassicController_GetLAnalogY	
#define Controller_GetRAnalogX		WiiClassicController_GetRAnalogX
#define Controller_GetRAnalogY		WiiClassicController_GetRAnalogY


#endif	/* CONTROLLER_H_ */
