import socket
import numpy as np
import cv2
import threading 


udpClient = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # 创建socket对象，走udp通道
host = '8.130.115.65'
port = 8888
serverAddr = (host, port)

SEND_BUF_SIZE = 921600*2  # 设置接收缓冲域大小

RECV_BUF_SIZE = 921600*2  # 设置发送缓冲域套接字关联的选项

udpClient.setsockopt(
    socket.SOL_SOCKET,
    socket.SO_SNDBUF,
    SEND_BUF_SIZE)

# 设置接收缓冲域套接字关联的选项
udpClient.setsockopt(
    socket.SOL_SOCKET,
    socket.SO_RCVBUF,
    RECV_BUF_SIZE)



#udpClient.setblocking(False)  # 设置为非阻塞模式


flag = 0
js = 0


def img_recv():
    global flag,js,serverAddr
    while True:
        if flag == 0:
            data = input("输入要的内容")
            udpClient.sendto(data.encode(), serverAddr)  # 发送数据给服务端 打通连接
            flag = 1
        
        serverData, serverAddr = udpClient.recvfrom(921600*2)  # 接收来自服务端的数据
         
        js += 1
        print("第"+str(js)+"帧图像")
        
        if js >= 3:
            try:
                receive_data = np.frombuffer(serverData, dtype='uint8')
                r_img = cv2.imdecode(receive_data, 1)
                cv2.putText(r_img, "server", (50, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 0), 2)
                print(4)
                r_img = cv2.imdecode(receive_data, 1)
                cv2.imshow('server', r_img)
                key = cv2.waitKey(1)  # 致命伤
                if key == ord("q"):
                    break
            except:
                print("这儿错了")


# 思考如何使用多线程进行数据处理加速
# 线程开启函数
def main1():
    t1 = threading.Thread(target=img_recv) # 接受图像
    t1.start()
    
if __name__ == "__main__":
    main1()


#udpClient.close()


