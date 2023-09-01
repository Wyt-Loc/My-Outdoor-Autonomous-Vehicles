using System;
using System.ComponentModel;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;
using System.Net;
using System.IO;

namespace IMLibrary3.Components
{
    public partial class WebFile : Component
    {
        /// <summary>
        /// 
        /// </summary>
        public WebFile()
        {
            InitializeComponent();
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="container"></param>
        public WebFile(IContainer container)
        {
            container.Add(this);
            InitializeComponent();
        }

        #region ������
        private System.Net.NetworkCredential netCre;// = new NetworkCredential();
        /// <summary>
        /// �ļ�MD5ֵ
        /// </summary>
        public string FileMD5Value;

        public TFileInfo TFileInfo = new  TFileInfo();
        #endregion

        #region  �ļ������¼�
        /// <summary>
        /// �¼������ͻ��յ��ļ�����
        /// </summary>
        /// <param name="sender">����</param>
        /// <param name="e"></param>
        public delegate void fileTransmitEventHandler(object sender, IMLibrary3.fileTransmitEvnetArgs e);//���ͻ��յ��ļ����� 

        /// <summary>
        /// 
        /// </summary>
        public event fileTransmitEventHandler fileTransmitted;

        /// <summary>
        /// 
        /// </summary>
        public event fileTransmitEventHandler fileTransmitError;

        /// <summary>
        /// 
        /// </summary>
        public event fileTransmitEventHandler fileTransmitting;

        #endregion

        #region ����Web�ļ�
       
        /// <summary>
        /// ����Web�ļ�
        /// </summary>
        /// <param name="WebURI"></param>
        /// <param name="LocalFile"></param>
        public void DownloadFile(string WebURI, string LocalFile)
        {
            if (this.netCre == null) this.netCre = new NetworkCredential("", "", "");
            DownloadFile(WebURI, LocalFile, this.netCre.UserName, this.netCre.Password, this.netCre.Password);
        }

       

        /// <summary>
        /// ����Web�ļ�
        /// </summary>
        /// <param name="webURI">Ҫ�����ļ�����ַ</param>
        /// <param name="localFile">�ļ�����·��</param>
        /// <param name="UserName">�����û���</param>
        /// <param name="password">����</param>
        /// <param name="domain">����</param>
        public void DownloadFile(string webURI, string localFile, string UserName, string password, string domain)
        {
            if (System.IO.File.Exists(localFile))
                System.IO.File.Delete(localFile);//����ļ����ڣ���ɾ��

            FileStream fw = null;
            try
            {

                WebClient myWebClient = new WebClient();

                this.netCre = new NetworkCredential(UserName, password, domain);
                myWebClient.Credentials = this.netCre;

                long fileLen = getDownloadFileLen(webURI);//��������ļ��Ĵ�С

                Stream readStream = myWebClient.OpenRead(webURI);//��WEB�ļ���׼����

                if (readStream.CanRead)
                {
                    int CurrDownLoadLen = 0;//���ļ�����
                    int maxReadWriteFileBlock = 1024 * 5;//һ�ζ��ļ�50k   
                    byte[] FileBlock;
                    FileBlock = new byte[maxReadWriteFileBlock];
                     fw = new FileStream(localFile, FileMode.Append, FileAccess.Write, FileShare.Read);
                    while (true)
                    {
                        int l = readStream.Read(FileBlock, 0, FileBlock.Length);//��WEB�ļ�����������
                        if (l == 0) break;
                        ////////////////////////�ļ�����
                        fw.Write(FileBlock, 0, l);
                        /////////////////////////
                        CurrDownLoadLen += l;

                        TFileInfo.fullName = localFile;
                        TFileInfo.Length =fileLen;
                        TFileInfo.CurrLength=CurrDownLoadLen;
                        TFileInfo.MD5 =FileMD5Value;

                        if (this.fileTransmitting != null)
                            this.fileTransmitting(this,new fileTransmitEvnetArgs( TFileInfo));
                    }
                    fw.Close();
                    fw.Dispose();
                }
                readStream.Close();
                myWebClient.Dispose();

                    if (this.fileTransmitted != null)
                        this.fileTransmitted(this, new fileTransmitEvnetArgs(TFileInfo));

            }
            catch (Exception ex)
            {
                if (fw != null)
                {
                    fw.Close();
                    fw.Dispose();
                }
                if (this.fileTransmitError != null)
                    this.fileTransmitError(this, new  fileTransmitEvnetArgs(TFileInfo));
            }
        }
 
        #endregion
         
        #region web�ļ��ϴ�
        /// <summary>
        /// web�ļ��ϴ�
        /// </summary>
        /// <param name="WebURI">�ļ��ϴ������ַ</param>
        /// <param name="LocalFile">�����ϴ��ļ���·��</param>
        /// <param name="isMD5file">�ļ����Ƿ�Ҫ����MD5�㷨��ȡ</param>
        public void UploadFile(string WebURI, string LocalFile,bool isMD5file)
        {
            if (this.netCre == null) this.netCre = new NetworkCredential("", "", "");
            this.UploadFile(WebURI, LocalFile, this.netCre.UserName, this.netCre.Password, this.netCre.Password, isMD5file);
        }
         
        /// <summary>
        ///  web�ļ��ϴ�
        /// </summary>
        /// <param name="webURI">�ļ��ϴ������ַ</param>
        /// <param name="localFile">�����ϴ��ļ���·��</param>
        /// <param name="UserName">�����û���</param>
        /// <param name="password">����</param>
        /// <param name="domain">����</param>
        /// <param name="isMD5file">�ļ����Ƿ�Ҫ����MD5�㷨��ȡ</param>
        public void UploadFile(string webURI, string localFile, string UserName, string password, string domain, bool isMD5file)
        {
            try
            {
                // Local Directory File Info
                if (!System.IO.File.Exists(localFile))//����ļ�������
                {
                    this.TFileInfo.Message ="Ҫ�ϴ����ļ������ڣ���ȷ���ļ�·���Ƿ���ȷ��";
                    if (this.fileTransmitError != null)
                        this.fileTransmitError(this, new fileTransmitEvnetArgs(TFileInfo));
                    return;
                }

                System.IO.FileInfo fInfo = new FileInfo(localFile);//��ȡ�ļ�����Ϣ

                if (isMD5file)//�����Ҫ���ļ�MD5,��ִ��MD5
                    webURI += "\\" + IMLibrary3.Security.Hasher.GetMD5Hash(localFile) + fInfo.Extension;//��ȡ�ļ���MD5ֵ
                else
                    webURI += "\\" + fInfo.Name;// +fInfo.Extension;//��ȡ�ļ���MD5ֵ

                // Create a new WebClient instance.
                WebClient myWebClient = new WebClient();
                this.netCre = new NetworkCredential(UserName, password, domain);
                myWebClient.Credentials = this.netCre;

                if (getDownloadFileLen(webURI) == fInfo.Length)//������������Ѿ��д��ļ��������˳�
                {
                    if (this.fileTransmitted != null)
                        this.fileTransmitted(this, new  fileTransmitEvnetArgs(TFileInfo));
                    return;
                }
                else
                {
                    Stream postStream = myWebClient.OpenWrite(webURI, "PUT");
                    if (postStream.CanWrite)
                    {
                        byte[] FileBlock;
                        int readFileCount = 0;//���ļ�����
                        int maxReadWriteFileBlock = 1024 * 30;//һ�ζ��ļ�30k   
                        long offset = 0;

                        readFileCount = (int)fInfo.Length / maxReadWriteFileBlock;//����ļ���д����

                        if ((int)fInfo.Length % maxReadWriteFileBlock != 0)
                            readFileCount++;//�����д�ļ����࣬�����д������1

                        for (int i = 0; i < readFileCount; i++)
                        {   //�����Ƕ�һ���ļ����ڴ����
                            if (i + 1 == readFileCount)//��������һ�ζ�д�ļ����������ļ�β����ȫ�����뵽�ڴ�
                                FileBlock = new byte[(int)fInfo.Length - i * maxReadWriteFileBlock];
                            else
                                FileBlock = new byte[maxReadWriteFileBlock];

                            ////////////////////////�ļ�����
                            FileStream fw = new FileStream(localFile, FileMode.Open, FileAccess.Read, FileShare.Read);
                            offset = i * maxReadWriteFileBlock;
                            fw.Seek(offset, SeekOrigin.Begin);//�ϴη��͵�λ��
                            fw.Read(FileBlock, 0, FileBlock.Length);
                            fw.Close();
                            fw.Dispose();
                            ///////////////////////////

                            postStream.Write(FileBlock, 0, FileBlock.Length);

                            TFileInfo.fullName = localFile;
                            TFileInfo.Name = fInfo.Name;
                            TFileInfo.Length = fInfo.Length;
                            TFileInfo .CurrLength =offset + FileBlock.Length;
                            TFileInfo.MD5 =FileMD5Value;
                            if (this.fileTransmitting != null)
                                this.fileTransmitting(this, new fileTransmitEvnetArgs(TFileInfo));
                        }
                    }
                    postStream.Close();
                    myWebClient.Dispose();
                }
                if (this.fileTransmitted != null)
                    this.fileTransmitted(this, new  fileTransmitEvnetArgs(TFileInfo));
            }
            catch (Exception ex)
            {
                TFileInfo .Message =ex.Message + ex.Source;
                if (this.fileTransmitError != null)
                    this.fileTransmitError(this, new  fileTransmitEvnetArgs(TFileInfo));
            }
        }
        #endregion
         
        #region ��ȡҪ�����ļ��Ĵ�С
        /// <summary>
        /// ��ȡҪ�����ļ��Ĵ�С
        /// </summary>
        /// <param name="WebURI">�ļ�URI</param>
        /// <returns></returns>
        public long getDownloadFileLen(string WebURI)
        {
            if (this.netCre == null) this.netCre = new NetworkCredential("", "", "");
            return getDownloadFileLen(WebURI, this.netCre.UserName, this.netCre.Password, this.netCre.Password);
        }

        /// <summary>
        /// ��ȡҪ�����ļ��Ĵ�С
        /// </summary>
        /// <param name="WebURI">��Ҫ�����ļ�����ַ</param>
        /// <param name="UserName">�����û���</param>
        /// <param name="password">����</param>
        /// <param name="domain">����</param>
        /// <returns></returns>
        public long getDownloadFileLen(string WebURI, string UserName, string password, string domain)
        {
            long len = 0;//��¼Ҫ�����ļ��Ĵ�С 
            try
            {
                HttpWebRequest req = (HttpWebRequest)WebRequest.Create(WebURI);
                req.Credentials = new NetworkCredential(UserName, password, domain);
                HttpWebResponse res = (HttpWebResponse)req.GetResponse();
                len = res.ContentLength;
                res.Close();
            }
            catch (Exception ex)
            {
                TFileInfo .Message =ex.Message + ex.Source;
                if (this.fileTransmitError != null)
                    this.fileTransmitError(this, new  fileTransmitEvnetArgs(TFileInfo));
            }
            return len;
        }
        #endregion
    }
}
