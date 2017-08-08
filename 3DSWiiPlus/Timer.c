#include "Timer.h"


static vu16 countdown = 0;
static vu16 countdownBackup = 0;
static Func_TimerCallback timerCallback = 0;


static void Timer0_Interrupt() interrupt 1 using 1{
	countdown--;
	if(countdown == 0){
		countdown = countdownBackup;
		if(timerCallback){
			timerCallback();
		}
	}
}


void Timer_Init(){
	AUXR &= 0x7F;		// 定时器时钟12T模式
	TMOD &= 0xF0;		// 定时器为16位自动重装
	TL0 = 0x3C;			// 设置定时初值
	TH0 = 0xF6;			// 设置定时初值
	TF0 = 0;			// 清除TF0标志
	ET0 = 1;			// 开启定时器0中断
	
    EA = 1;				// 开启中断总开关
}

void Timer_SetInterval(u16 ms){
	countdown = ms;
	countdownBackup = ms;
}

void Timer_SetCallback(Func_TimerCallback callback){
	timerCallback = callback;
}

void Timer_Start(){
	TR0 = 1;
}

void Timer_Stop(){
	TR0 = 0;
}
