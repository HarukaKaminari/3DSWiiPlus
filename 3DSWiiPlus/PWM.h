#ifndef PWM_H_
#define PWM_H_


#include "Utility.h"


void PWM_Init();
void PWM_Start();
void PWM_Stop();
void PWM_SetChannelStatus(u8 channel);
void PWM_SetDuty(u8 channel, u8 duty);


#endif	/* PWM_H_ */
