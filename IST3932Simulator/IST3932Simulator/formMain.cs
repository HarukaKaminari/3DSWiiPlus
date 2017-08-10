using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;


namespace IST3932Simulator
{
	public partial class formMain : Form
	{
		[DllImport("IST3932Driver.dll")]
		static extern void LCM12864_Init();
		[DllImport("IST3932Driver.dll")]
		static extern void LCM12864_Clear();
		[DllImport("IST3932Driver.dll", CallingConvention = CallingConvention.Cdecl)]
		static extern bool LCM12864_Refresh(int x, int y);
		[DllImport("IST3932Driver.dll", CallingConvention = CallingConvention.Cdecl)]
		static extern void LCM12864_DrawChar(byte fontSize, ushort idx, sbyte x, sbyte y, byte c);
		[DllImport("IST3932Driver.dll", CallingConvention = CallingConvention.Cdecl)]
		static extern void LCM12864_DrawString(byte fontSize, string str, short x, short y, byte c);
		[DllImport("IST3932Driver.dll", CallingConvention = CallingConvention.Cdecl)]
		static extern void LCM12864_DrawStringIdx(byte fontSize, byte idx, short x, short y, byte c);
		[DllImport("IST3932Driver.dll", CallingConvention = CallingConvention.Cdecl)]
		static extern void LCM12864_DrawPixel(byte x, byte y, byte c);
		[DllImport("IST3932Driver.dll", CallingConvention = CallingConvention.Cdecl)]
		static extern void LCM12864_DrawLine(byte x0, byte y0, byte x1, byte y1, byte c);
		[DllImport("IST3932Driver.dll", CallingConvention = CallingConvention.Cdecl)]
		static extern void LCM12864_FillRect(byte x, byte y, byte w, byte h, byte c);
		[DllImport("IST3932Driver.dll", CallingConvention = CallingConvention.Cdecl)]
		static extern void LCM12864_DrawCircle(byte x1, byte y1, byte r, byte c);
		[DllImport("IST3932Driver.dll", CallingConvention = CallingConvention.Cdecl)]
		static extern ushort LCM12864_GetStrLenFromIdx(byte fontSize, byte idx);

		public formMain()
		{
			InitializeComponent();
		}

		private void formMain_Load(object sender, EventArgs e)
		{
			LCM12864_Init();
			
			timer.Interval = 10;
			timer.Enabled = true;

			Start();
		}

		private void formMain_Paint(object sender, PaintEventArgs e)
		{
			picPreview.Refresh();
		}

		private void picPreview_Paint(object sender, PaintEventArgs e)
		{
			int zoom = sldZoom.Value;
			Graphics g = e.Graphics;
			g.Clear(Color.White);
			for (int y = 0; y < 64; ++y)
			{
				for (int x = 0; x < 128; ++x)
				{
					if (LCM12864_Refresh(x, y))
						g.FillRectangle(new SolidBrush(Color.Black), new Rectangle(x * zoom, y * zoom, zoom, zoom));
				}
			}
		}

		private sbyte selectedIndex = 0;
		private short offset = 128 << 4;  // 定点数
		private void timer1_Tick(object sender, EventArgs e)
		{
			Update();
			
			picPreview.Refresh();
		}

		private void sldZoom_Scroll(object sender, EventArgs e)
		{
			picPreview.Refresh();
		}



		private void Start()
		{
			byte len = (byte)(LCM12864_GetStrLenFromIdx(10, (byte)enumStrings.TITLE_SETUP) - 1);
			LCM12864_FillRect((byte)(((128 - len) >> 1) - 1), 0, (byte)(len + 2), 11, 1);
			LCM12864_FillRect(0, 12, 128, 1, 1);
			LCM12864_FillRect(0, 52, 128, 1, 1);
			LCM12864_DrawStringIdx(10, (byte)enumStrings.TITLE_SETUP, (short)((128 - len) >> 1), 1, 0);    // 设置菜单
		}
		private new void Update()
		{
			if (true)
			{
				short len;
				len = (short)(LCM12864_GetStrLenFromIdx(10, (byte)(enumStrings.MENU_KEY_MAPPING + selectedIndex)) - 1);
				LCM12864_FillRect(0, 27, 128, 10, 0);
				LCM12864_DrawStringIdx(10, (byte)(enumStrings.MENU_KEY_MAPPING + selectedIndex), (short)((128 - len) >> 1), 27, 1);  // 按键映射	摇杆映射	连发设定	摇杆校正	设备调整

				len = (short)(LCM12864_GetStrLenFromIdx(10, (byte)enumStrings.DESC_1) - 1);
				LCM12864_FillRect(0, 54, 128, 10, 0);
				LCM12864_DrawStringIdx(10, (byte)enumStrings.DESC_1, (short)(offset >> 4), 54, 1); // 上/下选择菜单项 A确定 B退出设置菜单
				
				offset -= 8;
				if (offset < -(len << 4)) offset = 128 << 4;
			}
		}
	}



	enum enumStrings
	{
		JOYSTICK_NOT_FOUND,
		RESTART,
		ENTER_SETUP_WITHIN_3_SECS,
		TITLE_SETUP,
		MENU_KEY_MAPPING,
		MENU_ANALOG_MAPPING,
		MENU_ANALOG_CALIBRATION,
		MENU_TURBO_SETTING,
		MENU_DEVICE_SETTING,
		DESC_1,
		TITLE_KEY_MAPPING,
		KEY_WII,
		KEY_CTR,
		KEY_WII_LEFT,
		KEY_WII_RIGHT,
		KEY_WII_UP,
		KEY_WII_DOWN,
		KEY_WII_A,
		KEY_WII_B,
		KEY_WII_X,
		KEY_WII_Y,
		KEY_WII_L,
		KEY_WII_R,
		KEY_WII_ZL,
		KEY_WII_ZR,
		KEY_WII_PLUS,
		KEY_WII_MINUS,
		KEY_WII_HOME,
		KEY_CTR_LEFT,
		KEY_CTR_RIGHT,
		KEY_CTR_UP,
		KEY_CTR_DOWN,
		KEY_CTR_A,
		KEY_CTR_B,
		KEY_CTR_X,
		KEY_CTR_Y,
		KEY_CTR_L,
		KEY_CTR_R,
		KEY_CTR_ZL,
		KEY_CTR_ZR,
		KEY_CTR_START,
		KEY_CTR_SELECT,
		KEY_CTR_HOME,
		KEY_CTR_NONE,
		DESC_2,
		TITLE_ANALOG_MAPPING,
		ANALOG_SWAP,
		ANALOG_NEGATIVE_X,
		ANALOG_NEGATIVE_Y,
		ANALOG_NO,
		ANALOG_YES,
		DESC_3,
		TITLE_ANALOG_CALIBRATION,
		CALIBRATION_THRESTHOLD,
		CALIBRATION_OFFSET,
		DESC_4,
		TITLE_TURBO_SETTING,
		TURBO_KEY_ENABLE,
		TURBO_KEY_DISABLE,
		TURBO_SPEED,
		TURBO_WII,
		TURBO_WII_NONE,
		TURBO_WII_LEFT,
		TURBO_WII_RIGHT,
		TURBO_WII_UP,
		TURBO_WII_DOWN,
		TURBO_WII_A,
		TURBO_WII_B,
		TURBO_WII_X,
		TURBO_WII_Y,
		TURBO_WII_L,
		TURBO_WII_R,
		TURBO_WII_ZL,
		TURBO_WII_ZR,
		TURBO_WII_PLUS,
		TURBO_WII_MINUS,
		TURBO_WII_HOME,
		TURBO_10,
		TURBO_15,
		TURBO_20,
		TURBO_25,
		TURBO_30,
		DESC_5,
		TITLE_DEVICE_SETTING,
		DEVICE_BRIGHTNESS,
		DEVICE_CONTRAST,
		DEVICE_BIAS,
		DEVICE_1_9,
		DEVICE_1_7,
		DESC_6,
	};
}
