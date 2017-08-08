#ifndef UART_H_
#define UART_H_


#include "Utility.h"


void Uart_Init();
void Uart_Send(u8 value);
void Uart_SendBuf(void* p, u8 len);
void Uart_Recv(u8* value);
void Uart_RecvBuf(void* p, u8* len);
void Uart_String(const char* str);
void Uart_Dec(u8 val);
void Uart_Hex(u8 val);


#endif	/* UART_H_ */
