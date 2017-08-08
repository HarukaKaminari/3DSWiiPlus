#include <STRING.H>
#include "NRF24L01.h"
#include "SPI.h"
#include "Uart.h"


//#define DEBUG


// 定义引脚
sbit NRF24L01_CS	= P3^4;	// 片选（单片机->nRF24L01）
sbit NRF24L01_CE	= P3^5;	// 使能（单片机->nRF24L01）
sbit NRF24L01_IRQ	= P3^6;	// 中断（nRF24L01->单片机）

// 定义指令
#define READ_REG	0x00	// 读配置寄存器。低5位为寄存器地址
#define WRITE_REG	0x20	// 写配置寄存器。低5位为寄存器地址。只有掉电和待机模式可操作
#define RD_RX_PLOAD	0x61	// 读RX有效数据，1~32字节。读完之后FIFO寄存器中的数据被清除。用于接收模式
#define WR_TX_PLOAD	0xA0	// 写TX有效数据，1~32字节。用于发射模式
#define FLUSH_TX	0xE1	// 清除TX FIFO寄存器。发射模式下用
#define FLUSH_RX	0xE2	// 清除RX FIFO寄存器。接收模式下用。传输应答信号的时候不要用它，因为它会使应答信号不能完整传输
#define REUSE_TX_PL	0xE3	// 重新使用上一包数据，CE为高的过程中，数据包被不断地重新发射。在发送过程中必须禁止使用它
#define NOP			0xFF	// 空操作，可以用来读状态寄存器

// 定义寄存器地址
#define CONFIG		0x00	// 配置寄存器
#define EN_AA		0x01	// 使能自动应答功能
#define EN_RXADDR	0x02	// 接收地址允许
#define SETUP_AW	0x03	// 设置地址宽度（所有数据通道）
#define SETUP_RETR	0x04	// 建立自动重发
#define RF_CH		0x05	// 射频通道
#define RF_SETUP	0x06	// 射频寄存器
#define STATUS		0x07	// 状态寄存器
#define OBSERVE_TX	0x08	// 发送检测寄存器
#define CD			0x09	// 载波检测
#define RX_ADDR_P0	0x0A	// 数据通道0接收地址。最大长度:5个字节（先写低字节，所写字节数量由SETUP_AW设定）
#define RX_ADDR_P1	0x0B	// 数据通道1接收地址。最大长度:5个字节（先写低字节，所写字节数量由SETUP_AW设定）
#define RX_ADDR_P2	0x0C	// 数据通道2接收地址。最低字节可设置。高字节部分必须与RX_ADDR_P1[39:8]相等
#define RX_ADDR_P3	0x0D	// 数据通道3接收地址。最低字节可设置。高字节部分必须与RX_ADDR_P1[39:8]相等
#define RX_ADDR_P4	0x0E	// 数据通道4接收地址。最低字节可设置。高字节部分必须与RX_ADDR_P1[39:8]相等
#define RX_ADDR_P5	0x0F	// 数据通道5接收地址。最低字节可设置。高字节部分必须与RX_ADDR_P1[39:8]相等
#define TX_ADDR		0x10	// 发送地址（先写低字节）。在增强型ShockBurst TM模式下RX_ADDR_P0与此地址相等
#define RX_PW_P0	0x11	// 接收数据通道0有效数据宽度（1到32字节）
#define RX_PW_P1	0x12	// 接收数据通道1有效数据宽度（1到32字节）
#define RX_PW_P2	0x13	// 接收数据通道2有效数据宽度（1到32字节）
#define RX_PW_P3	0x14	// 接收数据通道3有效数据宽度（1到32字节）
#define RX_PW_P4	0x15	// 接收数据通道4有效数据宽度（1到32字节）
#define RX_PW_P5	0x16	// 接收数据通道5有效数据宽度（1到32字节）
#define FIFO_STATUS	0x17	// FIFO状态寄存器


// 发送接收数据宽度定义
#define TX_ADDR_WIDTH	5	// 5字节的地址宽度
#define RX_ADDR_WIDTH	5	// 5字节的地址宽度
#define TX_PLOAD_WIDTH	8	// 8字节的用户数据宽度
#define RX_PLOAD_WIDTH	8	// 8字节的用户数据宽度


static u8 code TX_ADDRESS[TX_ADDR_WIDTH] = {0x10, 0x10, 0x10, 0x10, 0x10};  // 定义一个静态发送地址
static u8 sta;


/*
初始化nRF24L01
*/
void nRF24L01_Init(){
	P3M0 &= 0x8F;
	P3M1 &= 0x8F;		// /CS、CE、IRQ设置为准双向
	
	NRF24L01_CE = 0;	// RF24L01进入待机模式
	NRF24L01_CS = 1;	// 禁止SPI
	NRF24L01_IRQ = 1;	// 中断复位
}

/*
从reg寄存器读1字节
*/
u8 nRF24L01_SPI_Read(u8 reg){
	u8 reg_val;
  	NRF24L01_CS = 0;					// CS拉低，SPI开始传输数据
  	SPI_SendAndReceive(reg);			// 选择寄存器
  	reg_val = SPI_SendAndReceive(0);	// 读出该寄存器的值
  	NRF24L01_CS = 1;					// CS拉高，SPI完成数据传输
  	return reg_val;						// 返回数据
}

/*
向reg寄存器写1字节
*/
u8 nRF24L01_SPI_Write(u8 reg, u8 value){
	u8 status;
	NRF24L01_CS = 0;					// CS拉低，SPI开始传输数据
	status = SPI_SendAndReceive(reg);	// 选择寄存器
	SPI_SendAndReceive(value);			// 将数值写入该寄存器
	NRF24L01_CS = 1;					// CS拉高，SPI完成数据传输
	return status;						// 返回nRF24L01的状态
}


/*
从reg寄存器读出size个字节。通常用于读取接收通道数据或接收/发送地址
*/
u8 nRF24L01_SPI_Read_Buf(u8 reg, u8* pBuf, u8 size){
	u8 status, i;
  	NRF24L01_CS = 0;						// CS拉低，SPI开始传输数据
  	status = SPI_SendAndReceive(reg);		// 选择寄存器
  	for(i=0;i<size;++i)
		pBuf[i] = SPI_SendAndReceive(0);	// 读出该寄存器的值
  	NRF24L01_CS = 1;						// CS拉高，SPI完成数据传输
  	return status;							// 返回nRF24L01的状态
}

/*
把pBuf缓存中的数据写入到nRF24L01，通常用来写入发射通道数据或接收/发送地址
*/
u8 nRF24L01_SPI_Write_Buf(u8 reg, u8* pBuf, u8 size){
	u8 status, i;
	NRF24L01_CS = 0;						// CS置低，开始传输数据
	status = SPI_SendAndReceive(reg);		// 选择寄存器，同时返回状态字
	for(i=0;i<size;++i)
		SPI_SendAndReceive(pBuf[i]);		// 逐个字节写入nRF24L01
	NRF24L01_CS = 1;						// CS拉高，结束数据传输
	return status;							// 返回状态寄存器
}

/*
检查是否存在nRF24L01设备
*/
bit nRF24L01_Check(){
	u8 buf[TX_ADDR_WIDTH];
	u8 i = 0;
	nRF24L01_SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADDR_WIDTH);	// 写入5个字节的地址.
	nRF24L01_SPI_Read_Buf(TX_ADDR, buf, TX_ADDR_WIDTH);						// 读出写入的地址
	for(i=0;i<TX_ADDR_WIDTH;++i)
		Uart_Hex(buf[i]);
	Uart_String("\r\n");
	return memcmp(TX_ADDRESS, buf, TX_ADDR_WIDTH) == 0 ? 1 : 0;
}

/*
将nRF24L01设置为发送模式
*/
void nRF24L01_TX_Mode(){
	NRF24L01_CE = 0;															// 进入待机模式
  	nRF24L01_SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADDR_WIDTH);		// 写入发送地址
  	nRF24L01_SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADDR_WIDTH);	// 为了应答接收设备，接收通道0地址和发送地址相同
	nRF24L01_SPI_Write(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);					// 接收通道0选择和发送通道相同有效数据宽度
#ifdef DEBUG
  	nRF24L01_SPI_Write(WRITE_REG + EN_AA, 0x00);								// 禁止接收通道0自动应答
  	nRF24L01_SPI_Write(WRITE_REG + EN_RXADDR, 0x01);							// 使能接收通道0
  	nRF24L01_SPI_Write(WRITE_REG + SETUP_RETR, 0x00);							// 禁止自动重发
#else
  	nRF24L01_SPI_Write(WRITE_REG + EN_AA, 0x01);								// 使能接收通道0自动应答
  	nRF24L01_SPI_Write(WRITE_REG + EN_RXADDR, 0x01);							// 使能接收通道0
  	nRF24L01_SPI_Write(WRITE_REG + SETUP_RETR, 0x0A);							// 自动重发延时等待250us+86us，自动重发10次
#endif
  	nRF24L01_SPI_Write(WRITE_REG + RF_CH, 0x00);								// 选择射频通道为2.4GHz
  	nRF24L01_SPI_Write(WRITE_REG + RF_SETUP, 0x0F);								// 数据传输率2Mbps，发射功率0dBm，低噪声放大器增益
	NRF24L01_CE = 1;															// 拉高CE，进入接收模式
	Delay10us(1);																// 等待10us，转为发送模式
}

/*
将nRF24L01设置为接收模式
*/
void nRF24L01_RX_Mode(){
	NRF24L01_CE = 0;															// 进入待机模式
	nRF24L01_SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADDR_WIDTH);	// 接收设备接收通道0使用和发送设备相同的发送地址
	nRF24L01_SPI_Write(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);					// 接收通道0选择和发送通道相同有效数据宽度
#ifdef DEBUG
	nRF24L01_SPI_Write(WRITE_REG + EN_AA, 0x00);								// 禁用接收通道0自动应答
	nRF24L01_SPI_Write(WRITE_REG + EN_RXADDR, 0x01);							// 使能接收通道0
#else
	nRF24L01_SPI_Write(WRITE_REG + EN_AA, 0x01);								// 使能接收通道0自动应答
	nRF24L01_SPI_Write(WRITE_REG + EN_RXADDR, 0x01);							// 使能接收通道0
#endif
	nRF24L01_SPI_Write(WRITE_REG + RF_CH, 0x00);								// 选择射频通道为2.4GHz
	nRF24L01_SPI_Write(WRITE_REG + RF_SETUP, 0x0F);								// 数据传输率2Mbps，发射功率0dBm，低噪声放大器增益
  	nRF24L01_SPI_Write(WRITE_REG + CONFIG, 0x0F);								// 开启中断，CRC使能，16位CRC校验，上电，接收模式
	NRF24L01_CE = 1;															// 拉高CE，进入接收模式
}

/*
发送数据包（CE=1持续至少10us），130us后启动发射，返回值：0代表成功，非0为失败。其中1代表达到最大重发次数，其他值需要查手册
*/
u8 nRF24L01_Send(u8* pBuf){
	u8 ret;
	NRF24L01_CE = 0;															// 进入待机模式
	nRF24L01_SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADDR_WIDTH);		// 写入发送地址
	nRF24L01_SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADDR_WIDTH);	// 为了应答接收设备，接收通道0地址和发送地址相同
	nRF24L01_SPI_Write_Buf(WR_TX_PLOAD, pBuf, TX_PLOAD_WIDTH);					// 写数据包到TX FIFO
  	nRF24L01_SPI_Write(WRITE_REG + CONFIG, 0x0E);								// 开启中断，CRC使能，16位CRC校验，上电，发送模式
	NRF24L01_CE = 1;															// 拉高CE启动接收设备
	while(NRF24L01_IRQ);														// 等待发生中断
	sta = nRF24L01_SPI_Read(STATUS);											// 读状态寄存器
	if(sta & (1 << 4)){															// 如果达到最大重发次数
		nRF24L01_SPI_Write(FLUSH_TX, 0);										// 清除TX缓冲区
		ret = 1;
	}
	if(sta & (1 << 5)){																// 如果数据成功发送
		ret = 0;
	}
	nRF24L01_SPI_Write(WRITE_REG + STATUS, sta);								// 清状态寄存器
	return ret;
}

/*
接收数据包，返回值：0代表成功，非0为失败
*/
u8 nRF24L01_Recv(u8* pBuf){
	u8 ret = 1;
	sta = nRF24L01_SPI_Read(STATUS);								// 读状态寄存器
	if(sta & (1 << 6)){
		//NRF24L01_CE = 0;
		nRF24L01_SPI_Read_Buf(RD_RX_PLOAD, pBuf, TX_PLOAD_WIDTH);	// 从RX FIFO读出数据
		ret = 0;
	}
	nRF24L01_SPI_Write(FLUSH_RX, 0);
	nRF24L01_SPI_Write(WRITE_REG + STATUS, sta);					// 清除RX_DS中断标志
	return ret;
}

/*
读取寄存器值。调试专用
*/
u8 nRF24L01_TestReg(u8 reg){
	return nRF24L01_SPI_Read(reg);
}
