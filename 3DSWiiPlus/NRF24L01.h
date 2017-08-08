#ifndef NRF24L01_H_
#define NRF24L01_H_


#include "Utility.h"


void nRF24L01_Init();
u8 nRF24L01_SPI_Read(u8 reg);
u8 nRF24L01_SPI_Write(u8 reg, u8 value);
u8 nRF24L01_SPI_Read_Buf(u8 reg, u8* pBuf, u8 size);
u8 nRF24L01_SPI_Write_Buf(u8 reg, u8* pBuf, u8 size);
bit nRF24L01_Check();
void nRF24L01_TX_Mode();
void nRF24L01_RX_Mode();
u8 nRF24L01_Send(u8* pBuf);
u8 nRF24L01_Recv(u8* pBuf);
u8 nRF24L01_TestReg(u8 reg);


#endif   /* NRF24L01_H_ */
