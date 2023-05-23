import socket
import numpy as np
import cv2
from multiprocessing import Process


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

while True:
    if flag == 0:
        flag = 1
        data = input("输入要的内容")
        udpClient.sendto(data.encode(), serverAddr)  # 发送数据给服务端

    serverData, serverAddr = udpClient.recvfrom(921600)  # 接收来自服务端的数据

    if js >= 10:
        receive_data = np.frombuffer(serverData, dtype='uint8')
        r_img = cv2.imdecode(receive_data, 1)
        cv2.putText(r_img, "server", (50, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 0), 2)
        r_img = cv2.imdecode(receive_data, 1)
        cv2.imshow('server', r_img)
        key = cv2.waitKey(1)  # 致命伤
        if key == ord("q"):
            break

    js += 1
    print("第" + str(js) + "帧图像")

udpClient.close()
