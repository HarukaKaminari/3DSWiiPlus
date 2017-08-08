#include <INTRINS.h>
#include "IAP.h"


/*
每个扇区为512字节
*/


/* 定义IAP指令 */
#define CMD_IDLE	0	// 等待
#define CMD_READ	1	// 字节读取
#define CMD_PROGRAM	2	// 字节编程
#define CMD_ERASE	3	// 扇区擦除

/* 定义等待时间 */
#define WAIT_TIME	0x00	// 系统时钟>=30MHz
//#define WAIT_TIME	0x01	// 系统时钟>=24MHz
//#define WAIT_TIME	0x02	// 系统时钟>=20MHz
//#define WAIT_TIME	0x03	// 系统时钟>=12MHz
//#define WAIT_TIME	0x04	// 系统时钟>=6MHz
//#define WAIT_TIME	0x05	// 系统时钟>=3MHz
//#define WAIT_TIME	0x06	// 系统时钟>=2MHz
//#define WAIT_TIME	0x07	// 系统时钟>=1MHz


/*
关闭IAP
*/
void IAP_Idle(){
    IAP_CONTR = 0;		// 关闭IAP功能
    IAP_CMD = 0;		// 清除命令寄存器
    IAP_TRIG = 0;		// 清除触发寄存器
    IAP_ADDRH = 0x80;	// 将地址设置到非IAP区域
    IAP_ADDRL = 0;
}

/*
从IAP区域指定地址读1字节
*/
u8 IAP_ReadByte(u16 addr){
    u8 dat;							// 数据缓冲区
    IAP_CONTR = WAIT_TIME | 0x80;	// 打开IAP功能，并且设置等待时间
    IAP_CMD = CMD_READ;				// 设置IAP的指令为读
    IAP_ADDRL = addr;				// 设置地址
    IAP_ADDRH = addr >> 8;
    IAP_TRIG = 0x5A;				// 触发指令1（0x5A）
    IAP_TRIG = 0xA5;				// 触发指令2（0xA5）
    _nop_();						// 等待操作完成
    dat = IAP_DATA;					// 读IAP数据
    IAP_Idle();						// 关闭IAP
    return dat;
}

/*
写1字节数据到IAP指定地址
*/
void IAP_ProgramByte(u16 addr, u8 dat){
    IAP_CONTR = WAIT_TIME | 0x80;	// 打开IAP功能，并且设置等待时间
    IAP_CMD = CMD_PROGRAM;			// 设置IAP的指令为编程
    IAP_ADDRL = addr;				// 设置地址
    IAP_ADDRH = addr >> 8;
    IAP_DATA = dat;					// 写IAP数据
    IAP_TRIG = 0x5A;				// 触发指令1（0x5A）
    IAP_TRIG = 0xA5;				// 触发指令2（0xA5）
    _nop_();						// 等待操作完成
    IAP_Idle();						// 关闭IAP
}

/*
擦除指定扇区
*/
void IAP_EraseSector(u16 addr){
    IAP_CONTR = WAIT_TIME | 0x80;	// 打开IAP功能，并且设置等待时间
    IAP_CMD = CMD_ERASE;			// 设置IAP的指令为擦除
    IAP_ADDRL = addr;				// 设置地址
    IAP_ADDRH = addr >> 8;
    IAP_TRIG = 0x5A;				// 触发指令1（0x5A）
    IAP_TRIG = 0xA5;				// 触发指令2（0xA5）
    _nop_();						// 等待操作完成
    IAP_Idle();						// 关闭IAP
}
