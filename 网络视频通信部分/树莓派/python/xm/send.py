from socket import *
from threading import *



addr = ('8.130.115.65', 8888)
s = socket(AF_INET, SOCK_DGRAM)  # 创建UDP套接字


while True:
    data = input("发送的内容")
    s.sendto(data.encode(), addr)
