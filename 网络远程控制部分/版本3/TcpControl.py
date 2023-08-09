# -*- coding: utf-8 -*-
# @Time    : 2023/6/28 10:38
# @Author  : Wyt
# @File    : TcpControl.py
import socket
import threading

us_flag = 0
info = b''


def main_tcp():
    global us_flag, info
    flag = 1
    # 1.创建socket
    tcp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # 2. 链接服务器
    server_addr = ("8.130.115.65", 8887)
    tcp_socket.connect(server_addr)

    print("开启")
    while True:
        # 3. 发送数据
        if flag == 1:
            send_data = "smpsmp"  # input("请输入要发送的数据：")
            tcp_socket.send(send_data.encode("utf-8"))
            flag = 0

        info = tcp_socket.recv(1024)
        try:
            if info:
                print(info.decode('gbk'))
                with open("data.txt", 'a') as f:
                    f.write(info.decode("gbk"))
                # tcp_socket.close()
        except IOError:
            pass


def main_usart():
    thing2 = threading.Thread(target=main_tcp)
    thing2.start()
    thing2.join()


# # 函数入口
# if __name__ == '__main__':
#     main_usart()
