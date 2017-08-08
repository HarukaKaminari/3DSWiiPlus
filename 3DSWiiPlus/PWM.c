#include "PWM.h"


// 预定义寄存器地址
static const u16 code addr_PWM_T1[] = {0xff00, 0xff10, 0xff20, 0xff30, 0xff40, 0xff50};
static const u16 code addr_PWM_T2[] = {0xff02, 0xff12, 0xff22, 0xff32, 0xff42, 0xff52};
static const u16 code addr_PWM_CR[] = {0xff04, 0xff14, 0xff24, 0xff34, 0xff44, 0xff54};


void PWM_Init(){
	P3M0 |= 0x80;
	P3M1 &= 0x7F;	// P3.7设置为强推挽
	P2M0 |= 0x0E;
	P2M1 &= 0xF1;	// P2.1、P2.2、P2.3设置为强推挽
	P_SW2 |= 0x80;	// 使能访问XSFR（PWM的寄存器位于XSFR区域）
    PWMCFG = 0x00;	// 配置PWM的输出初始电平为低电平
	PWMCR = 0x00;	// 关闭PWM计数器归零中断
	PWMCKS = 0x00;	// 选择PWM的时钟为系统时钟/(0+1)
	PWMC = 0x100;	// 设置PWM周期为256
	P_SW2 &= ~0x80;	// 禁止访问XSFR
}

void PWM_Start(){
	P_SW2 |= 0x80;	// 使能访问XSFR（PWM的寄存器位于XSFR区域）
	PWMCR |= 0x80;	// 启动PWM
	P_SW2 &= ~0x80;	// 禁止访问XSFR
}

void PWM_Stop(){
	P_SW2 |= 0x80;	// 使能访问XSFR（PWM的寄存器位于XSFR区域）
	PWMCR &= ~0x80;	// 停止PWM
	P_SW2 &= ~0x80;	// 禁止访问XSFR
}

void PWM_SetChannelStatus(u8 channel){
	P_SW2 |= 0x80;				// 使能访问XSFR（PWM的寄存器位于XSFR区域）
	channel &= 0x30;			// 保证channel在6路范围之内
	channel |= (PWMCR & 0xC0);	// 创建PWMCR的值
	PWMCR = channel;			// 赋值给PWMCR
	P_SW2 &= ~0x80;				// 禁止访问XSFR
}

void PWM_SetDuty(u8 channel, u8 duty){
	P_SW2 |= 0x80;									// 使能访问XSFR（PWM的寄存器位于XSFR区域）
	*((vu16 xdata*)addr_PWM_T1[channel]) = 0;
	*((vu16 xdata*)addr_PWM_T2[channel]) = duty;
	*((vu16 xdata*)addr_PWM_CR[channel]) = 0x00;	// 选择PWM管脚为默认，关闭PWM中断
	P_SW2 &= ~0x80;									// 禁止访问XSFR
}
