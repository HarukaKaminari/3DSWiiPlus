#ifndef SAVE_DATA_H_
#define SAVE_DATA_H_


#include "Utility.h"


typedef struct{
	s8 signature[8];
	s8 keyMapping[KEY_ITEM_COUNT];
	s8 analogMapping;
	s8 reverseAnalogX;
	s8 reverseAnalogY;
	s8 turboEnableKey;
	s8 turboDisableKey;
	s8 turboFreq;
	s8 analogRatioL;
	s8 analogOffsetL;
	s8 analogRatioR;
	s8 analogOffsetR;
}structSaveData;


extern structSaveData xdata saveData;


void SaveData_Init();
void SaveData_Read();
void SaveData_Flush();
void SaveData_Dump();
void SaveData_Restore();


#endif	/* SAVE_DATA_H_ */
