#include <INTRINS.h>
#include "WiiClassicController.h"
#include "I2C.h"
#include "Uart.h"


typedef struct WiiControllerData{
	u8 xLAnalog;
	u8 yLAnalog;
	
	u8 xRAnalog;
	u8 yRAnalog;
	
	u8 LTrigger;
	u8 RTrigger;
	
	u16 buttons;
}structWiiControllerData;


static structWiiControllerData wiiControllerData;
static u16 prevButtons = 0;


static u8 Decrypt(u8 x){
	return x;
	//return (x ^ 0x17) + 0x17;
}

void WiiClassicController_Init(){
	s8 result;
	u8 buf[6];
	u8 i;
	
	Uart_String("Initializing Wii Classic Controller...\r\n");
	
	buf[0] = 0xF0;
	buf[1] = 0x55;
	result = I2C_Puts(WII_CLASSIC_CONTROLLER_ID, 0, 0, buf, 2);
	while(result != 0);
	
	Delay(1);
	
	buf[0] = 0xFB;
	buf[1] = 0x00;
	result = I2C_Puts(WII_CLASSIC_CONTROLLER_ID, 0, 0, buf, 2);
	while(result != 0);
	
	Delay(1);
	
	Uart_String("Reading ID...\r\n");
	
	buf[0] = 0xFA;
	result = I2C_Puts(WII_CLASSIC_CONTROLLER_ID, 0, 0, buf, 1);
	while(result != 0);
	
	result = I2C_Gets(WII_CLASSIC_CONTROLLER_ID, 0, 0, buf, 6);
	while(result != 0);
	
	Uart_String("ID=");
	for(i=0;i<6;++i){
		Uart_Hex(buf[i]);
		Uart_String(" ");
	}
	Uart_String("\r\n\r\n");
	
	Delay(1);
	
	Uart_String("Wii Classic Controller Initialization Finished.\r\n");
}

static void WiiClassicController_GetData(){
	u8 buf[6];
	s8 result;
	u8 i;
	
	buf[0] = 0;
	result = I2C_Puts(WII_CLASSIC_CONTROLLER_ID, 0, 0, buf, 1);
	while(result != 0);
	
	Delay(0);
	
	result = I2C_Gets(WII_CLASSIC_CONTROLLER_ID, 0, 0, buf, 6);
	while(result != 0);
	
	for(i=0;i<6;++i)buf[i] = Decrypt(buf[i]);
	
	wiiControllerData.xLAnalog = (buf[0] & 0x3F);
	wiiControllerData.yLAnalog = (buf[1] & 0x3F);
	
	wiiControllerData.xRAnalog = ((buf[0] >> 6) & 0x03) << 3;
	wiiControllerData.xRAnalog |= ((buf[1] >> 6) & 0x03) << 1;
	wiiControllerData.xRAnalog |= ((buf[2] >> 7) & 0x01);
	
	wiiControllerData.yRAnalog = buf[2] & 0x1F;
	
	wiiControllerData.LTrigger = ((buf[2] >> 5) & 0x03) << 3;
	wiiControllerData.LTrigger |= (buf[3] >> 5) & 0x07;
	
	wiiControllerData.RTrigger = buf[3] & 0x1F;
	
	wiiControllerData.buttons = buf[5];
	wiiControllerData.buttons <<= 8;
	wiiControllerData.buttons |= buf[4];
	wiiControllerData.buttons = ~wiiControllerData.buttons;
}

void WiiClassicController_Read(){
	prevButtons = wiiControllerData.buttons;
	WiiClassicController_GetData();
}

u8* WiiClassicController_GetPtr(){
	return &wiiControllerData;
}

bit WiiClassicController_IsKeyDown(u16 key){
	return (~prevButtons & wiiControllerData.buttons & key);
}

bit WiiClassicController_IsKeyHold(u16 key){
	return (wiiControllerData.buttons & key);
}

bit WiiClassicController_IsKeyUp(u16 key){
	return (prevButtons & ~wiiControllerData.buttons & key);
}

s8 WiiClassicController_GetLAnalogX(){
	return wiiControllerData.xLAnalog - 32;
}

s8 WiiClassicController_GetLAnalogY(){
	return wiiControllerData.yLAnalog - 32;
}

s8 WiiClassicController_GetRAnalogX(){
	return (wiiControllerData.xRAnalog << 1) - 32;
}

s8 WiiClassicController_GetRAnalogY(){
	return (wiiControllerData.yRAnalog << 1) - 32;
}
