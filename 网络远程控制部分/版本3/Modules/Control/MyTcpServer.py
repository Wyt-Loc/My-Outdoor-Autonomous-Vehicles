# -*- coding: utf-8 -*-
# @Time    : 2023/6/30 12:20
# @Author  : Wyt
# @File    : MyTcpServer.py

import socket
import Modules.Control.MyprintLog


class TcpServer(Modules.Control.MyprintLog.PrintLog):
    """
    TCP服务端
    """
    connPool = []  # 连接池 记录所有客户端信息，暂时考虑一个
    sendData = b''
    receData = b''
    addr = None
    conn = None

    us_flag = 0
    info = b''
    flag = 1
    debug_dpj = 1  # 单片机调试模式 0   整体调试 1

    def __init__(self) -> None:
        # 1. 创建Tcp服务，等待连接
        super().__init__()
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # 参数可以不加
        self.server.bind(('192.168.1.104', 10006))  # 绑定要监听的端口  此IP为 串口转以太网接口IP
        self.server.listen(5)  # 开始监听 表示可以使用五个链接排队

        if self.debug_dpj == 1:
            # 1.创建socket
            self.tcp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            # 2. 链接服务器
            server_addr = ("8.130.115.65", 8887)
            self.tcp_socket.connect(server_addr)

    def __del__(self) -> None:
        # 2. 关闭所有服务
        self.server.close()

    def tcpConnSendFlag(self):
        # 3. 发送连接数据
        send_data = "smpsmp"  # 发送标志信息
        self.tcp_socket.send(send_data.encode("utf-8"))

    def tcpReceData(self):
        self.info = self.tcp_socket.recv(1024)
        if self.info:
            print("得到数据", self.info.decode('gbk'))

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
        self.receData = self.conn.recv(1024)
        self.printReceData(self.receData.decode(), str(self.addr))
        if self.receData.decode() == "ok":
            return 1

    def sendMessages(self, data: str):
        # 向客户端发送data
        self.sendData = data
        self.conn.send(data.encode())  # 发送消息
        self.printSendData(self.sendData, str(self.addr))

# if __name__ == '__main__':
#     mytcp = TcpServer()
#     mytcp.tcpConnSendFlag()
#     mytcp.tcpReceData()
