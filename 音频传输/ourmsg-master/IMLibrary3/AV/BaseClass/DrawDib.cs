using System;
using System.Text;
using System.Drawing;
using System.Diagnostics;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace IMLibrary.AV
{
    /// <summary>
    /// DrawDib ��ժҪ˵����
    /// </summary>
    public class DrawDib
    {
        /// <summary>
        /// ��ʼ����Ƶ��ˢ
        /// </summary>
        public DrawDib()
        {
        }

        /// <summary>
        /// ��ʼ����ˢ�ؼ�
        /// </summary>
        /// <param name="Control">���ƿؼ�</param>
        public DrawDib(Control Control)
        {
            this.Control = Control;
        }

        #region ����

        /// <summary>
        /// ���й�ָ��
        /// </summary>
        private IntPtr hdd;

        /// <summary>
        /// ͼ����Ϣ
        /// </summary>
        public BITMAPINFOHEADER  BITMAPINFOHEADER = new BITMAPINFOHEADER();

        /// <summary>
        /// Ҫ��ʾͼ��Ŀؼ�
        /// </summary>
        public Control Control=new Control();
        
        #endregion

        #region ����
        ///// <summary>
        ///// ͼ����Ϣ
        ///// </summary>
        //public BITMAPINFOHEADER BITMAPINFOHEADER
        //{
        //    set { _BITMAPINFOHEADER = value;}
        //    get { return _BITMAPINFOHEADER; }
        //}

        /// <summary>
        /// ��ʾͼ��Ŀؼ����
        /// </summary>
        public IntPtr Handle
        {
            get { return this.hdd; }
        }

        /// <summary>
        /// 
        /// </summary>
        public bool IsOpened
        {
            get { return this.hdd != IntPtr.Zero; }
        }
        #endregion

        #region ����
        /// <summary>
        /// ��ͼ��ˢ���
        /// </summary>
        public void Open()
        {
            this.hdd = DrawDibOpen();
            Debug.Assert(hdd != IntPtr.Zero);
            DrawDibBegin(hdd, IntPtr.Zero, this.Control.Width, this.Control.Height, ref  BITMAPINFOHEADER,  BITMAPINFOHEADER.biWidth, BITMAPINFOHEADER.biHeight, 0);
        }

        /// <summary>
        /// ����ͼ��
        /// </summary>
        /// <param name="data">ͼ������</param>
        /// <param name="control">Ҫ��ʾͼ��ؼ�</param>
        public void Draw(byte[] data, Control control)
        {
            try
            {
                using (Graphics g = control.CreateGraphics())
                {
                    IntPtr hdc = g.GetHdc();
                    bool b = DrawDibDraw(
                        hdd,
                        hdc,
                        0,
                        0,
                        Control.Width,
                        Control.Height,
                        ref  BITMAPINFOHEADER,
                        data,
                        0,
                        0,
                        BITMAPINFOHEADER.biWidth,
                        BITMAPINFOHEADER.biHeight,
                        0);
                    g.ReleaseHdc(hdc);
                }
            }
            catch { }
        }

        /// <summary>
        /// ����ͼ��
        /// </summary>
        /// <param name="data">ͼ������</param>
        public void Draw(byte[] data)
        {
            try
            {
                using (Graphics g = Control.CreateGraphics())
                {
                    IntPtr hdc = g.GetHdc();
                    bool b = DrawDibDraw(
                        hdd,
                        hdc,
                        0,
                        0,
                        Control.Width,
                        Control.Height,
                        ref BITMAPINFOHEADER,
                        data,
                        0,
                        0,
                        BITMAPINFOHEADER.biWidth,
                        BITMAPINFOHEADER.biHeight,
                        0);
                    g.ReleaseHdc(hdc);
                }
            }
            catch { }
        }

        /// <summary>
        /// �رջ�ˢ
        /// </summary>
        public void Close()
        {
            if (hdd != IntPtr.Zero)
            {
                DrawDibEnd(hdd);
                DrawDibClose(hdd);
            }
        }
        #endregion

        #region API����
        /*
		**  DrawDibOpen()
		**
		*/
        [DllImport("MSVFW32.dll")]
        public static extern IntPtr DrawDibOpen();

        /*
        **  DrawDibClose()
        **
        */
        [DllImport("MSVFW32.dll")]
        public static extern bool DrawDibClose(IntPtr hdd);

        [DllImport("MSVFW32.dll")]
        public static extern bool DrawDibBegin(
            IntPtr hdd,
            IntPtr hdc,
            int dxDst,
            int dyDst,
            ref BITMAPINFOHEADER lpbi,
            int dxSrc,
            int dySrc,
            int wFlags
            );
        [DllImport("MSVFW32.dll")]
        public static extern bool DrawDibEnd(IntPtr hdd);
        /*
        **  DrawDibDraw()
        **
        **  actualy draw a DIB to the screen.
        **
        */
        [DllImport("MSVFW32.dll")]
        public static extern bool DrawDibDraw(
            IntPtr hdd,
            IntPtr hdc,
            int xDst,
            int yDst,
            int dxDst,
            int dyDst,
            ref BITMAPINFOHEADER lpbi,
            byte[] lpBits,
            int xSrc,
            int ySrc,
            int dxSrc,
            int dySrc,
            uint wFlags
            );
        #endregion
    }
}