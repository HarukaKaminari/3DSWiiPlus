#include <STRING.H>
#include "ModuleAnalogCalibration.h"
#include "main.h"
#include "LCM12864.h"
#include "SaveData.h"
#include "PWM.h"
#include "Controller.h"


static void Init(){
	SaveData_Read();
	
	if(1){
		u8 len = LCM12864_GetStrLenFromIdx(10, TITLE_ANALOG_CALIBRATION) - 1;
		LCM12864_FillRect(((128 - len) >> 1) - 1, 0, len + 2, 11, 1);
		LCM12864_DrawStringIdx(10, TITLE_ANALOG_CALIBRATION, (128 - len) >> 1, 1, 0);
		LCM12864_DrawStringIdx(10, CALIBRATION_THRESTHOLD, 0, 12, 1);
		LCM12864_DrawStringIdx(10, CALIBRATION_OFFSET, 64, 12, 1);
	}
}

void ModuleAnalogCalibration(){
	s16 offset = 128 << 4;	// 定点数
	s8 selectedItem = 0;
	
	Init();
	
	PWM_SetChannelStatus(0x0F);
	PWM_Start();
	
	while(1){
		s16 tlx = 0, tly = 0, trx = 0, try = 0;
		if(1){
			s16 len = LCM12864_GetStrLenFromIdx(10, DESC_4) - 1;
			u8 strRatio[6];
			u8 strOffset[6];
			Hex2String(saveData.analogRatioL, strRatio + 0);
			strRatio[2] = ',';
			Hex2String(saveData.analogRatioR, strRatio + 3);
			Hex2String(saveData.analogOffsetL, strOffset + 0);
			strOffset[2] = ',';
			Hex2String(saveData.analogOffsetR, strOffset + 3);
			
			LCM12864_FillRect(25, 12, 15, 10, 0);
			LCM12864_DrawString(10, strRatio, 25, 12, 1);
			
			LCM12864_FillRect(99, 12, 15, 10, 0);
			LCM12864_DrawString(10, strOffset, 99, 12, 1);
			
			LCM12864_FillRect(0, 22, 128, 10, 0);
			LCM12864_DrawStringIdx(10, DESC_4, offset >> 4, 22, 1);	// L/R选择设定的摇杆 上/下调整阈值 左/右调整偏移量 A保存设定并返回设置菜单 B不保存设定直接返回设置菜单

			offset -= 8;
			if(offset < -(len << 4))offset = 128 << 4;
		}
		LCM12864_Refresh();
		
		Controller_Read();
		
		if(Controller_IsKeyHold(WII_CLASSIC_CONTROLLER_BUTTON_UP)){
			if(Controller_IsKeyHold(WII_CLASSIC_CONTROLLER_BUTTON_L)){
				saveData.analogRatioL--;
				saveData.analogRatioL &= 0x80;
			}
			if(Controller_IsKeyHold(WII_CLASSIC_CONTROLLER_BUTTON_R)){
				saveData.analogRatioR--;
				saveData.analogRatioR &= 0x80;
			}
		}
		if(Controller_IsKeyHold(WII_CLASSIC_CONTROLLER_BUTTON_DOWN)){
			if(Controller_IsKeyHold(WII_CLASSIC_CONTROLLER_BUTTON_L)){
				saveData.analogRatioL++;
				saveData.analogRatioL &= 0x80;
			}
			if(Controller_IsKeyHold(WII_CLASSIC_CONTROLLER_BUTTON_R)){
				saveData.analogRatioR++;
				saveData.analogRatioR &= 0x80;
			}
		}
		if(Controller_IsKeyHold(WII_CLASSIC_CONTROLLER_BUTTON_LEFT)){
			if(Controller_IsKeyHold(WII_CLASSIC_CONTROLLER_BUTTON_L)){
				saveData.analogOffsetL--;
				saveData.analogOffsetL &= 0x80;
			}
			if(Controller_IsKeyHold(WII_CLASSIC_CONTROLLER_BUTTON_R)){
				saveData.analogOffsetR--;
				saveData.analogOffsetR &= 0x80;
			}
		}
		if(Controller_IsKeyHold(WII_CLASSIC_CONTROLLER_BUTTON_RIGHT)){
			if(Controller_IsKeyHold(WII_CLASSIC_CONTROLLER_BUTTON_L)){
				saveData.analogOffsetL++;
				saveData.analogOffsetL &= 0x80;
			}
			if(Controller_IsKeyHold(WII_CLASSIC_CONTROLLER_BUTTON_R)){
				saveData.analogOffsetR++;
				saveData.analogOffsetR &= 0x80;
			}
		}
		if(Controller_IsKeyDown(WII_CLASSIC_CONTROLLER_BUTTON_A)){
			SaveData_Flush();
			
			currentModule = Module_ConfigMenu;
			PWM_Stop();
			return;
		}
		if(Controller_IsKeyDown(WII_CLASSIC_CONTROLLER_BUTTON_B)){
			currentModule = Module_ConfigMenu;
			PWM_Stop();
			return;
		}
		
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
}
