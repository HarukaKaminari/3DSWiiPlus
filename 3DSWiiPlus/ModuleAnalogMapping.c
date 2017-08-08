#include <STRING.H>
#include "ModuleAnalogMapping.h"
#include "main.h"
#include "LCM12864.h"
#include "SaveData.h"
#include "Controller.h"


#define CONFIG_ITEM_COUNT	3


static void Init(){
	SaveData_Read();
	
	if(1){
		u8 len = LCM12864_GetStrLenFromIdx(10, TITLE_ANALOG_MAPPING) - 1;
		LCM12864_FillRect(((128 - len) >> 1) - 1, 0, len + 2, 11, 1);
		LCM12864_DrawStringIdx(10, TITLE_ANALOG_MAPPING, (128 - len) >> 1, 1, 0);	// 摇杆映射
		LCM12864_DrawString(10, "=", 61, 12, 1);
	}
}

void ModuleAnalogMapping(){
	s16 offset = 128 << 4;	// 定点数
	s8 selectedItem = 0;

	Init();
	
	while(1){
		if(1){
			u16 len;
			
			LCM12864_FillRect(0, 12, 60, 10, 0);
			LCM12864_DrawStringIdx(10, ANALOG_SWAP + selectedItem, 0, 12, 1);	// 交换左右摇杆 x轴反向 y轴反向
			
			LCM12864_FillRect(79, 12, 49, 10, 0);
			switch(selectedItem){
				case 0:	LCM12864_DrawStringIdx(10, ANALOG_NO + saveData.analogMapping, 119, 12, 1);	break;	// 否 是
				case 1:	LCM12864_DrawStringIdx(10, ANALOG_NO + saveData.reverseAnalogX, 119, 12, 1);break;	// 否 是
				case 2:	LCM12864_DrawStringIdx(10, ANALOG_NO + saveData.reverseAnalogY, 119, 12, 1);break;	// 否 是
			}
			
			len = LCM12864_GetStrLenFromIdx(10, DESC_3) - 1;
			LCM12864_FillRect(0, 22, 128, 10, 0);
			LCM12864_DrawStringIdx(10, DESC_3, offset >> 4, 22, 1);	// 上/下选择设定项目 左/右调整设定值 A保存设定并返回设置菜单 B不保存设定直接返回设置菜单

			offset -= 8;
			if(offset < -(len << 4))offset = 128 << 4;
		}
		LCM12864_Refresh();
		
		Controller_Read();
		
		if(Controller_IsKeyDown(WII_CLASSIC_CONTROLLER_BUTTON_UP)){
			selectedItem--;
			if(selectedItem < 0)selectedItem = CONFIG_ITEM_COUNT - 1;
		}
		if(Controller_IsKeyDown(WII_CLASSIC_CONTROLLER_BUTTON_DOWN)){
			selectedItem++;
			if(selectedItem > CONFIG_ITEM_COUNT - 1)selectedItem = 0;
		}
		if(Controller_IsKeyDown(WII_CLASSIC_CONTROLLER_BUTTON_LEFT)){
			switch(selectedItem){
				case 0:
					saveData.analogMapping--;
					saveData.analogMapping &= 1;
					break;
				case 1:
					saveData.reverseAnalogX--;
					saveData.reverseAnalogX &= 1;
					break;
				case 2:
					saveData.reverseAnalogY--;
					saveData.reverseAnalogY &= 1;
					break;
			}
		}
		if(Controller_IsKeyDown(WII_CLASSIC_CONTROLLER_BUTTON_RIGHT)){
			switch(selectedItem){
				case 0:
					saveData.analogMapping++;
					saveData.analogMapping &= 1;
					break;
				case 1:
					saveData.reverseAnalogX++;
					saveData.reverseAnalogX &= 1;
					break;
				case 2:
					saveData.reverseAnalogY++;
					saveData.reverseAnalogY &= 1;
					break;
			}
		}
		if(Controller_IsKeyDown(WII_CLASSIC_CONTROLLER_BUTTON_A)){
			SaveData_Flush();
			
			currentModule = Module_ConfigMenu;
			return;
		}
		if(Controller_IsKeyDown(WII_CLASSIC_CONTROLLER_BUTTON_B)){
			currentModule = Module_ConfigMenu;
			return;
		}
	}
}
