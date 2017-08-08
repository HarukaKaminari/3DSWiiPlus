#include <INTRINS.H>
#include "Utility.h"


// 下面两个延时都是基于30MHz的主频，STC-Y5指令集
void Delay10us(u8 us){
	while(us--){
		u8 i = 72;
		_nop_();
		_nop_();
		while(--i);
	}
}

void Delay(u16 ms){
	while(ms--){
		u8 i = 30, j = 43;
		do{
			while(--j);
		}while(--i);
	}
}

void SoftReset(){
	IAP_CONTR = 0x20;
}

u8 Dec2String(u8 dec, u8* strBuf){
	u8 _2 = dec / 100;
	u8 _1 = (dec - _2 * 100) / 10;
	u8 _0 = dec - _2 * 100 - _1 * 10;
	if(_2){
		strBuf[0] = _2 + '0';
		strBuf[1] = _1 + '0';
		strBuf[2] = _0 + '0';
		strBuf[3] = 0;
		return 3;
	}else if(_1){
		strBuf[0] = _1 + '0';
		strBuf[1] = _0 + '0';
		strBuf[2] = 0;
		return 2;
	}else{
		strBuf[0] = _0 + '0';
		strBuf[1] = 0;
		return 1;
	}
}

u8 Hex2String(u8 hex, u8* strBuf){
	u8 hi = (hex >> 4) & 0xF;
	u8 lo = hex & 0xF;
	strBuf[0] = ((hi>9)?(hi-10+'A'):(hi+'0'));
	strBuf[1] = ((lo>9)?(lo-10+'A'):(lo+'0'));
	strBuf[2] = 0;
	return 2;
}
