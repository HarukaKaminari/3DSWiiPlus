#include <STRING.H>
#include "Uart.h"


bit busy = 0;	// 发送忙标志
static xdata u8 sendBuf[0x100];	// 发送缓冲
static xdata u8 recvBuf[0x100];	// 接收缓冲
static u8 sendP = 0;
static u8 sendBufP = 0;
static u8 recvP = 0;
static u8 recvBufP = 0;


static void Uart() interrupt 4 using 1{
	if(RI){
		RI = 0;		// 清除RI位
		//P0 = SBUF;	// P0显示串口数据
		//P22 = RB8;	// P2.2显示校验位
		recvBuf[recvBufP++] = SBUF;	// 从UART数据接收寄存器中读数据，写入缓冲
	}
	if(TI){
		TI = 0;		// 清除TI位
		busy = 0;	// 清忙标志
	}
}

static void Uart_Flush(){
	if(sendP != sendBufP){
		while(busy);	// 等待前面的数据发送完成
		busy = 1;
		SBUF = sendBuf[sendP++];	// 写数据到UART数据发送寄存器
	}
}

// 主频30M
void Uart_Init(){
	P_SW1 &= 0x3F;	// 串口1的TX/RX位于P3.1/P3.0
	PCON &= 0x3F;	// 波特率不倍速，SCON.6不作为帧错误检测功能
	SCON = 0x50;	// 8位数据，可变波特率，无奇偶校验
	AUXR |= 0x40;	// 定时器1时钟为Fosc，即1T
	AUXR &= 0xFE;	// 串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;	// 清除定时器1模式位
	TMOD |= 0x20;	// 设定定时器1为8位自动重装方式
	TL1 = 0xF8;		// 设定定时初值（波特率115200）
	TH1 = 0xF8;		// 设定定时器重装值
	ET1 = 0;		// 禁止定时器1中断
	TR1 = 1;		// 启动定时器1
	ES = 1;			// 使能串口中断
    EA = 1;			// 全局中断打开
}

void Uart_Send(u8 value){
	sendBuf[sendBufP++] = value;
	Uart_Flush();
}

void Uart_SendBuf(void* p, u8 len){
	u8 remains = 0x100 - sendBufP;
	if(remains >= len)
		memcpy(sendBuf + sendBufP, p, len);
	else{
		memcpy(sendBuf + sendBufP, p, remains);
		memcpy(sendBuf, (u8*)p + remains, len - remains);
	}
	sendBufP += len;
	Uart_Flush();
}

void Uart_Recv(u8* value){
	if(recvP != recvBufP)
		*value = recvBuf[recvBufP++];
	else
		*value = 0;
}

void Uart_RecvBuf(void* p, u8* len){
	if(recvP < recvBufP){
		*len = recvBufP - recvP;
		memcpy(p, recvBuf + recvP, recvBufP - recvP);
		recvP = recvBufP;
	}else if(recvP > recvBufP){
		u8 remains = 0x100 - recvP;
		memcpy(p, recvBuf + recvP, remains);
		memcpy((u8*)p + remains, recvBuf, recvBufP - remains);
		*len = 0x100 + recvBufP - recvP;
	}else{
		*len = 0;
	}
}

void Uart_String(const char* str){
	Uart_SendBuf(str, strlen(str));
}

void Uart_Dec(u8 val){
	u8 i = 0;
	u8 strBuf[4];
	Dec2String(val, strBuf);
	Uart_String(strBuf);
}

void Uart_Hex(u8 val){
	u8 i = 0;
	u8 strBuf[3];
	Hex2String(val, strBuf);
	Uart_String(strBuf);
}
