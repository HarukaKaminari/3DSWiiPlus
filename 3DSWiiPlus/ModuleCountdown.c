#include "ModuleCountdown.h"
#include "main.h"
#include "LCM12864.h"
#include "Timer.h"
#include "Controller.h"


static vu8 countdown = 4;


static void Timer_Interrupt(){
	countdown--;
}

static void Init(){
	Timer_SetInterval(1000);
	Timer_SetCallback(Timer_Interrupt);
	Timer_Start();
	
	if(1){
		LCM12864_FillRect(1, 17, 126, 30, 1);
		LCM12864_DrawStringIdx(10, ENTER_SETUP_WITHIN_3_SECS, 7, 27, 0);	// 3秒内按HOME键进入设置菜单
	}
}

void ModuleCountdown(){
	Init();
	
	while(1){
		if(1){
			LCM12864_FillRect(7, 27, 5, 10, 1);
			LCM12864_DrawChar(10, countdown + '0' - 0x20, 7, 27, 0);
		}
		
		LCM12864_Refresh();
		
		Controller_Read();
		if(Controller_IsKeyDown(WII_CLASSIC_CONTROLLER_BUTTON_HOME)){
			Timer_Stop();
			currentModule = Module_ConfigMenu;
			return;
		}
		
		if(countdown == 0){
			Timer_Stop();
			currentModule = Module_Generic;
			return;
		}
	}
}
