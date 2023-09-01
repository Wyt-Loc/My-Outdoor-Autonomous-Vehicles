using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using IMLibrary3.Organization;
  
namespace OurMsg
{
    public partial class FormCreateRoom : Form
    {
        #region ��ʼ��
        /// <summary>
        /// 
        /// </summary>
        /// <param name="MyUserID"></param>
        /// <param name="MyUserName"></param>
        /// <param name="isCreate"></param>
        public FormCreateRoom(string MyUserID, string MyUserName, bool isCreate)
        {
            InitializeComponent();
            myUserID = MyUserID;
            myUserName = MyUserName;

            this.textBoxCreateUser.Text = myUserID;

            ListViewItem item = new ListViewItem(myUserID);
            item.SubItems.Add(myUserName);
            item.ImageIndex = 0;
            item.Tag = myUserID;
            listViewGroupUsers.Items.Add(item);
            setSelUsersCount();

            if (isCreate)
                this.butCreateGroup.Visible = true;
            else
                this.butOK.Visible = true;
            IsCreate = isCreate;
        }
        #endregion

        #region ������
        /// <summary>
        /// ����Ⱥ����Ϣ��ʱ������
        /// </summary>
        private int outTime = 0;

        /// <summary>
        /// �Ƿ񴴽�Ⱥ�飬��Ϊ����Ⱥ����Ϣ
        /// </summary>
        private bool IsCreate = false;

        /// <summary>
        /// ���������Ⱥ����Ϣ�Ƿ�ɹ�
        /// </summary>
        public bool isUpdateSuccess = false;
         
        /// <summary>
        /// �û�ID
        /// </summary>
        public string myUserID { get; set; }
        
        /// <summary>
        /// �û���
        /// </summary>
        public string myUserName { get; set; }
         
        private string oldVersion = "";

        private exRoom _Room = new exRoom();
        /// <summary>
        /// Ⱥ����Ϣ
        /// </summary>
        public exRoom Room
        {
            set
            {
                _Room = value;

                this.textBoxCreateUser.Text = value.CreateUserID;
                this.textBoxGroupID.Text = value.RoomID;
                this.textBoxGroupNotice.Text = value.Notice;
                this.textBoxGroupName.Text = value.RoomName;

                foreach ( exUser user in value.Users.Values )
                {
                    bool t = false;
                    foreach (ListViewItem tempItem in listViewGroupUsers.Items)
                        if ((tempItem.Tag as string) == user.UserID)
                        {
                            t = true;
                            continue;
                        }

                    if (!t)
                    {
                        ListViewItem item = new ListViewItem(user.UserID);
                        item.SubItems.Add(user.UserName);
                        item.ImageIndex = 0;
                        item.Tag = user.UserID;
                        listViewGroupUsers.Items.Add(item);
                        setSelUsersCount();
                    }
                }
                oldVersion = _Room.RoomName.Trim() + _Room.Notice.Trim() + _Room.UserIDs;
            }
            get { return _Room; }
        }

        #endregion

        #region �¼�

        /// <summary>
        /// ����Ⱥ���¼�����
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="Room"></param>
        public delegate void CreateRoomEventHandler(object sender,exRoom Room);

        /// <summary>
        /// ����Ⱥ���¼�
        /// </summary>
        public event CreateRoomEventHandler CreateRoom;


        /// <summary>
        /// ����Ⱥ����Ϣ�¼�
        /// </summary>
        /// <param name="sender">�����¼�����</param>
        /// <param name="group">Ⱥ�����</param>
        public delegate void UpdateRoomEventHandler(object sender, exRoom Room);
        /// <summary>
        /// ����Ⱥ����Ϣ�¼�
        /// </summary>
        public event UpdateRoomEventHandler UpdateRoom;

        #endregion

        #region �����¼�
        private void FormCreateRoom_Load(object sender, EventArgs e)
        {


        }  
        #endregion

        #region ����û�
        private void butAddUsers_Click(object sender, EventArgs e)
        {
            FormUsersToGroup fs = new FormUsersToGroup();
            fs.ShowDialog(this);
            
            foreach ( exUser user in fs.Users)
            {
                bool t = false;
                foreach (ListViewItem tempItem in listViewGroupUsers.Items)
                    if ((tempItem.Tag as string) == user.UserID)
                    {
                        t = true;
                        continue;
                    }

                if (!t)
                {
                    ListViewItem item = new ListViewItem(user.UserID);
                    item.SubItems.Add(user.UserName);
                    item.ImageIndex = 0;
                    item.Tag = user.UserID;
                    listViewGroupUsers.Items.Add(item);
                    setSelUsersCount();
                }
            }
        }
        #endregion

        #region ȡ��
        private void butCancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }
        #endregion

        #region �����û�����
        private void setSelUsersCount()
        {
            labelUserCount.Text = listViewGroupUsers.Items.Count.ToString();
        }
        #endregion

        #region ѡ���û�����
        private void listViewGroupUsers_SelectedIndexChanged(object sender, EventArgs e)
        {
            foreach (ListViewItem item in listViewGroupUsers.Items)
                if (item.Selected)
                    item.BackColor = Color.Blue;
                else
                    item.BackColor = listViewGroupUsers.BackColor;

            if (listViewGroupUsers.SelectedItems.Count > 0
                  && (listViewGroupUsers.SelectedItems[0].Tag as string) !=myUserID )
                this.butDelUser.Enabled = true;
            else
                this.butDelUser.Enabled = false;
        }
        #endregion

        #region ɾ���û�
        private void butDelUser_Click(object sender, EventArgs e)
        {
            if (listViewGroupUsers.SelectedItems.Count > 0)
            {
                listViewGroupUsers.Items.Remove(listViewGroupUsers.SelectedItems[0]);
                setSelUsersCount();
                this.butDelUser.Enabled = false;
            }
        }
        #endregion

        #region ��ʱʱ���¼�
        private void timer1_Tick(object sender, EventArgs e)
        {
            outTime++;
            if (isUpdateSuccess)
            {
                timer1.Enabled = false;
                if (!IsCreate)
                {
                    MessageBox.Show("��������Ϣ�ɹ���", "��ʾ", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                else
                    MessageBox.Show("��������Ϣ�ɹ���Ⱥ���룺"+ this.Room.RoomID, "��ʾ", MessageBoxButtons.OK, MessageBoxIcon.Information);

                this.Close();
            }
            else if (outTime > 10)
            {
                timer1.Enabled = false;
                MessageBox.Show("�������������Ϣ��ʱ�������²�����", "��ʾ", MessageBoxButtons.OK, MessageBoxIcon.Information);

                this.butCreateGroup.Enabled = true;
                this.butOK.Enabled = true;

                outTime = 0;
            }
        }
        #endregion

        #region ����Ⱥ�鰴ť
        private void butCreateGroup_Click(object sender, EventArgs e)
        {  
            if (this.textBoxGroupName.Text.Trim() == "")
            {
                MessageBox.Show("��������Ϊ��", "��ʾ", MessageBoxButtons.OK, MessageBoxIcon.Information);
                this.textBoxGroupName.Focus();
                return;
            }

            this.butCreateGroup.Enabled = false;
            this.isUpdateSuccess = false;

           exRoom NewRoom = new exRoom();
            NewRoom.RoomID = this.textBoxGroupID.Text.Trim();
            NewRoom.UserIDs = "";
            NewRoom.CreateUserID = this.textBoxCreateUser.Text;//Ⱥ�鴴���û����
            NewRoom.RoomName = this.textBoxGroupName.Text;//Ⱥ������
            NewRoom.Notice = this.textBoxGroupNotice.Text;//Ⱥ��֪ͨ

            foreach (ListViewItem item in this.listViewGroupUsers.Items)
                NewRoom.UserIDs += item.Text + ";";

            if (CreateRoom != null)
                CreateRoom(this, NewRoom);

            this.timer1.Enabled = true;//����������
        }
        #endregion

        #region ����Ⱥ�鰴ť�¼�
        private void butOK_Click(object sender, EventArgs e)
        {
            if (!IsCreate && myUserID  != textBoxCreateUser.Text)
            {
                this.Close();
                return;
            }

            if (this.textBoxGroupName.Text.Trim() == "")
            {
                MessageBox.Show("��������Ϊ��", "��ʾ", MessageBoxButtons.OK, MessageBoxIcon.Information);
                this.textBoxGroupName.Focus();
                return;
            } 
            this.butOK.Enabled = false;
            this.isUpdateSuccess = false;

           exRoom NewRoom = new exRoom();
            NewRoom.UserIDs = "";
            NewRoom.RoomID= this.textBoxGroupID.Text;//����
            NewRoom.CreateUserID = this.textBoxCreateUser.Text;//Ⱥ�鴴���û����
            NewRoom.RoomName = this.textBoxGroupName.Text;//Ⱥ������
            NewRoom.Notice = this.textBoxGroupNotice.Text;//Ⱥ��֪ͨ

            foreach (ListViewItem item in this.listViewGroupUsers.Items)
                NewRoom.UserIDs += item.Text + ";";

            string newVersion = NewRoom.RoomName.Trim() + NewRoom.Notice.Trim() + NewRoom.UserIDs;

            if (newVersion == oldVersion)//���δ���޸ģ����˳�
            {
                this.Close();
                return;
            }

            if (this.UpdateRoom != null)
                this.UpdateRoom(this, NewRoom);

            this.timer1.Enabled = true;//����������
        }
        #endregion


    }

}