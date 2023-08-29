import socket
import threading

js = 0

udpServer = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # 创建socket对象，走udp通道
host = socket.gethostname() # 获取本地主机名
port = 8888
serverAddr = (host, port)
udpServer.bind(serverAddr) # 绑定服务端地址


SEND_BUF_SIZE = 921600  # 设置接收缓冲域大小

RECV_BUF_SIZE = 921600  # 设置发送缓冲域套接字关联的选项

udpServer.setsockopt(
    socket.SOL_SOCKET,
    socket.SO_SNDBUF,
    SEND_BUF_SIZE)

# 设置接收缓冲域套接字关联的选项
udpServer.setsockopt(
    socket.SOL_SOCKET,
    socket.SO_RCVBUF,
    RECV_BUF_SIZE)


clientlist = [] # 存储客户端的ip地址    规定第一个接收到的时树莓派的地址 第二个时win的地址
# 那么， 我们只接收第一个地址的数据 然后发送给第二个地址

xintiaolist = [] #这个我们仍然只保留2个地址和端口的元组信息
# 每经过3秒我们检测一次这个端口和我们clientlist的端口内容是否一样，
# 如果不一样，那么我们将这个新的端口重新定义为新的目标端口


# 是否要转发的标志位
issend = 0

flag = 0
js = 0


# 接收视频数据的线程
# 此线程只处理视频的接收和转发
def img_recv():
    global clientlist, flag, issend,js
    while flag < 2: # 前两次通信用来存储 两个客户端的IP地址 用来穿透使用
        print("开启进程1")
        clientData,clientAddr = udpServer.recvfrom(1024) # 接收来自客户端的数据
        clientlist.append(clientAddr)
        udpServer.sendto('wyt'.encode(), clientlist[flag]) # 发送数据给客户端
        print(clientAddr)
        flag += 1
        if flag == 2:
            print("第一个")
            print(clientlist)
            flag += 1

    while True:
            clientData,clientAddr = udpServer.recvfrom(921600) # 接收来自客户端的数据
            udpServer.sendto(clientData, clientlist[1]) # 发送数据给客户端
            js += 1
            if not js % 100:
                print(str(js)+"sucess")


# 由于现在的问题是一段时间后，我们就接收不到服务器的图像数据了
# 猜测是由于图像接收的端口变了。 检测长时间没有图片发送过去，那么客户端方，向这边发送心跳，请求重新连接
# 接收到心跳后我们更新这个地址和端口，重新进行发送。
# 难点在于如何筛选出，这个心跳信号来。
# 接收心跳   这个心跳只有在断线时才会发送过来，通过筛选地址的方式来区分这个两个信息，是图片还是心跳。
# 为了减小服务器压力， 我们每3秒接收一次, 相对于每3秒我们抽一帧进行是否断开的判断
# 破案： 断线是由于UDP协议自身的问题，UDP规定120秒内没有数据传输，则自动断开连接。
# 在我们看来就是固定在 120秒 左右断开，所以我们只需在客户端每隔120秒内向服务器发送一个心跳包来维持这个连接即可。
def recv_xintiao():
    print("jinlaile")
    global clientlist,js,issend
    while True:
        while issend == 1:
            data, ip = udpServer.recvfrom(921600)
            if ip == clientlist[0] or ip == clientlist[1]: # 如果IP和树莓派一样 那么直接舍弃了
                js = 0
                issend = 0
                print("zoude1")
                pass
            else:
                issend = 1
                clientlist[1] = ip # 将新的IP地址赋值给目标地址
                issend = 0
                js=0
                issend = 0
                print(zoude2)


# 线程启动函数
def main_sxt():
    t1 = threading.Thread(target=img_recv) # 开启数据转发
    #t2 = threading.Thread(target=recv_xintiao) # 成功解决这个问题，不需要了
    t1.start()
    t1.join()
    #t2.start()


#if __name__ == "__main__":
 #   main1()


# 单个线程 处理图像转发压力太大，采用多线程来分别实现收和发，收的数据存放在一个公共区域中，
# 不停的去发送数据
#while True:

 #   while flag < 2: # 前两次通信用来存储 两个客户端的IP地址 用来穿透使用 
  #      clientData,clientAddr = udpServer.recvfrom(1024) # 接收来自客户端的数据
   #     clientlist.append(clientAddr)
    #    udpServer.sendto('wyt'.encode(), clientlist[flag]) # 发送数据给客户端
     #   print(clientAddr)
      #  flag += 1
       # if flag == 2:
        #    print("第一个")
         #   print(clientlist)
#            flag += 1
    
 #   clientData,clientAddr = udpServer.recvfrom(460800) # 接收来自客户端的数据
  #  udpServer.sendto(clientData, clientlist[1]) # 发送数据给客户端
   # js += 1
    #print(str(js)+"sucess")








