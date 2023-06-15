import socket
import cv2
import time
import threading

cap = cv2.VideoCapture(0)


# 设置镜头分辨率，默认是640x480
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)


udpClient = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # 创建socket对象，走udp通道
port = 8888
host = '8.130.115.65'
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


info = 'smp'


aa = []

while True:
    udpClient.sendto(info.encode(),serverAddr)
    data,address = udpClient.recvfrom(1024)
    if data != "":
        aa.append(data.decode())
        print("1111")
        if len(aa) == 2:
            break


aa[1] = int(aa[1])

print(aa)



def img_send():
    global t1flag
    while  True:
        global serverAddr
        _, img = cap.read()
        img = cv2.flip(img, 1)

        # 压缩图片
        _, send_data = cv2.imencode('.jpg', img, [cv2.IMWRITE_JPEG_QUALITY, 50])
    
        udpClient.sendto(send_data, tuple(aa))
        print(f'正在发送数据，大小:{img.size} Byte')


# 开始视频传输
def main1():
    data = input("输入开始标记")
    udpClient.sendto(data.encode(), tuple(aa)) # 发送数据给服务端
    if int(data) == 99:
        t1 = threading.Thread(target=img_send) # 图片发送
 #       t2 = threading.Thread(target=img_stop) # 图片停止发送
        t1.start()
#        t2.start()


if __name__ == '__main__':
    main1()

   

