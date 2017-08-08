#include <STRING.H>
#include "ModuleTurboSetting.h"
#include "main.h"
#include "LCM12864.h"
#include "SaveData.h"
#include "Controller.h"


#define CONFIG_ITEM_COUNT	3


static const char* code items[] = {
	"设定键",
	"取消键",
	"连发速度",
};
static const char* code wiiButtons[] = {
	"无",
	"\x80\x00",
	"\x80\x01",
	"\x80\x02",
	"\x80\x03",
	"\x80\x10",
	"\x80\x11",
	"\x80\x12",
	"\x80\x13",
	"\x80\x20\x80\x21",
	"\x80\x22\x80\x23",
	"\x80\x24\x80\x25",
	"\x80\x26\x80\x27",
	"\x80\x40",
	"\x80\x41",
	"\x80\x42",
};
static const char* code freq[] = {
	"每秒10次",
	"每秒15次",
	"每秒20次",
	"每秒25次",
	"每秒30次",
};


static void Init(){
	SaveData_Read();
	
	if(1){
		u8 len = LCM12864_GetStrLenFromIdx(10, TITLE_TURBO_SETTING) - 1;
		LCM12864_FillRect(((128 - len) >> 1) - 1, 0, len + 2, 11, 1);
		LCM12864_DrawStringIdx(10, TITLE_TURBO_SETTING, (128 - len) >> 1, 1, 0);	// 连发设定
		LCM12864_DrawString(10, "=", 61, 12, 1);
	}
}

void ModuleTurboSetting(){
	s16 offset = 128 << 4;	// 定点数
	s8 selectedItem = 0;
	
	Init();
	
	while(1){
		if(1){
			u16 len = LCM12864_GetStrLenFromIdx(10, DESC_5) - 1;

			LCM12864_FillRect(0, 12, 40, 10, 0);
			LCM12864_DrawStringIdx(10, TURBO_KEY_ENABLE + selectedItem, 0, 12, 1);	// 设定键 取消键 连发速度
			
			LCM12864_FillRect(89, 12, 39, 10, 0);
			switch(selectedItem){
				case 0:
				case 1:{
					LCM12864_DrawStringIdx(10, TURBO_WII, 89, 12, 1);	// WII
					LCM12864_DrawStringIdx(10, TURBO_WII_NONE + (selectedItem ? saveData.turboDisableKey : saveData.turboEnableKey), 109, 12, 1);	// 无 左 右 上 下 A B X Y L R ZL ZR + - H
				}break;
				case 2:{
					LCM12864_DrawStringIdx(10, TURBO_10 + saveData.turboFreq, 89, 12, 1);	// 每秒10次 每秒15次 每秒20次 每秒25次 每秒30次
				}break;
			}
			
			LCM12864_FillRect(0, 22, 128, 10, 0);
			LCM12864_DrawStringIdx(10, DESC_5, offset >> 4, 22, 1);	// 上/下选择设定项目 左/右调整设定值 A保存设定并返回设置菜单 B不保存设定直接返回设置菜单
			
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
					saveData.turboEnableKey--;
					if(saveData.turboEnableKey < 0)saveData.turboEnableKey = KEY_ITEM_COUNT;
					break;
				case 1:
					saveData.turboDisableKey--;
					if(saveData.turboDisableKey < 0)saveData.turboDisableKey = KEY_ITEM_COUNT;
					break;
				case 2:
					saveData.turboFreq--;
					if(saveData.turboFreq < 0)saveData.turboFreq = 5 - 1;
					break;
			}
		}
		if(Controller_IsKeyDown(WII_CLASSIC_CONTROLLER_BUTTON_RIGHT)){
			switch(selectedItem){
				case 0:
					saveData.turboEnableKey++;
					if(saveData.turboEnableKey > KEY_ITEM_COUNT)saveData.turboEnableKey = 0;
					break;
				case 1:
					saveData.turboDisableKey++;
					if(saveData.turboDisableKey > KEY_ITEM_COUNT)saveData.turboDisableKey = 0;
					break;
				case 2:
					saveData.turboFreq++;
					if(saveData.turboFreq > 5 - 1)saveData.turboFreq = 0;
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
