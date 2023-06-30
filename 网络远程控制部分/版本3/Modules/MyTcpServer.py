# -*- coding: utf-8 -*-
# @Time    : 2023/6/30 12:20
# @Author  : Wyt
# @File    : MyTcpServer.py

import socket
import MyprintLog


class TcpServer(MyprintLog.PrintLog):
    """
    TCP服务端
    """
    connPool = []  # 连接池 记录所有客户端信息，暂时考虑一个
    sendData = b''
    receData = b''
    addr = None
    conn = None
    flag = 0

    def __init__(self) -> None:
        # 1. 创建Tcp服务，等待连接
        super().__init__()
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # 参数可以不加
        self.server.bind(('192.168.1.101', 10006))  # 绑定要监听的端口
        self.server.listen(5)  # 开始监听 表示可以使用五个链接排队

    def __del__(self) -> None:
        # 2. 关闭所有服务
        self.server.close()

    def getConnObj(self) -> bool:
        # 3. 得到连接对象
        self.conn, self.addr = self.server.accept()  # 等待链接,多个链接的时候就会出现问题,其实返回了两个值
        self.connPool.append((self.conn, self.addr))  # 保存客户端地址和端口
        self.MyprintLogClient((self.conn, self.addr))  # 得到时间
        print(self.conn, self.addr)
        print("等待连接")
        if len(self.connPool) >= 1:
            return True
        else:
            return False

    def receMessages(self):
        # 循环接收TCP客户端数据
        while True:
            while self.flag == 0:
                self.receData = self.conn.recv(1024)
                self.printReceData(self.receData.decode(), (self.conn, self.addr))
                self.flag = 1

    def sendMessages(self, data: str):
        while True:
            while self.flag == 1:
                # 向客户端发送data
                self.sendData = data
                self.conn.send(data.encode())  # 发送消息
                self.printSendData(self.sendData, (self.conn, self.addr))
                self.flag = 0
