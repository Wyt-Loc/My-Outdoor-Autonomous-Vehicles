# -*- coding: utf-8 -*-
# @Time    : 2023/6/28 17:35
# @Author  : Wyt
# @File    : tcptest.py
import datetime
import socket
import threading


class TcpServer:
    """
    TCP服务端
    """
    tcpserver = []  # tcp连接池
    validationinformation = 'IS IT UART2ETH ?'  # 校验信息
    receData = b""
    sendData = b""
    flag = 0

    def __init__(self):
        # 创建监听 建立一个服务端
        self.addr = None
        self.conn = None
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # 参数可以不加
        self.server.bind(('192.168.1.101', 10006))  # 绑定要监听的端口
        self.server.listen(5)  # 开始监听 表示可以使用五个链接排队

    def __del__(self):
        # 销毁所有TCP连接
        self.server.close()  # 断开连接

    def receTcp(self):
        # 接收Tcp连接
        self.conn, self.addr = self.server.accept()  # 等待链接,多个链接的时候就会出现问题,其实返回了两个值
        self.tcpserver.append((self.conn, self.addr))  # 保存客户端地址和端口
        print(self.conn, self.addr)

    def Myprint(self):
        # 保存数据
        print("接收到" + self.addr[0] + "的数据：" + self.receData.decode())

        curtime = str(datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f'))
        with open("data.txt", 'a', encoding="utf-8") as f:
            f.writelines(
                "In " + curtime + " received from " + self.addr[0]
                + " the news: " + self.receData.decode() + "\n")

    def showdata(self):
        # 显示和保存
        print("开始接收数据")
        while True:
            self.receData = self.conn.recv(1024)
            if self.receData:
                self.Myprint()
                self.flag = 1

    def replyData(self, data: bytes):
        # 发送数据到客户端
        while True:
            if self.flag == 1:
                self.conn.send(data)
                self.flag = 0


def aaa():
    b = str(12344)
    return b.encode()


def MyTcpControl():
    t = TcpServer()
    t.receTcp()
    t1 = threading.Thread(target=t.showdata)
    t1.start()
    t2 = threading.Thread(target=t.replyData, args=(aaa(),))
    t2.start()


if __name__ == '__main__':
    MyTcpControl()

    #  备用
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
