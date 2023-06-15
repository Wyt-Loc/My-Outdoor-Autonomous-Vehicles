import socket

udpClient = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # 创建socket对象，走udp通道
port = 8888
host = '8.130.115.65'
serverAddr = (host, port)
while True:

    data = input("发送的数据为：")
    udpClient.sendto(data.encode(), serverAddr) # 发送数据给服务端
    serverData,serverAddr = udpClient.recvfrom(1024) # 接收来自服务端的数据
    print(serverData.decode('utf-8'), serverAddr)
udpClient.close()
    

