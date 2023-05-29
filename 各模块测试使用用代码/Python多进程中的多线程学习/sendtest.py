import socket

s = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

while True:
    data = input("发送的内容:")
    s.sendto(data.encode(),('8.130.115.65',8888))

