import os
import threading
import time
from socket import *


# 远程启动服务器
class StartUPServer:
    receData = ""
    tcpClient = []
    addr = []

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
                th1 = threading.Thread(target=self.datachuli, args=(self.tcpClient,))
                th1.start()

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

    def task_1_StartUp(self, ip):
        receData = self.receTcpSendData(ip)
        if receData == "task_1":
            print("task_1 已发现")
        time.sleep(1)
        self.sendReplyData(ip, "task_1_isok")
        print("task_1匹配完成")
        receData = self.receTcpSendData(ip)
        if receData == "clienttask_1isok":
            print("服务器 task_1 启动")

    def task_2_StartUp(self, ip):
        receData = self.receTcpSendData(ip)
        if receData == "task_2":
            print("task_2 已发现")
        time.sleep(1)
        self.sendReplyData(ip, "task_2_isok")
        print("task_2匹配完成")

    def datachuli(self, ip):
        # 等待客户端连接
        self.isConn(ip)
        # 校验客户端用户
        self.validatingUser(ip)
        # 执行代码部分 **********************************************
        self.task_1_StartUp(ip)
        self.task_2_StartUp(ip)
        # 执行代码部分结束 ******************************************
        self.startUpOver(ip)


if __name__ == '__main__':
    stratup = StartUPServer("127.0.0.1", 7788)
    stratup.createNewConnThread()
