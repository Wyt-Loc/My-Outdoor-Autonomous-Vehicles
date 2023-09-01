using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using IMLibrary3;
using IMLibrary3.Data;
using IMLibrary3.Organization;

using System.Data.SQLite;

namespace OurMsg 
{
    public partial class FormDataManage : Form 
    {
        public FormDataManage()
        {
            InitializeComponent();
            this.treeView_Organization.AfterSelect += new TreeViewEventHandler(treeView_Organization_AfterSelect);
        }

        #region ����

        /// <summary>
        /// ��ǰ�����ҳ
        /// </summary>
        private int CurrPage = 0;

        /// <summary>
        /// ҳ������
        /// </summary>
        private int PageCount = 0;

        /// <summary>
        /// ÿһҳ��ʾ�ļ�¼��
        /// </summary>
        private int pageSize = 20;

        /// <summary>
        /// �Ի�����
        /// </summary>
        private IMLibrary3.Enmu.MessageType MessageType = IMLibrary3.Enmu.MessageType.Group;

        /// <summary>
        /// ��֯��������
        /// </summary>
        List<exGroup> Groups =null ;

        /// <summary>
        /// �û�
        /// </summary>
        List<exUser> Users = null;

        /// <summary>
        /// �Զ�����
        /// </summary>
        List<exRoom> Rooms = null;

        /// <summary>
        /// ��ǰ�û��ʺ�
        /// </summary>
        public string myUserID = "";

        /// <summary>
        /// ѡ���û��ʺ�
        /// </summary>
        string SelectUserID = "";

        /// <summary>
        /// ��ȡ�Ի���Ϣ
        /// </summary>
        IMLibrary3.Protocol.Message Msg =null;
        #endregion

        #region ��ͼ�ڵ�ѡ�к��¼�
        /// <summary>
        /// ��ͼ�ڵ�ѡ�к��¼�
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void treeView_Organization_AfterSelect(object sender, TreeViewEventArgs e)
        {
            this.txtRecord.Clear();

            this.ButLast.Enabled = false;
            this.ButDown.Enabled = false;
            this.ButFirst.Enabled = false;
            this.ButUp.Enabled = false;
            this.ButDelRecord.Enabled = false;

            this.PageCount = 0;
            this.CurrPage = 0;

            if (e.Node.Tag is exUser)
            {
                exUser user = e.Node.Tag as exUser;
                this.SelectUserID=user.UserID;
                this.MessageType = IMLibrary3.Enmu.MessageType.User;
            }
            else if (e.Node.Tag is exRoom)
            {
               exRoom room = e.Node.Tag as exRoom;
                this.SelectUserID = room.RoomID ;
                this.MessageType = IMLibrary3.Enmu.MessageType.Group;              
            }
            else if (e.Node.ImageIndex == 17)
                this.MessageType = IMLibrary3.Enmu.MessageType.Notice; 
    
            this.PageCount = GetPageCount(this.SelectUserID);
            if (this.PageCount > 0)
                DBRecordToRichTextBox(1);

            this.TextBoxPage.Text = this.CurrPage.ToString() + "/" + this.PageCount.ToString();
        }

        #endregion

        #region ��öԻ���¼��Ϣ������
        /// <summary>
        /// ��öԻ���¼��Ϣ������
        /// </summary>
        /// <param name="userID">������ID</param>
        /// <param name="MyID">�Լ���ID</param>
        /// <returns></returns>
        private int GetPageCount(string userID)//���ҳ��
        {
            int count = 0;

            string sql = "";
            switch (this.MessageType)
            {
                case IMLibrary3.Enmu.MessageType.User:
                    sql = string.Format("select Count(*) from [MsgRecord] where ((froms='{0}' and tos='{1}') or (froms='{1}' and tos='{0}')) And MessageType={2}  ORDER BY DateTime DESC", userID, myUserID, (byte)this.MessageType);
                    break;
                case IMLibrary3.Enmu.MessageType.Group:
                    sql = string.Format("select Count(*) from [MsgRecord] where  tos='{0}' and MessageType={1} ORDER BY DateTime DESC", SelectUserID, (byte)this.MessageType);
                    break;
                case IMLibrary3.Enmu.MessageType.Notice:
                    sql = string.Format("select Count(*) from [MsgRecord] where MessageType={0} ORDER BY DateTime DESC", (byte)this.MessageType);
                    break;
            }


            SQLiteDataReader dr = SQLiteDBHelper.ExecuteReader(sql, null);

            if (dr == null) return count;
            while (dr.Read())
            {
                count = Convert.ToInt32(dr[0]);
            }
            dr.Close(); dr.Dispose();


            int pageCount = Convert.ToInt32(count / this.pageSize);
            if ((count % this.pageSize) != 0)
                pageCount += 1;
            //MessageBox.Show(count.ToString());
            return pageCount;
        }

        private void addRecord(long ID)
        {
            //RecordIndex record = new RecordIndex(ID);
            //this.recordCollention.add(record);
        }
         
        #endregion

        #region �����ҳ��¼
        private void DBRecordToRichTextBox(int Page)
        {
            if (Page > PageCount) return;//���ҳ��������ǰҳ�������˳�
            if (Page <= 0) return;//���ҳ��Ϊ���������˳�

            this.txtRecord.Clear();
            this.CurrPage = Page;

            this.ButLast.Enabled = true;
            this.ButDown.Enabled = true;
            this.ButFirst.Enabled = true;
            this.ButUp.Enabled = true;
            this.ButDelRecord.Enabled = true;


            if (Page == this.PageCount)
            {
                this.ButLast.Enabled = false;
                this.ButDown.Enabled = false;
            }
            if (Page == 1)
            {
                this.ButFirst.Enabled = false;
                this.ButUp.Enabled = false;
            }
            this.TextBoxPage.Text = Page.ToString() + "/" + this.PageCount.ToString();

            bool IsSend = false;//��ʶ����Ϣ�Ƿ��ǶԷ����͵�

            #region sql���ʽ
            string sql = "";
            switch (this.MessageType)
            {
                case IMLibrary3.Enmu.MessageType.User:
                    sql = string.Format("select *  from [MsgRecord] where ((froms='{0}' and tos='{1}') or (froms='{1}' and tos='{0}')) And MessageType={2}  ORDER BY DateTime DESC limit " + (Page - 1) * pageSize + "," + pageSize, SelectUserID, myUserID, (byte)this.MessageType);
                    break;
                case IMLibrary3.Enmu.MessageType.Group:
                    sql = string.Format("select *  from [MsgRecord] where tos='{0}' and MessageType={1} ORDER BY DateTime DESC limit " + (Page - 1) * pageSize + "," + pageSize, SelectUserID, (byte)this.MessageType);
                    break;
                case IMLibrary3.Enmu.MessageType.Notice:
                    sql = string.Format("select *  from [MsgRecord] where MessageType={0} ORDER BY DateTime DESC limit " + (Page - 1) * pageSize + "," + pageSize, (byte)this.MessageType);
                    break;
            }
            #endregion

            try
            {
                SQLiteDataReader dr = SQLiteDBHelper.ExecuteReader(sql, null);
                if (dr == null) return;
                while (dr.Read())
                {
                    IsSend = false;//��ʶ����Ϣ���ǶԷ����͵�

                    #region �����Ϣ��
                    Msg = OpeRecordDB.GetDrMsg(dr);
                    if (Msg == null) continue;
                    #endregion

                    exUser sendUserInfo = findUser(Msg.from, Users);
                    if (sendUserInfo == null) return;

                    if (sendUserInfo.UserID == this.myUserID)//����ϢΪ�Է����͵�
                        IsSend = true;

                    int iniPos = txtRecord.TextLength;//��õ�ǰ��¼richBox������λ��
                    string title = sendUserInfo.UserName + "(" + sendUserInfo.UserID + ")" + Msg.DateTime ;
                    Color titleColor = Color.Red;
                    if (!IsSend)//������յ���Ϣ
                        titleColor = Color.Blue;

                    txtRecord.AppendText(title);
                    txtRecord.Select(iniPos, txtRecord.TextLength - iniPos);
                    txtRecord.SelectionFont = new Font("����", 10);
                    txtRecord.SelectionColor = titleColor;
                    txtRecord.AppendText("\n  ");

                    textMsgToRich(Msg, IsSend);
                }
                dr.Close(); dr.Dispose();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Source + ex.Message);
            }
        } 
        #endregion

        #region ����ı���Ϣ�� richBox
        /// <summary>
        /// ����ı���Ϣ��richBox
        /// </summary>
        /// <param name="IsSend">�Ƿ�Է����͵���Ϣ</param>
        private void textMsgToRich(IMLibrary3.Protocol.Message Msg, bool IsSend)//����ı���Ϣ��ͼƬ��richBox
        {
            int iniPos = this.txtRecord.TextLength;//��õ�ǰ��¼richBox������λ��

            if (Msg.ImageInfo != "")//�����Ϣ����ͼƬ�������ͼƬ
            {
                string[] imagePos = Msg.ImageInfo.Split('|');
                int addPos = 0;//
                int currPos = 0;//��ǰ��Ҫ��ӵ��ı�λ��
                int textPos = 0;
                for (int i = 0; i < imagePos.Length - 1; i++)
                {
                    string[] imageContent = imagePos[i].Split(',');//���ͼƬ���ڵ�λ�á�ͼƬ���ơ�ͼƬ����

                    currPos = Convert.ToInt32(imageContent[0]);//���ͼƬ���ڵ�λ��

                    this.txtRecord.AppendText(Msg.Content.Substring(textPos, currPos - addPos));
                    this.txtRecord.SelectionStart = this.txtRecord.TextLength;

                    textPos += currPos - addPos;
                    addPos += currPos - addPos;

                    Image image=null;
                    if ( txtRecord.GetPicture(imageContent[1]) == null)
                    {
                        if (System.IO.File.Exists(Application.StartupPath + "\\face\\" + imageContent[1] + ".gif") )
                        {
                            image = System.Drawing.Image.FromFile(Application.StartupPath + @"\face\" + imageContent[1] + ".gif");
                        }
                        else if (IsSend)//����ǶԷ����͹�����
                        {
                            if (System.IO.File.Exists(myUserID + "\\sendImage\\" + imageContent[1] + ".gif"))
                                image = System.Drawing.Image.FromFile(myUserID + "\\sendImage\\" + imageContent[1] + ".gif");
                        }
                        else if (!IsSend)//������Լ����͵�
                        {
                            if (System.IO.File.Exists(myUserID + "\\ArrivalImage\\" + imageContent[1] + ".gif"))
                                image = System.Drawing.Image.FromFile(myUserID + "\\ArrivalImage\\" + imageContent[1] + ".gif");
                        }
                    }
                    else
                        image = this.txtRecord.GetPicture(imageContent[1]).Image;
                     
                    this.txtRecord.addGifControl(imageContent[1], image);
                    addPos++;
                }
                this.txtRecord.AppendText(Msg.Content.Substring(textPos, Msg.Content.Length - textPos) + "\n");
            }
            else//�����Ϣ��û��ͼƬ����ֱ�������Ϣ�ı�
            {
                this.txtRecord.AppendText(Msg.Content + "\n");
            }

            this.txtRecord.Select(iniPos, this.txtRecord.TextLength - iniPos);
            this.txtRecord.SelectionFont =Msg.Font ;
            this.txtRecord.SelectionColor = Msg.Color ;
            this.txtRecord.Select(this.txtRecord.TextLength, 0);
        }
        #endregion

        #region ���û�֪ͨ��Ϣ���� RichTextBox �ؼ�
        /// <summary>
        /// ���û�֪ͨ��Ϣ���� RichTextBox �ؼ�
        /// </summary>
        /// <param name="msg">��Ϣ��</param>
        /// <param name="IsSend">����Ƿ�����Ϣ�����յ���Ϣ</param>
        public void NoticeMsgToRichTextBox(IMLibrary3.Protocol.Message msg,exUser user)//�����͵���Ϣ������ʷrich
        {
            int iniPos = txtRecord.TextLength;//��õ�ǰ��¼richBox������λ��

            if (msg.Title != "")
            {
                #region ��ӱ���
                txtRecord.AppendText(msg.Title);
                txtRecord.Select(iniPos, txtRecord.TextLength - iniPos);
                txtRecord.SelectionFont = new Font("����", 18);
                txtRecord.SelectionColor = msg.Color;
                txtRecord.SelectionAlignment = HorizontalAlignment.Center;
                txtRecord.AppendText("\n\n");
                #endregion
            }

            #region �������
            iniPos = txtRecord.TextLength;//��õ�ǰ��¼richBox������λ��

            #region //�����Ϣ����ͼƬ�������ͼƬ
            if (msg.ImageInfo != "")//�����Ϣ����ͼƬ�������ͼƬ
            {
                string[] imagePos = msg.ImageInfo.Split('|');
                int addPos = 0;//
                int currPos = 0;//��ǰ��Ҫ��ӵ��ı�λ��
                int textPos = 0;
                for (int i = 0; i < imagePos.Length - 1; i++)
                {
                    string[] imageContent = imagePos[i].Split(',');//���ͼƬ���ڵ�λ�á�ͼƬ���� 
                    currPos = Convert.ToInt32(imageContent[0]);//���ͼƬ���ڵ�λ��

                    this.txtRecord.AppendText(msg.Content.Substring(textPos, currPos - addPos));
                    this.txtRecord.SelectionStart = this.txtRecord.TextLength;

                    textPos += currPos - addPos;
                    addPos += currPos - addPos;

                    Image image;

                    if (this.txtRecord.GetPicture(imageContent[1]) == null)
                        image = System.Drawing.Image.FromFile(Application.StartupPath + @"\face\" + imageContent[1] + ".gif");
                    else
                        image = this.txtRecord.GetPicture(imageContent[1]).Image;

                    this.txtRecord.addGifControl(imageContent[1], image);
                    addPos++;
                }
                this.txtRecord.AppendText(msg.Content.Substring(textPos, msg.Content.Length - textPos) + "  \n");
            }
            #endregion
            #region  //�����Ϣ��û��ͼƬ����ֱ�������Ϣ�ı�
            else//�����Ϣ��û��ͼƬ����ֱ�������Ϣ�ı�
            {
                txtRecord.AppendText(msg.Content + "\n");
            }
            #endregion
            txtRecord.Select(iniPos, this.txtRecord.TextLength - iniPos);
            txtRecord.SelectionFont = msg.Font;
            txtRecord.SelectionColor = msg.Color;
            txtRecord.SelectionAlignment = HorizontalAlignment.Left;
            #endregion

            if (msg.remark != "")
            {
                #region ������
                iniPos = txtRecord.TextLength;//��õ�ǰ��¼richBox������λ��
                txtRecord.AppendText("\n" + msg.remark);
                txtRecord.Select(iniPos, txtRecord.TextLength - iniPos);
                txtRecord.SelectionFont = new Font("����", 13);
                txtRecord.SelectionColor = msg.Color;
                txtRecord.SelectionAlignment = HorizontalAlignment.Right;
                txtRecord.AppendText("\n  ");
                #endregion
            }


            iniPos = txtRecord.TextLength;//��õ�ǰ��¼richBox������λ��
            txtRecord.AppendText("\n�����û���" + user.UserName + "(" + user.UserID + ")\n");
            txtRecord.AppendText("����ʱ�䣺" + System.DateTime.Now.ToString() + "\n");
            txtRecord.Select(iniPos, txtRecord.TextLength - iniPos);
            txtRecord.SelectionFont = txtRecord.Font;
            txtRecord.SelectionAlignment = HorizontalAlignment.Left;

            txtRecord.Select(this.txtRecord.TextLength, 0);
        }
        #endregion

        #region �����������¼�

        private void toolStrip1_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {
            switch (e.ClickedItem.ToolTipText)
            {
                case "��һҳ":
                    DBRecordToRichTextBox(1);
                    break;
                case "��һҳ":
                    DBRecordToRichTextBox(this.CurrPage -1);
                    break;
                case "��һҳ":
                    DBRecordToRichTextBox(this.CurrPage+ 1);
                    break;
                case "���һҳ":
                    DBRecordToRichTextBox(this.PageCount);
                    break;
                case "�ر�":
                    this.Dispose();
                    break;
                case "ɾ����¼":
                    delRecord();
                    break;
            }
        }
        #endregion

        #region ɾ����¼
        /// <summary>
        /// ɾ����¼
        /// </summary>
        /// <param name="IsNotNotice"></param>
        private void delRecord()
        {
            string sql = "1=1";
            if (MessageBox.Show("ȷ��Ҫɾ����ǰ�����жԻ���¼��?", "��ʾ", System.Windows.Forms.MessageBoxButtons.YesNoCancel, System.Windows.Forms.MessageBoxIcon.Question) == System.Windows.Forms.DialogResult.Yes)
            {
                switch (this.MessageType)
                {
                    case IMLibrary3.Enmu.MessageType.User:
                        sql = string.Format("delete from [MsgRecord] where ((froms='{0}' and tos='{1}') or (froms='{1}' and tos='{0}')) And MessageType={2}", SelectUserID, myUserID, (byte)this.MessageType);
                        break;
                    case IMLibrary3.Enmu.MessageType.Group:
                        sql = string.Format("delete from [MsgRecord] where  tos='{0}' and MessageType={1}", SelectUserID, (byte)this.MessageType);
                        break;
                    case IMLibrary3.Enmu.MessageType.Notice:
                        sql = string.Format("delete from [MsgRecord] where MessageType={0}", (byte)this.MessageType);
                        break;
                }
                SQLiteDBHelper.ExecuteNonQuery(sql);
            }
            this.txtRecord.Clear();
        }
        #endregion

        #region �ؼ��˵��¼�
 
        private void contextMenuStrip_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {
            switch (e.ClickedItem.Name)
            {
                case "MenuItemCopy":
                    this.txtRecord.Copy();
                    break;
                case "MenuItemPaset":
                    this.txtRecord.Paste();
                    break;
                case "MenuItemCut":
                    this.txtRecord.Cut();
                    break;
                case "MenuItemDel":
                    this.txtRecord.SelectedText = "";
                    break;
                case "MenuItemSelAll":
                    this.Focus();
                    this.txtRecord.SelectAll();
                    break;
            }
        }

        private void RTBRecord_LinkClicked(object sender, LinkClickedEventArgs e)
        {
            try
            {
                System.Diagnostics.Process.Start(e.LinkText);
            }
            catch
            {
                //MessageBox.Show("�޷������ӡ�","��ʾ",MessageBoxButtons.OK,MessageBoxIcon.Information);
            }
        }

        #endregion

        private void FormDataManage_Load(object sender, EventArgs e)
        {
            LoadData();
        }

        public void LoadData()
        {
            Groups = OpeRecordDB.GetGroups();
            Users =  OpeRecordDB.GetUsers();
            Rooms =  OpeRecordDB.GetRooms();
            LoadLocalOrg();
        }

        #region ˽�з���

        #region ���ر������ݿ��е���֯��������
        private delegate void delegateLoadLocalOrg();//ί�д���
        /// <summary>
        /// ���ر������ݿ��е���֯����
        /// </summary>
        private void LoadLocalOrg()
        {
            if (this.treeView_Organization.Nodes.Count > 0) return;

            TreeNode nodeOrg = addNodeToTree("���з���", 14);
            TreeNode nodeGroups=addNodeToTree("��", 16);

            addNodeToTree("֪ͨ��Ϣ", 17);

            foreach ( exGroup group in this.Groups)
                group.IsShowOnlineInfo = false;
            foreach (exUser user in this.Users)
                user.IsShowOnlineInfo = false;

            TreeNode node = this.addGroupToTreeView(this.Groups);
            this.addUserToTreeView(this.Users, this.Groups);
            if (node == null) return;

            foreach (TreeNode nodeTemp in node.Nodes)
            {
                nodeOrg.Nodes.Add(nodeTemp);
            }

            ///���Ⱥ��
            foreach ( exRoom group in this.Rooms)
            {
                TreeNode nodeTemp = new TreeNode(group.RoomName + "(" + group.RoomID + ")");
                nodeTemp.Tag = group;
                nodeTemp.ImageIndex = 16;
                nodeTemp.SelectedImageIndex = 16;

                nodeGroups.Nodes.Add(nodeTemp);
            }

            nodeOrg.Expand();
            nodeGroups.Expand();
        }

        private TreeNode addNodeToTree(string Text, int imageIndex)
        {
            TreeNode node=new TreeNode(Text);
            node.SelectedImageIndex =imageIndex;
            node.ImageIndex =imageIndex ;
            this.treeView_Organization.Nodes.Add(node);
            return node;
        }
        #endregion

        #region ��treeView����ӷ���groups���ϲ����� TreeNode
        /// <summary>
        /// ��treeView����ӷ���groups���ϲ����� TreeNode
        /// </summary>
        /// <param name="treeView1"></param>
        /// <param name="groups"></param>
        /// <returns></returns>
        private TreeNode addGroupToTreeView(List<exGroup> groups)
        {
            if (groups == null) return null;
            TreeNode nodeTemp = new TreeNode();

            ///��Ӹ�����ڵ�
            foreach (exGroup group in groups)
                if (findGroup(group.SuperiorID, groups) == null)
                {
                    TreeNode node = new TreeNode();
                    node.Text = group.GroupName;
                    node.ToolTipText = group.GroupName;
                    node.ImageIndex = 14;
                    node.SelectedImageIndex = 15;
                    node.Tag = group;
                    group.TreeNode = node;
                    nodeTemp.Nodes.Add(node);
                }

            bool t = false;
            exGroup parentGroup;
            int rCount = 0;
            int sqrCount = (groups.Count * groups.Count);//���ѭ������

            while (!t && rCount <= sqrCount)//�����ѯ��δ������ѭ������û�г���������nƽ���������
            {
                t = true;
                foreach (exGroup group in groups)
                {
                    parentGroup = findGroup(group.SuperiorID, groups);//�ҵ��ϼ����Žڵ�
                    if (parentGroup != null && group.TreeNode == null) //���Ҫ��ӵĲ��Žڵ㲻�Ǹ����Žڵ��Ҵ˽ڵ㻹δ��� 
                    {
                        if (parentGroup.TreeNode != null)// ��ǰ���ϼ������Ѿ����ʱ����Ӳ���  
                        {
                            TreeNode node = new TreeNode();
                            node.Text = group.GroupName;
                            node.ToolTipText = group.GroupName;
                            node.ImageIndex = 14;
                            node.SelectedImageIndex = 15;
                            node.Tag = group;
                            group.TreeNode = node;
                            (parentGroup.TreeNode as TreeNode).Nodes.Add(node);

                            group.SuperiorGroup = parentGroup;//�����ϼ���
                        }
                        else//�����ǰ���Žڵ���ϼ����Ų��Ǹ����Žڵ㲢���ϼ����ŵ��ϼ����Ż�δ��ӣ�����Ӳ��ɹ���ѭ���������
                        {
                            t = false;
                        }
                    }
                    rCount++;//��ѯ������1��������ڲ���nƽ����δ��������ǿ�н���
                }
            }
            return nodeTemp;
        }
        #endregion

        #region ��treeView�е�groups������û�
        /// <summary>
        /// ��treeView�е�groups������û�
        /// </summary>
        /// <param name="groups"></param>
        /// <param name="users"></param>
        private void addUserToTreeView(List<exUser> users, List<exGroup> groups)
        {
            if (users == null || groups == null) return;

            foreach (exUser user in users)
            {
                exGroup group = findGroup(user.GroupID, groups);
                if (group != null && group.TreeNode != null)
                {
                    TreeNode node = new TreeNode();
                    node.Text = user.UserName;
                    node.ToolTipText = user.UserName;
                    node.ImageIndex = 1;
                    node.SelectedImageIndex = 1;
                    node.Tag = user;
                    user.TreeNode = node;
                    TreeNode groupNode = group.TreeNode as TreeNode;
                    if (groupNode != null)
                        groupNode.Nodes.Add(node);

                    //user.Group = group;
                    //user.Group.UserCount++;
                }

            }
        }
        #endregion

        #region ������
        /// <summary>
        /// ������
        /// </summary>
        /// <param name="GroupID"></param>
        /// <returns></returns>
        private  exGroup findGroup(string GroupID, List<exGroup> groups)
        {
            if (groups == null) return null;

            foreach (exGroup group in groups)
                if (group.GroupID == GroupID)
                    return group;
            return null;
        }
        #endregion

        #region �����û�
        /// <summary>
        /// �����û�
        /// </summary>
        /// <param name="userID"></param>
        /// <param name="users"></param>
        /// <returns></returns>
        private exUser findUser(string userID, List<exUser> users)
        {
            if (users == null) return null;

            foreach (exUser user in users)
                if (user.UserID == userID)
                    return user;
            return null;
        }
        #endregion

        #region �����Զ���Ⱥ��
        /// <summary>
        /// �����Զ���Ⱥ��
        /// </summary>
        /// <param name="GroupID"></param>
        /// <returns></returns>
        private exRoom findGroup(string RoomID, List<exRoom> Rooms)
        {
            if (Rooms == null) return null;

            foreach (exRoom room in Rooms)
                if (room.RoomID == RoomID)
                    return room;
            return null;
        }
        #endregion

        #endregion
    }
}