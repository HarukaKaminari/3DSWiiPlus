#include <STRING.H>
#include "ModuleDeviceSetting.h"
#include "main.h"
#include "LCM12864.h"
#include "SaveData.h"
#include "Controller.h"


#define CONFIG_ITEM_COUNT	3


static const char* code items[] = {
	"粗调对比度",
	"微调对比度",
	"偏压比设定",
};


static void Init(){
	SaveData_Read();
	
	if(1){
		const char* code title = "设备调整";
		u8 len = strlen(title) * (10 >> 1) - 1;
		LCM12864_FillRect(((128 - len) >> 1) - 1, 0, len + 2, 11, 1);
		LCM12864_DrawString(10, title, (128 - len) >> 1, 1, 0);
		LCM12864_DrawString(10, "=", 61, 12, 1);
	}
}

void ModuleDeviceSetting(){
	s16 offset = 128 << 4;	// 定点数
	s8 selectedItem = 0;
	u8 value[4];
	
	Init();
	
	while(1){
		if(1){
			const char* code desc = "\x80\x02/\x80\x03选择设定项目 \x80\x00/\x80\x01调整设定值 \x80\x10保存设定并返回设置菜单 \x80\x11不保存设定直接返回设置菜单";
			s16 len = strlen(desc) * (10 >> 1) - 1;
			LCM12864_FillRect(0, 22, 128, 10, 0);
			LCM12864_DrawString(10, desc, offset >> 4, 22, 1);

			LCM12864_FillRect(0, 12, 40, 10, 0);
			LCM12864_DrawString(10, items[selectedItem], 0, 12, 1);
			
			LCM12864_FillRect(89, 12, 39, 10, 0);
			switch(selectedItem){
				case 0:{
					Hex2String(saveData.contrast0, value);
				}break;
				case 1:{
					Hex2String(saveData.contrast1, value);
				}break;
				case 2:{
					static const char* code str[] = {"1/9", "1/7"};
					strcpy(value, str[saveData.bias]);
				}break;
			}
			LCM12864_DrawString(10, value, 111, 12, 1);
			
			offset -= 8;
			if(offset < -(len << 4))offset = 128 << 4;
		}

		LCM12864_Refresh();
		
		Controller_Read();
		// 上下键选择设定项目，左右键调整设定值，A键存档，B键不存档直接退回菜单界面
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
					saveData.contrast0--;
					saveData.contrast0 &= 7;
					break;
				case 1:
					saveData.contrast1--;
					saveData.contrast1 &= 0x3F;
					break;
				case 2:
					saveData.bias--;
					saveData.bias &= 1;
					break;
			}
			LCM12864_SetContrast0(saveData.contrast0);
			LCM12864_SetContrast1(saveData.contrast1);
			LCM12864_SetBias(saveData.bias);
			
		}
		if(Controller_IsKeyDown(WII_CLASSIC_CONTROLLER_BUTTON_RIGHT)){
			switch(selectedItem){
				case 0:
					saveData.contrast0++;
					saveData.contrast0 &= 7;
					break;
				case 1:
					saveData.contrast1++;
					saveData.contrast1 &= 0x3F;
					break;
				case 2:
					saveData.bias++;
					saveData.bias &= 1;
					break;
			}
			LCM12864_SetContrast0(saveData.contrast0);
			LCM12864_SetContrast1(saveData.contrast1);
			LCM12864_SetBias(saveData.bias);
		}
		if(Controller_IsKeyDown(WII_CLASSIC_CONTROLLER_BUTTON_A)){
			SaveData_Flush();
			
			currentModule = Module_ConfigMenu;
			return;
		}
		if(Controller_IsKeyDown(WII_CLASSIC_CONTROLLER_BUTTON_B)){
			SaveData_Read();
			
			LCM12864_SetContrast0(saveData.contrast0);
			LCM12864_SetContrast1(saveData.contrast1);
			LCM12864_SetBias(saveData.bias);
	
			currentModule = Module_ConfigMenu;
			return;
		}
	}
}
