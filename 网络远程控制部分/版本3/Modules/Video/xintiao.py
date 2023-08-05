import socket
import cv2
import time
import threading

t1flag = 0
t2flag = 0

cap = cv2.VideoCapture(1, cv2.CAP_DSHOW)

# 设置镜头分辨率，默认是640x480
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 320)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 320)

udpClient = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # 创建socket对象，走udp通道
port = 8888
host = '8.130.115.65'
serverAddr = (host, port)

flag = 0

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

while flag == 0:
    data = 'smpsmp'
    udpClient.sendto(data.encode(), serverAddr)
    flag += 1

js = 0


def img_send():
    global t1flag, js
    while t1flag == 0:
        time.sleep(0.08)
        global serverAddr
        _, img = cap.read()
        img = cv2.flip(img, 1)
        # 压缩图片
        _, send_data = cv2.imencode('.jpg', img, [cv2.IMWRITE_JPEG_QUALITY, 50])
        data = send_data[0:230400]
        udpClient.sendto(data, serverAddr)
        # data1 = send_data[]
        # udpClient.sendto(send_data, serverAddr)
        js += 1
        if not js % 100:
            print(str(js), "sucess")


def img_stop():
    global t2flag, t1flag
    while t2flag == 0:
        data = int(input("输入99结束传输"))
        if data == 99:
            t1flag = 1
            t2flag = 1
            break


# 断线重连，防止视频长时间卡死 发送心跳，表示客户端还是存活状态
def img_reconn():
    global serverAddr
    while True:
        time.sleep(3)
        udpClient.sendto("1reconn1".encode(), serverAddr)
        print("发送心跳成功")


# 开始视频传输
def main_sp():
    data = input("输入99开始传输视频")
    if data != None:

        udpClient.sendto(data.encode(), serverAddr)  # 发送数据给服务端
        if int(data) == 99:
            t1 = threading.Thread(target=img_send)  # 图片发送
            #        t3 = threading.Thread(target=img_reconn) # 段开后尝试重连
            t1.start()
        t2 = threading.Thread(target=img_stop)  # 图片停止发送
        t2.start()


#        t3.start()


if __name__ == '__main__':
    main_sp()

    # udpClient.close()
    # cv2.destroyAllWindows()
