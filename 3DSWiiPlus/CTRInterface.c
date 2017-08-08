#include "CTRInterface.h"
#include "PWM.h"
#include "Timer.h"
#include "SaveData.h"
#include "Controller.h"


static const u16 code map[] = {
	WII_CLASSIC_CONTROLLER_BUTTON_DUMMY,
	WII_CLASSIC_CONTROLLER_BUTTON_LEFT,
	WII_CLASSIC_CONTROLLER_BUTTON_RIGHT,
	WII_CLASSIC_CONTROLLER_BUTTON_UP,
	WII_CLASSIC_CONTROLLER_BUTTON_DOWN,
	WII_CLASSIC_CONTROLLER_BUTTON_A,
	WII_CLASSIC_CONTROLLER_BUTTON_B,
	WII_CLASSIC_CONTROLLER_BUTTON_X,
	WII_CLASSIC_CONTROLLER_BUTTON_Y,
	WII_CLASSIC_CONTROLLER_BUTTON_L,
	WII_CLASSIC_CONTROLLER_BUTTON_R,
	WII_CLASSIC_CONTROLLER_BUTTON_ZL,
	WII_CLASSIC_CONTROLLER_BUTTON_ZR,
	WII_CLASSIC_CONTROLLER_BUTTON_PLUS,
	WII_CLASSIC_CONTROLLER_BUTTON_MINUS,
	WII_CLASSIC_CONTROLLER_BUTTON_HOME,
};
static const u8 code freq[] = {100, 66, 50, 40, 33};

static u16 turboStatus = 0;
static vu8 isDown = 0;


static void Timer_Interrupt(){
	isDown = ~isDown;
}

void CTRInterface_Init(){
	P0M0 |= 0xFF;
	P0M1 |= 0xFF;
	P4M0 |= 0xFF;
	P4M1 |= 0xFF;			// 将P0和P4口设置为开漏
}

void CTRInterface_Prepare(){
	Timer_SetInterval(freq[saveData.turboFreq]);
	Timer_SetCallback(Timer_Interrupt);
	Timer_Start();
	
	PWM_SetChannelStatus(0x0F);
	PWM_Start();
}

void CTRInterface_Update(){
	u8 i;
	u16 buttons = 0;
	s16 tlx = 0, tly = 0, trx = 0, try = 0;
	
	Controller_Read();
	
	if(saveData.turboEnableKey && Controller_IsKeyHold(map[saveData.turboEnableKey])){
		for(i=0;i<KEY_ITEM_COUNT+1;++i){
			if(i == saveData.turboEnableKey || i == saveData.turboDisableKey)continue;
			if(Controller_IsKeyHold(map[i]))turboStatus |= (1 << i);
		}
	}else if(saveData.turboDisableKey && Controller_IsKeyHold(map[saveData.turboDisableKey])){
		for(i=0;i<KEY_ITEM_COUNT+1;++i){
			if(i == saveData.turboEnableKey || i == saveData.turboDisableKey)continue;
			if(Controller_IsKeyHold(map[i]))turboStatus &= ~(1 << i);
		}
	}
	
	for(i=0;i<KEY_ITEM_COUNT;++i){
		if(Controller_IsKeyHold(map[i + 1]))buttons |= (1 << saveData.keyMapping[i]);
		if(turboStatus & (1 << (i + 1)) & isDown)buttons &= ~(1 << saveData.keyMapping[i]);
	}
	
	P0 = ~(buttons & 0xFF);
	P4 = ~((buttons >> 8) & 0xFF);
	
	if(saveData.analogMapping){
		trx = Controller_GetLAnalogX();
		try = Controller_GetLAnalogY();
		tlx = Controller_GetRAnalogX();
		tly = Controller_GetRAnalogY();
	}else{
		tlx = Controller_GetLAnalogX();
		tly = Controller_GetLAnalogY();
		trx = Controller_GetRAnalogX();
		try = Controller_GetRAnalogY();
	}
	if(saveData.reverseAnalogX){
		tlx = -tlx;
		trx = -trx;
	}
	if(saveData.reverseAnalogY){
		tly = -tly;
		try = -try;
	}
	tlx *= saveData.analogRatioL;
	tlx >>= 6;
	tlx += saveData.analogOffsetL;
	tly *= saveData.analogRatioL;
	tly >>= 6;
	tly += saveData.analogOffsetL;
	if(tlx < 0)tlx = 0;
	if(tlx > 0xFF)tlx = 0xFF;
	if(tly < 0)tly = 0;
	if(tly > 0xFF)tly = 0xFF;
	trx *= saveData.analogRatioR;
	trx >>= 6;
	trx += saveData.analogOffsetR;
	try *= saveData.analogRatioR;
	try >>= 6;
	try += saveData.analogOffsetR;
	if(trx < 0)trx = 0;
	if(trx > 0xFF)trx = 0xFF;
	if(try < 0)try = 0;
	if(try > 0xFF)try = 0xFF;
	PWM_SetDuty(0, tlx);
	PWM_SetDuty(1, tly);
	PWM_SetDuty(2, trx);
	PWM_SetDuty(3, try);
}
