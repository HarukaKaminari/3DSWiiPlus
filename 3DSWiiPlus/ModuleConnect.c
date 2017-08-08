#include "ModuleConnect.h"
#include "main.h"
#include "LCM12864.h"
#include "Controller.h"


static void Init(){
	
}

void ModuleConnect(){
	u16 len;
	
	Init();
	
	if(Controller_IsConnected()){
		currentModule = Module_Countdown;
		return;
	}
	
	LCM12864_Clear();
	len = LCM12864_GetStrLenFromIdx(10, JOYSTICK_NOT_FOUND) - 1;
	LCM12864_DrawStringIdx(10, JOYSTICK_NOT_FOUND, (128 - len) >> 1, 32 - 10, 1);	// 未检测到手柄！
	len = LCM12864_GetStrLenFromIdx(10, RESTART) - 1;
	LCM12864_DrawStringIdx(10, RESTART, (128 - len) >> 1, 32, 1);	// 设备将自动重启！
	LCM12864_Refresh();
	Delay(1000);
	SoftReset();
}
