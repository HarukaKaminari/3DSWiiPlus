namespace IST3932Simulator
{
	partial class formMain
	{
		/// <summary>
		/// 必需的设计器变量。
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// 清理所有正在使用的资源。
		/// </summary>
		/// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows 窗体设计器生成的代码

		/// <summary>
		/// 设计器支持所需的方法 - 不要修改
		/// 使用代码编辑器修改此方法的内容。
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			this.picPreview = new System.Windows.Forms.PictureBox();
			this.timer = new System.Windows.Forms.Timer(this.components);
			this.sldZoom = new System.Windows.Forms.TrackBar();
			((System.ComponentModel.ISupportInitialize)(this.picPreview)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.sldZoom)).BeginInit();
			this.SuspendLayout();
			// 
			// picPreview
			// 
			this.picPreview.Location = new System.Drawing.Point(12, 12);
			this.picPreview.Name = "picPreview";
			this.picPreview.Size = new System.Drawing.Size(512, 256);
			this.picPreview.TabIndex = 0;
			this.picPreview.TabStop = false;
			this.picPreview.Paint += new System.Windows.Forms.PaintEventHandler(this.picPreview_Paint);
			// 
			// timer
			// 
			this.timer.Interval = 1;
			this.timer.Tick += new System.EventHandler(this.timer1_Tick);
			// 
			// sldZoom
			// 
			this.sldZoom.LargeChange = 1;
			this.sldZoom.Location = new System.Drawing.Point(12, 274);
			this.sldZoom.Maximum = 4;
			this.sldZoom.Minimum = 1;
			this.sldZoom.Name = "sldZoom";
			this.sldZoom.Size = new System.Drawing.Size(512, 45);
			this.sldZoom.TabIndex = 1;
			this.sldZoom.TickStyle = System.Windows.Forms.TickStyle.Both;
			this.sldZoom.Value = 1;
			this.sldZoom.Scroll += new System.EventHandler(this.sldZoom_Scroll);
			// 
			// formMain
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(538, 512);
			this.Controls.Add(this.sldZoom);
			this.Controls.Add(this.picPreview);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "formMain";
			this.Text = "IST3932 Simulator";
			this.Load += new System.EventHandler(this.formMain_Load);
			this.Paint += new System.Windows.Forms.PaintEventHandler(this.formMain_Paint);
			((System.ComponentModel.ISupportInitialize)(this.picPreview)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.sldZoom)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.PictureBox picPreview;
		private System.Windows.Forms.Timer timer;
		private System.Windows.Forms.TrackBar sldZoom;
	}
}

