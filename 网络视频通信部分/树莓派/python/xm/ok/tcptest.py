import socket
import usart_send
import threading

# 1.创建socket
tcp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# 2. 链接服务器
server_addr = ("8.130.115.65", 8887)
tcp_socket.connect(server_addr)

def tcp_send():

    while True:
        # 3. 发送数据
        flag = 1
        if flag == 1:
            send_data =  "smpsmp"  # input("请输入要发送的数据：")
            tcp_socket.send(send_data.encode("utf-8"))
            flag = 0
        info = tcp_socket.recv(1024)
        if info:
            print(info.decode('gbk'))

        # 4. 关闭套接字
        # tcp_socket.close()
    
