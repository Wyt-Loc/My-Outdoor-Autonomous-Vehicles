import socket
import time
from multiprocessing import Process
import numpy as np
import cv2
import threading

# import tcptest

# 图像部分     先打开 服务器 再打开树莓派 再打开PC
# TCP部分      先打开PC 再打开树莓派

# UDP创建

udpClient = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # 创建socket对象，走udp通道
host = '8.130.115.65'
port = 8888
serverAddr = (host, port)

SEND_BUF_SIZE = 921600  # 设置接收缓冲域大小

RECV_BUF_SIZE = 921600  # 设置发送缓冲域套接字关联的选项

udpClient.setsockopt(
    socket.SOL_SOCKET,
    socket.SO_SNDBUF,
    SEND_BUF_SIZE)

# 设置接收缓冲域套接字关联的选项
udpClient.setsockopt(
    socket.SOL_SOCKET,
    socket.SO_RCVBUF,
    RECV_BUF_SIZE)

# udpClient.setblocking(False)  # 设置为非阻塞模式

flag = 0
js = 0
js_old = 0
isconn = 0


def img_recv():
    global flag, js, serverAddr, isconn, num
    while True:
        if flag == 0:
            # data = input("输入要的内容")
            data = 'pcpc'
            udpClient.sendto(str(data).encode(), serverAddr)  # 发送数据给服务端 打通连接
            flag = 1

        serverData, serverAddr = udpClient.recvfrom(921600)  # 接收来自服务端的数据

        js += 1
        # js += 1
        # print("第" + str(js) + "帧图像")
        if js >= 3:
            try:
                receive_data = np.frombuffer(serverData, dtype='uint8')
                frame = cv2.imdecode(receive_data, 1)
                cv2.imshow('server', frame)
                key = cv2.waitKey(1)  # 致命伤
                if key == ord("q"):
                    break
            except:
                pass


# 解决断线的办法， 当长时间接收不到图像时，我们采用重新建立udp连接的方法来解决
# 具体做法： 1. 我们让客户端重新发一次数据，让服务器来接收这个数据 并提取其中的地址信息
#            2. 提取到以后，和之前保存的地址信息进行比对，如果不一样说明确实是这个问题，把地址信息更新
# 最终发现是由于UDP协议自身的特性 ，2分钟没有消息自动断开，所以我们在2分钟以内要发送一次心跳来维持连接
def conn_sent():
    while True:
        time.sleep(10)
        udpClient.sendto("1".encode(), serverAddr)


# 思考如何使用多线程进行数据处理加速
# 线程开启函数
def main1():
    global js
    t1 = threading.Thread(target=img_recv)  # 接受图像
    t1.start()
    p1 = Process(target=conn_sent)
    # p2 = Process(target=tcptest.TcpControl)
    p1.start()
    # p2.start()


if __name__ == "__main__":
    main1()

    # udpClient.close()
