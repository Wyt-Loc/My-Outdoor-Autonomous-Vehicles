# -*- coding: utf-8 -*-
# @Time    : 2023/8/5 22:08
# @Author  : Wyt
# @File    : STARTUPCLIENT.py
# @Description : 自动执行脚本
import threading
import time
from socket import *
import TcpControl


# 远程启动客户端
class StartUP:
    # 汽车自动启动
    # 建立TCP连接
    def __init__(self, ip: str, com: int):
        server_ip = ip  # 服务器IP
        server_port = com  # 服务器端口
        # 创建socket
        self.tcp_client_socket = socket(AF_INET, SOCK_STREAM)
        # 链接服务器
        self.tcp_client_socket.connect((server_ip, server_port))

    # 发送TCP数据
    def sendTcpStartUpData(self, data: str):
        self.tcp_client_socket.send(data.encode("gbk"))

    # 接收TCP数据
    def recvTcpReturnData(self) -> str:
        return self.tcp_client_socket.recv(1024).decode("gbk")

    # 校验用户
    def validatingUser(self):
        # 首先验证用户
        # 验证账号
        send_data = "usr123"
        self.sendTcpStartUpData(send_data)
        time.sleep(1)
        # 验证密码
        send_data = "pwd123"
        self.sendTcpStartUpData(send_data)
        # 校验用户是否正确
        recvData = self.recvTcpReturnData()
        if recvData == "usrok":
            print("用户登录成功")
        if recvData == "usrerror":
            print("用户名或密码错误，请重新尝试登录")

    # 是否连接成功
    def isConnok(self):
        # 确认已建立连接
        send_data = "isok?"
        self.sendTcpStartUpData(send_data)
        # 接收对方发送过来的数据，最大接收1024个字节
        recvData = self.recvTcpReturnData()
        if recvData == "isok":
            print("设备连接成功")
            return 1
        return 0

    def startUpOver(self):
        send_data = "okok"
        self.sendTcpStartUpData(send_data)
        recvData = self.recvTcpReturnData()
        if recvData == "over":
            # 关闭套接字
            self.tcp_client_socket.close()
            print("启动程序执行完成")

    # 任务消息校验
    def taskMessageValidation(self, taskname: str):
        send_data = taskname
        print(taskname, taskname + "任务开始匹配....")
        self.sendTcpStartUpData(send_data)
        recvData = self.recvTcpReturnData()
        if recvData == taskname + "_isok":
            print(taskname + "任务匹配完成")

    # 任务_1 启动流程
    def task_1(self):
        # 首先进行任务消息校验
        self.taskMessageValidation("task_1")
        # 发送客户端 task_1 启动完成标志
        send_data = "clienttask_1isok"
        self.sendTcpStartUpData(send_data)
        print("task_1 client 已启动")
        # 等待服务器 task_1 启动
        recvData = self.recvTcpReturnData()
        print("recvData = ", recvData)
        if recvData == "tcpcontrol.pyosopem":
            th1 = threading.Thread(target=TcpControl.main_usart)
            th1.start()  # 通过脚本开启TCP通讯子线程
            print("tcp通讯建立完成")

    # 任务_2 启动流程
    def task_2(self):
        # 首先进行任务消息校验
        self.taskMessageValidation("task_2")
        # 开始启动

    def carStartUp(self):
        # 判断是否连接成功
        self.isConnok()
        # 校验用户
        self.validatingUser()
        # 执行代码部分  ****************************************
        self.task_1()  # 启动任务_1
        self.task_2()  # 启动任务_2
        # 执行代码部分结束 **************************************
        # 启动过程结束校验
        self.startUpOver()


if __name__ == '__main__':
    start = StartUP("8.130.115.65", 7788)
    start.sendTcpStartUpData("dev_2")  # 发送设备id
    time.sleep(1)
    start.carStartUp()
