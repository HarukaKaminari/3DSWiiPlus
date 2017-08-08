#include "Timer.h"


/*
30M主频的定时器实在太快了，因此使用两个硬件定时器T0和T2配合进行定时。
其中，定时器T2的定时时间恒定为25ms，定时器T0的定时时间为总定时时间 % 25ms。
如果总定时时间小于25ms，则只启动定时器T0，T0中断到来之后触发回调函数。
如果总定时时间大于等于25ms，则先启动定时器T2，通过T2中断累计时间，直到剩余定时时间不足25ms的时候，启动T0，把剩下的时间计时掉，在T0中断触发回调函数。
由于countdown和重装countdown均为8bit，因此最大定时时间为256*25ms=6400ms。
*/


static vu8 countdown = 0;
static vu8 countdownBackup = 0;
static Func_TimerCallback timerCallback = 0;


static void Timer0_Interrupt() interrupt 1 using 1{
	if(timerCallback){
		timerCallback();
		Timer_Start();
	}
}

static void Timer2_Interrupt() interrupt 12 using 1{
	countdown--;
	// 如果countdown减光，则重装countdown，关闭定时器2，启动定时器0
	if(countdown == 0){
		countdown = countdownBackup;
		AUXR &= 0xEF;
		TR0 = 1;
	}
}


void Timer_Init(){
	AUXR &= 0x7F;		// 定时器时钟12T模式
	TMOD &= 0xF0;		// 定时器为16位自动重装
	TF0 = 0;			// 清除TF0标志
	ET0 = 1;			// 开启定时器0中断
	
	AUXR &= 0xF3;		// T2为定时器，12T模式
	INT_CLKO &= 0xFB;	// 不允许T2对外输出时钟
	IE2 |= 0x04;		// 开启定时器2中断
	
    EA = 1;				// 开启中断总开关
}

// 12T，30MHz的主频下，16位定时器0最大定时时间为26.214ms。所以超过25ms的定时时间，都需要countdown参与
void Timer_SetInterval(u16 ms){
	// 计算定时器0的初值和countdown数
	u8 remain = ms % 25;
	// 如果正好是25ms的整数倍，则令T0只计1个时钟就溢出触发中断。这种做法略微有点误差，但总算还能用
	if(remain == 0){
		TH0 = 0xFF;
		TL0 = 0xFF;
	}else{
		u16 value = 65536 - remain * 2500;
		TH0 = value >> 8;
		TL0 = value & 0xFF;
	}
	countdown = ms / 25;
	countdownBackup = countdown;
	// 定时器2的定时时间恒定为25ms
	T2L = 0xDC;
	T2H = 0x0B;
}

void Timer_SetCallback(Func_TimerCallback callback){
	timerCallback = callback;
}

void Timer_Start(){
	// 如果countdown有参与定时，则启动定时器2；否则直接启动定时器0
	if(countdown > 0){
		TR0 = 0;
		AUXR |= 0x10;
	}else{
		TR0 = 1;
		AUXR &= 0xEF;
	}
}

void Timer_Stop(){
	TR0 = 0;
	AUXR &= 0xEF;
}
