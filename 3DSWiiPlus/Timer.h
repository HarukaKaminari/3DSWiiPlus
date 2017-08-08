#ifndef TIMER_H_
#define TIMER_H_


#include "Utility.h"


typedef void (*Func_TimerCallback)(void);


void Timer_Init();
void Timer_SetInterval(u16 ms);
void Timer_SetCallback(Func_TimerCallback callback);
void Timer_Start();
void Timer_Stop();


#endif	/* TIMER_H_ */
