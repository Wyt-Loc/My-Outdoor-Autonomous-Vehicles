using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace IMLibrary3 
{
	/// <summary> 
	/// MyPicture ��ժҪ˵����
	/// </summary>
	public class MyPicture : System.Windows.Forms.PictureBox
    {
        #region �����������ɵĴ���
        /// <summary> 
		/// ����������������
		/// </summary>
		private System.ComponentModel.Container components = null;

        /// <summary>
        /// 
        /// </summary>
		public MyPicture()
		{
			// �õ����� Windows.Forms ���������������ġ�
			InitializeComponent();
			// TODO: �� InitializeComponent ���ú�����κγ�ʼ��
            BackColor  = Color.Transparent;
		}


		/// <summary> 
		/// ������������ʹ�õ���Դ��
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		/// <summary> 
		/// �����֧������ķ��� - ��Ҫʹ�ô���༭�� 
		/// �޸Ĵ˷��������ݡ�
		/// </summary>
		private void InitializeComponent()
		{
			components = new System.ComponentModel.Container();
		}
		#endregion


        /// <summary>
        /// ��¼��ǰͼƬ�Ƿ���Ҫ���͵��Է�
        /// </summary>
		public bool IsSend=false;//��ʶ��ͼƬ�Ƿ���Ҫ���͵��Է�,Ĭ�ϲ�����

        /// <summary>
        /// ��ǰͼƬ��richtextBox�е�λ��
        /// </summary>
        public int Pos = 0;

        /// <summary>
        /// ͼƬ�ļ���MD5ֵ
        /// </summary>
        public string  MD5 = ""; 

        /// <summary>
        /// ͼƬ�Ƿ����
        /// </summary>
        public bool IsLoad = false;


        /// <summary>
        /// ����ͼƬ����
        /// </summary>
		public void playGif()
		{
			System.Drawing.ImageAnimator.Animate(this.Image,new System.EventHandler(this.OnFrameChanged));
		}
 
		private void OnFrameChanged(object sender, EventArgs e) 
		{
			this.Invalidate();
		}


	}
}
