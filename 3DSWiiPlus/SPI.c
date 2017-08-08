#include "SPI.h"


#define USE_HARDWARE_SPI


// 每个SPI设备的/CS要在各自的.c中单独定义
#ifndef USE_HARDWARE_SPI
sbit SPI_MOSI	= P1^3;
sbit SPI_MISO	= P1^4;
sbit SPI_CLK	= P1^5;
#endif


void SPI_Init(){
#ifdef USE_HARDWARE_SPI
	P_SW1 &= 0xF3;	// 将硬件SPI设置到第一组管脚（P1.2、P1.3、P1.4、P1.5）
#endif
	P1M0 &= 0xC7;
	P1M1 &= 0xC7;	// 将SPI_CLK、SPI_MOSI和SPI_MISO设置成准双向
#ifdef USE_HARDWARE_SPI
	SPDAT = 0;		// 初始化SPI缓冲区
	SPSTAT = 0xC0;	// 初始化SPI状态
	SPCTL = 0xDC;	// 开启硬件SPI，主机模式，高位先发，下降沿发送上升沿接收，CLK频率为1/4主频
#endif
}

u8 SPI_SendAndReceive(u8 value){
#ifdef USE_HARDWARE_SPI
	SPDAT = value;				// 填充缓冲区
	while(!(SPSTAT & 0x80));	// 等待传输完成
	SPSTAT = 0xC0;				// 清标志位
	return SPDAT;				// 返回从机发送过来的数据
#else
	u8 i;
	for(i=0;i<8;++i){
		SPI_CLK = 0;
		SPI_MOSI = value & 0x80;
		SPI_CLK = 1;
		value <<= 1;
		value |= SPI_MISO;
	}
	return value;
#endif
}
