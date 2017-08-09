# 3DSWiiPlus
这是一个3DS硬件扩展，实现了3DS外接手柄的功能。

原理：
通过单片机对Wii Classic Controller解码，获得外接手柄的所有按键和摇杆的状态，然后发送到3DS端直接重写按键摇杆状态。