# -*- coding: utf-8 -*-
# @Time    : 2023/5/24 16:03
# @Author  : Wyt
# @File    : tcptest.py
import socket
import time
import pygame
import threading

send_data = ""
send_flag = 0


def TcpControl():
    global send_data, send_flag

    # 1.创建socket
    tcp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # 2. 链接服务器
    server_addr = ("8.130.115.65", 8887)
    tcp_socket.connect(server_addr)
    flag = 0

    while True:
        if flag == 0:
            # 3. 发送数据
            send_data = 'pcpc'  # input("请输入要发送的数据：")
            try:
                tcp_socket.send(send_data.encode("gbk"))
                send_data = ""
            except:
                print("tcp链接出现错误")
            flag = 1

        try:
            if send_data != "" and send_flag == 1:
                print(send_data)
                tcp_socket.send(send_data.encode("gbk"))
                send_data = ""
                send_flag = 0
        except:
            print("tcp链接出现错误")

        # 4. 关闭套接字
        # tcp_socket.close()


# 数据格式规定


# 数据格式规定：  帧头+校验位+数据+帧尾
#              帧头 0xff
#              校验位  为数据长度
#              数据 相应的数据
#              帧尾 0xaa
#              都按照 短按处理
#              前进 ———— 0xff + 2 + go + 0xaa
#              后退 ———— 0xff + 4 + back + 0xaa
#              左转 ———— 0xff + 4 + left + 0xaa
#              右转 ———— oxff + 5 + right + 0xaa


def go():
    global send_data
    send_data = 'go\r\n'
    print("1")


def back():
    global send_data
    send_data = 'back\r\n'
    print("2")


def left():
    global send_data
    send_data = 'left\r\n'
    print("3")


def right():
    global send_data
    send_data = 'right\r\n'
    print(4)


def key_Con():
    global send_data, send_flag

    WIDTH = 500
    HEIGHT = 500

    # 创建游戏窗口
    pygame.display.set_mode((WIDTH, HEIGHT))

    # 游戏循环
    while True:
        # 处理事件
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                exit()
            elif event.type == pygame.KEYDOWN and send_data == "" and send_flag == 0:
                if event.key == pygame.K_UP:
                    go()
                    send_flag = 1
                elif event.key == pygame.K_DOWN:
                    back()
                    send_flag = 1
                elif event.key == pygame.K_LEFT:
                    left()
                    send_flag = 1
                elif event.key == pygame.K_RIGHT:
                    right()
                    send_flag = 1


# 图像部分     先打开 服务器 再打开树莓派 再打开PC
# TCP部分      先打开PC 再打开树莓派


if __name__ == '__main__':
    t1 = threading.Thread(target=TcpControl)
    t2 = threading.Thread(target=key_Con)
    t1.start()
    t2.start()
    t1.join()
    t2.join()
