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

serverData = b''
flag = 0


def img_rece(serverData, flag, udpClient):
    print("开启线程1")
    while True:
        if flag == 0:
            print("1111")
            serverData, serverAddr = udpClient.recvfrom(921600)  # 接收来自服务端的数据
            print("收了一次")
            flag = 1


def img_show(serverData, flag):
    print("开启线程2")
    js = 0
    while True:
        if flag == 1:
            print("第二次")
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
            flag = 0


if __name__ == "__main__":
    data = input("输入要的内容")
    udpClient.sendto(data.encode(), serverAddr)  # 服务器连接完成
    # 这块开始开线程， 进行图像加速接受处理
    process_list = []
    p1 = Process(target=img_rece, args=(serverData, flag, udpClient))
    p2 = Process(target=img_show, args=(serverData, flag))
    p1.start()
    p2.start()
    process_list.append(p1)
    process_list.append(p2)
    p1.join()  # 阻塞式加入
    p2.join()
    udpClient.close()
    print("任务结束")
