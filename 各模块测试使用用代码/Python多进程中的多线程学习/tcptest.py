# -*- coding: utf-8 -*-
# @Time    : 2023/5/24 16:03
# @Author  : Wyt
# @File    : tcptest.py


# 图像部分     先打开 服务器 再打开树莓派 再打开PC
# TCP部分      先打开PC 再打开树莓派


import socket
import time

# 1.创建socket
tcp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# 2. 链接服务器
server_addr = ("8.130.115.65", 8887)
tcp_socket.connect(server_addr)


# 如何使用键盘控制运动，这块我们选择了 2 种控制方式，点按和长按，点按的话
# 就是按一下走一定的距离，长按就是一直走，知道检测到按键松开了，这是停止

def TcpControl():
    flag = 0
    while True:
        if flag == 0:
            # 3. 发送数据
            send_data = 'pcpc'  # input("请输入要发送的数据：")
            try:
                tcp_socket.send(send_data.encode("gbk"))
            except:
                print("tcp链接出现错误")
            flag = 1

        send_data = input("输入要发送的数据：")
        try:
            tcp_socket.send(send_data.encode("gbk"))
        except:
            print("tcp链接出现错误")

        # 4. 关闭套接字
        # tcp_socket.close()


TcpControl()
