/*
这套设备需要的IO口如下：
1个硬件Uart，用于串口下载。
15个GPIO，用于映射到3DS的13个按键以及右摇杆的ZL/ZR键。
2个GPIO，用于模拟I2C。
4个硬件PWM，用于四组DAC。
1个硬件SPI，用于控制LCD和nrf24L01+。
3个GPIO，用于LCD的/CS、CD和LED。
2个GPIO，用于nRF24L01+的/CS和CE。

详细表如下：
	名称	IO口	模式	管脚
硬件Uart：
	RX		P3.0	准双向	18
	TX		P3.1	准双向	19
CTRInterface：
	LEFT	P0.0	开漏	40
	RIGHT	P0.1	开漏	41
	UP		P0.2	开漏	42
	DOWN	P0.3	开漏	43
	A		P0.4	开漏	44
	B		P0.5	开漏	1
	X		P0.6	开漏	2
	Y		P0.7	开漏	3
	L		P4.0	开漏	17
	R		P4.1	开漏	26
	ZL		P4.2	开漏	27
	ZR		P4.3	开漏	28
	STA		P4.4	开漏	29
	SEL		P4.5	开漏	38
	HOME	P4.6	开漏	39
I2C：
	SDA		P3.2	开漏	20
	SCL		P3.3	开漏	21
PWM：
	PWM0	P3.7	强推挽	25
	PWM1	P2.1	强推挽	31
	PWM2	P2.2	强推挽	32
	PWM3	P2.3	强推挽	33
硬件SPI：
	MOSI	P1.3	准双向	8
	MISO	P1.4	准双向	9
	SCLK	P1.5	准双向	10
LCD：
	/CS		P1.0	准双向	4
	CD		P1.1	准双向	5
	LED		P1.2	准双向	7
nRF24L01+：
	/CS		P3.4	准双向	22
	CE		P3.5	准双向	23
	IRQ		P3.6	准双向	24
*/


#include <INTRINS.h>
#include "main.h"
#include "Uart.h"
#include "I2C.h"
#include "IAP.h"
#include "PWM.h"
#include "Timer.h"
#include "SPI.h"
#include "CTRInterface.h"
#include "LCM12864.h"
#include "SaveData.h"
#include "Controller.h"
#include "ModuleConnect.h"
#include "ModuleCountdown.h"
#include "ModuleGeneric.h"
#include "ModuleConfigMenu.h"
#include "ModuleKeyMapping.h"
#include "ModuleAnalogMapping.h"
#include "ModuleAnalogCalibration.h"
#include "ModuleTurboSetting.h"
#include "ModuleDeviceSetting.h"


enumModuleType currentModule = Module_Connect;


static void Init(){
	P0M0 = 0;P0M1 = 0;
	P1M0 = 0;P1M1 = 0;
	P2M0 = 0;P2M1 = 0;
	P3M0 = 0;P3M1 = 0;
	P4M0 = 0;P4M1 = 0;	// 设置所有P口均为弱上拉
	P0 = 0xFF;
	P1 = 0xFF;
	P2 = 0xFF;
	P3 = 0xFF;
	P4 = 0xFF;			// 所有P口均输出高电平
}

void main(){
	Init();
	Uart_Init();
	I2C_Init();
	PWM_Init();
	Timer_Init();
	SPI_Init();
	
	CTRInterface_Init();
	LCM12864_Init();
	LCM12864_Backlight(0);
	SaveData_Init();
	Controller_Init();
	
	while(1){
		LCM12864_Clear();
		
		switch(currentModule){
			case Module_Connect:			ModuleConnect();			break;
			case Module_Countdown:			ModuleCountdown();			break;
			case Module_Generic:			ModuleGeneric();			break;
			case Module_ConfigMenu:			ModuleConfigMenu();			break;
			case Module_KeyMapping:			ModuleKeyMapping();			break;
			case Module_AnalogMapping:		ModuleAnalogMapping();		break;
			case Module_AnalogCalibration:	ModuleAnalogCalibration();	break;
			case Module_TurboSetting:		ModuleTurboSetting();		break;
			case Module_DeviceSetting:		ModuleDeviceSetting();		break;
		}
	}
}
