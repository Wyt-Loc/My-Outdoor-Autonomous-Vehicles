import socket
import cv2
import time

cap = cv2.VideoCapture(0)


# 设置镜头分辨率，默认是640x480
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 320)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 240)


udpClient = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # 创建socket对象，走udp通道
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
    data = input("发送的数据为:")
    udpClient.sendto(data.encode(), serverAddr)
    flag += 1


# 开始视频传输
while True:
    data = input("输入开始标记")
    udpClient.sendto(data.encode(), serverAddr) # 发送数据给服务端
    if int(data) == 999:
        break
    
while True:

    _, img = cap.read()
    img = cv2.flip(img, 1)

    # 压缩图片
    _, send_data = cv2.imencode('.jpg', img, [cv2.IMWRITE_JPEG_QUALITY, 50])
    
    udpClient.sendto(send_data, serverAddr)
    print(f'正在发送数据，大小:{img.size} Byte')

    
udpClient.close()
cv2.destroyAllWindows()



