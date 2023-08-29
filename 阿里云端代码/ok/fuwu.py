import threading
import time
from socket import *
import os


# 远程启动服务器
class StartUPServer:
    receData = ""
    tcpClient = []
    addr = []
    threads = [0,0,0]  # 线程池
    tcp_id = [0,0,0]  # 设备id消息，用来分辨是什么设备，客户端还是服务器

    # 开启服务器
    def __init__(self, ip: str, com: int):
        self.tcpServer = socket(AF_INET, SOCK_STREAM)  # 创建TCP的套接字
        self.local_host = (ip, com)  # 创建服务端的IP及端口
        self.tcpServer.setsockopt(SOL_SOCKET, SO_REUSEADDR, True)  # 设置端口复用
        self.tcpServer.bind(self.local_host)  # 绑定IP及端口
        self.tcpServer.listen(10)  # 使服务端进入监听状态，最多可连接10个客户端
        print("Listen...")  # 表示等待连接

    def createNewConnThread(self):
        # 创建连接服务
        while True:
            self.tcpClient, self.addr = self.tcpServer.accept()  # 接受客户端的连接，获取客户端的信息
            vb1 = self.tcpClient.recv(1204).decode()  # 接收身份信息
            print(vb1)
            print(self.tcpClient, self.addr, "已连接")
            if vb1 == "dev_1":
                self.threads[0] = self.tcpClient # 将TCP连接对象加入到线程池
                self.tcp_id[0] = vb1
                print("-----------------")
                print(self.threads)
                print(self.tcp_id)
                print("-------------------")
                th1 = threading.Thread(target=self.datachuli, args=(self.tcpClient,vb1))
                th1.start()
            if vb1 == "dev_2":
                self.threads[1] = self.tcpClient
                self.tcp_id[1] = vb1
                print("---------------")
                print(self.threads)
                print(self.tcp_id)
                print("---------------")
                th2 = threading.Thread(target=self.datachuli, args=(self.tcpClient,vb1))
                th2.start()

    # 接收TCP数据
    @staticmethod
    def receTcpSendData(ip):
        return ip.recv(1024).decode("gbk")

    @staticmethod
    def sendReplyData(ip, data: str):
        ip.send(data.encode('gbk'))

    def validatingUser(self, ip):
        # 校验用户
        receData = self.receTcpSendData(ip)
        if receData == "usr123":
            receData = self.receTcpSendData(ip)
            if receData == "pwd123":
                print("用户校验成功")
                self.sendReplyData(ip, "usrok")
            else:
                self.sendReplyData(ip, "usrerror")
                print("用户名或密码错误")

    def isConn(self, ip):
        # 判断是否连接
        receData = self.receTcpSendData(ip)
        if receData == "isok?":
            self.sendReplyData(ip, "isok")

    def startUpOver(self, ip):
        receData = self.receTcpSendData(ip)
        if receData == "okok":
            self.sendReplyData(ip, "over")

    def task_1_StartUp(self, ip, id):
        if id == "dev_1":
            receData = self.receTcpSendData(ip)
            if receData == "task_1":
                print("task_1 已发现")
                time.sleep(1)
                self.sendReplyData(ip, "task_1_isok")
                print("task_1匹配完成")
                receData = self.receTcpSendData(ip)
                if receData == "clienttask_1isok":
                    print("服务器 task_1 启动")
                    os.popen("python3 server.py")  # 打开服务器服务
                    print("server.py 启动完成")
                    self.sendReplyData(ip, "tcpcontrol.py")
                    receData = self.receTcpSendData(ip)
                    if receData == "tcpcontrol.pyisok":
                        print("打开TCP2")
                        try:
                            self.sendReplyData(self.threads[1], "tcpcontrol.pyisopen")
                        except:
                            pass
        if id == "dev_2":
            receData = self.receTcpSendData(ip)
            if receData == "task_1":
                print("task_1 已发现")
                time.sleep(1)
                self.sendReplyData(ip, "task_1_isok")
                print("task_1匹配完成")
                receData = self.receTcpSendData(ip)
                if receData == "clienttask_1isok":
                    print("服务器 task_1 启动")
                    self.sendReplyData(self.threads[1],"tcpcontrol.pyosopem")

    def task_2_StartUp(self, ip, id):
        if id == "dev_1":
            receData = self.receTcpSendData(ip)
            if receData == "task_2":
                print("task_2 已发现")
                self.sendReplyData(ip, "task_2_isok")
                print("task_2匹配完成")
                # 校验完成
            else:
                print("用户校验失败")
                return 0
            receData = self.receTcpSendData(ip)
            if receData == "start_sxt":
                print("开启摄像头服务")  # 这块已经开启了 所以不做具体处理
                os.popen("python3 server.py")
                sendData = "start_sxtisok"
                self.sendReplyData(ip,sendData)
                print("fuwukaiqi")
            
        if id == "dev_2":
            receData = self.receTcpSendData(ip)
            if receData == "task_2":
                print("task_2 摄像头服务请求已发现")
                self.sendReplyData(ip, "task_2_isok")
                print("task_2摄像头服务请求匹配完成")
                # 校验完成
            else:
                print("用户校验失败")
                return 0
            print("摄像头设备接成功")
    
    def datachuli(self, ip,id):
        print(ip,'开始')
        # 等待客户端连接
        self.isConn(ip)
        # 校验客户端用户
        self.validatingUser(ip)
        # 执行代码部分 **********************************************
        # self.task_1_StartUp(ip,id)
        self.task_2_StartUp(ip,id)
        # 执行代码部分结束 ******************************************
        self.startUpOver(ip)
        print(ip,"结束")

if __name__ == '__main__':
    stratup = StartUPServer("172.24.34.71", 7788)
    stratup.createNewConnThread()

