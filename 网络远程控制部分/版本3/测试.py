# -*- coding: utf-8 -*-
# @Time    : 2023/6/30 10:32
# @Author  : Wyt
# @File    : 测试.py
import datetime
import time
import socket
import threading


class GetTimeInfo:
    """
    得到时间类
    1. 主要是几个时间
        1.年月日时分秒毫 详细的时间
        2.程序运行的时间，从开始运行到当前时刻的时间，最终结果换算为毫秒
        为了方便打log 返回值一律采用 str 类型
    """
    detailedTime = ""
    runningTime = ""
    startTime = ""
    stopTime = ""

    def __init__(self) -> None:
        # 得到服务开启时的时间
        self.startTime = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')
        with open("Modules/Control/redme.txt", "a", encoding="utf-8") as f:
            f.writelines("In " + self.startTime + " TcpServer start Running\n")

    def __del__(self) -> None:
        # 得到服务关闭时的时间
        self.stopTime = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')
        print(self.stopTime)

    def getDetailedTime(self) -> str:
        # 得到详细时间
        self.detailedTime = str(datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f'))
        return self.detailedTime

    @staticmethod
    def getRunningTime() -> str:
        # 得到当前时间
        curtime = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')
        return curtime

    def getTimeDiff(self) -> str:
        # 得到相对于开始时间的时间差
        time.sleep(5)
        runtime = datetime.datetime.strptime(self.getRunningTime(), "%Y-%m-%d %H:%M:%S.%f")
        starttime = datetime.datetime.strptime(self.startTime, "%Y-%m-%d %H:%M:%S.%f")
        diffTime = str(runtime - starttime)
        return "程序运行时间为 " + diffTime


class PrintLog(GetTimeInfo):
    """
    打印Log信息类
    """

    def __init__(self):
        # 记录什么时间开始运行
        super().__init__()  # 不加 提示警告， 加了 也没啥影响， 就是要 init 2次
        with open("Modules/Control/redme.txt", "a", encoding="utf-8") as f:
            f.writelines("In " + self.getDetailedTime() + " start Runing" + "\n")

    def MyprintDetailedTime(self, level: str):
        # 打印日志 -- 详细时间
        with open("Modules/Control/redme.txt", "a", encoding="utf-8") as f:
            f.writelines("level:" + level + " In DetailedTime " + self.getDetailedTime() + " start Runing" + "\n")

    def MyprintRunTime(self, level: str):
        # 打印日志 -- 运行时间
        with open("Modules/Control/redme.txt", "a", encoding="utf-8") as f:
            f.writelines("level:" + level + " In RunningTime " + self.getRunningTime() + " start Runing" + "\n")

    def MyprintLogWarnings(self, timeyypes: str):
        # 打印日志信息 -- 级别警告
        if timeyypes == "DetailedTime":
            self.MyprintDetailedTime("Warning")
        elif timeyypes == "RunningTime":
            self.MyprintRunTime("Warning")

    def MyprintLogInfos(self, timeyypes: str):
        # 打印日志信息 -- 级别日常信息
        if timeyypes == "DetailedTime":
            self.MyprintDetailedTime("Info")
        elif timeyypes == "RunningTime":
            self.MyprintRunTime("Info")

    def MyprintLogErrors(self, timeyypes: str):
        # 打印日志信息 -- 级别错误异常
        if timeyypes == "DetailedTime":
            self.MyprintDetailedTime("Error")
        elif timeyypes == "RunningTime":
            self.MyprintRunTime("Error")

    def MyprintLogClient(self, client: tuple):
        # 打印日志信息 -- 级别错误异常
        with open("Modules/Control/redme.txt", "a", encoding="utf-8") as f:
            f.writelines("In DetailedTime " + self.getDetailedTime() + str(client)
                         + " Connection" + "\n")


class TcpServer(PrintLog):
    """
    TCP服务端
    """
    connPool = []  # 连接池 记录所有客户端信息，暂时考虑一个
    sendData = b''
    receData = b''

    def __init__(self) -> None:
        # 1. 创建Tcp服务，等待连接
        super().__init__()
        self.addr = None
        self.conn = None
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # 参数可以不加
        self.server.bind(('192.168.1.101', 10006))  # 绑定要监听的端口
        self.server.listen(5)  # 开始监听 表示可以使用五个链接排队

    def __del__(self) -> None:
        # 2. 关闭所有服务
        self.server.close()

    def getConnObj(self):
        # 3. 得到连接对象
        self.conn, self.addr = self.server.accept()  # 等待链接,多个链接的时候就会出现问题,其实返回了两个值
        self.connPool.append((self.conn, self.addr))  # 保存客户端地址和端口
        self.MyprintLogClient((self.conn, self.addr))  # 得到时间
        print(self.conn, self.addr)

    def sendMessages(self, data: bytes) -> bool:
        self.sendData = data

        return True


if __name__ == '__main__':
    time1 = GetTimeInfo()
    t2 = PrintLog()
    while True:
        t2.MyprintLogErrors("DetailedTime")
        print(time1.getTimeDiff())
        t2.MyprintLogErrors("RunningTime")

# 备用
# server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # 参数可以不加
# server.bind(('192.168.1.101', 10006))  # 绑定要监听的端口
# server.listen(5)  # 开始监听 表示可以使用五个链接排队
# conn, addr = server.accept()  # 等待链接,多个链接的时候就会出现问题,其实返回了两个值
# print('Connected Addresss : ', addr)
#
# while True:
#     data = conn.recv(1024)  # 接收数据
#     if data:
#         print('Client name: ', data.decode())  # 打印接收到的数据
#         conn.send(data)  # 要用持有这个客户端的连接去发生消息，所以不能用server
