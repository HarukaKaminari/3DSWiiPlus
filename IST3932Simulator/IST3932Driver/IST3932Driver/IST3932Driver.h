#pragma once


extern "C" __declspec(dllexport) void LCM12864_Init();
extern "C" __declspec(dllexport) void LCM12864_Clear();
extern "C" __declspec(dllexport) bool LCM12864_Refresh(int x, int y);
extern "C" __declspec(dllexport) void LCM12864_DrawChar(u8 fontSize, u16 idx, s8 x, s8 y, bit c);
extern "C" __declspec(dllexport) void LCM12864_DrawString(u8 fontSize, const char* str, s16 x, s16 y, bit c);
extern "C" __declspec(dllexport) void LCM12864_DrawStringIdx(u8 fontSize, u8 idx, s16 x, s16 y, bit c);
extern "C" __declspec(dllexport) void LCM12864_DrawPixel(u8 x, u8 y, bit c);
extern "C" __declspec(dllexport) void LCM12864_DrawLine(u8 x0, u8 y0, u8 x1, u8 y1, bit c);
extern "C" __declspec(dllexport) void LCM12864_FillRect(u8 x, u8 y, u8 w, u8 h, bit c);
extern "C" __declspec(dllexport) void LCM12864_DrawCircle(u8 x1, u8 y1, u8 r, bit c);
extern "C" __declspec(dllexport) u16 LCM12864_GetStrLenFromIdx(u8 fontSize, u8 idx);
