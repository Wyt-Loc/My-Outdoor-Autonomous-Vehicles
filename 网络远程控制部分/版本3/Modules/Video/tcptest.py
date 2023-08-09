# -*- coding: utf-8 -*-
# @Time    : 2023/5/24 16:03
# @Author  : Wyt
# @File    : tcptest.py
import socket
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
        except ValueError:
            print("tcp链接出现错误")

        # 4. 关闭套接字
        # tcp_socket.close()


# 数据格式规定

# WASD 长按事件，   上下左右点按事件

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
# 按键处理方面  上下控制步进电机， 左右控制舵机 加速暂时不考虑
# 主要就是按键的模式 点按 还是 长按， 这个效果在接收端不好处理，所以转到发送端处理
# 如果是长按，则在原来的基础上加标记  “long” 标记这个命令是长按，在松手的时候我们有一个捕获过程，捕捉到之后
# 在此发送一个对应的松手命令，来结束Stm32执行的动作.

def gos():
    global send_data
    send_data = 'advancess\r\n'
    print("前进")


def backs():
    global send_data
    send_data = 'backs\r\n'
    print("后退")


def lefts():
    global send_data
    send_data = 'lefts\r\n'
    print("左转")


def rights():
    global send_data
    send_data = 'rights\r\n'
    print("右转")


def gol():
    global send_data
    send_data = 'advancesl\r\n'
    print("前进长")


def backl():
    global send_data
    send_data = 'backl\r\n'
    print("后退长")


def leftl():
    global send_data
    send_data = 'leftl\r\n'
    print("左转长")


def rightl():
    global send_data
    send_data = 'rightl\r\n'
    print("右转长按")


def stopServos():
    global send_data
    send_data = 'stopservos\r\n'
    print("舵机停止")


def stopMotor():
    global send_data
    send_data = 'stopmotor\r\n'
    print('电机停止')


WIDTH = 500
HEIGHT = 500


def key_Con():
    global send_data, send_flag

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
                    gos()
                    send_flag = 1
                elif event.key == pygame.K_DOWN:
                    backs()
                    send_flag = 1
                elif event.key == pygame.K_LEFT:
                    lefts()
                    send_flag = 1
                elif event.key == pygame.K_RIGHT:
                    rights()
                    send_flag = 1
                elif event.key == pygame.K_w:
                    gol()
                    send_flag = 1
                elif event.key == pygame.K_s:
                    backl()
                    send_flag = 1
                elif event.key == pygame.K_a:
                    leftl()
                    send_flag = 1
                elif event.key == pygame.K_d:
                    rightl()
                    send_flag = 1
                elif event.key == pygame.K_1:
                    stopServos()
                    send_flag = 1
                elif event.key == pygame.K_2:
                    stopMotor()
                    send_flag = 1


# 图像部分     先打开 服务器 再打开树莓派 再打开PC
# TCP部分     先打开PC 再打开树莓派


if __name__ == '__main__':
    t1 = threading.Thread(target=TcpControl)
    t2 = threading.Thread(target=key_Con)
    t1.start()
    t2.start()
    t1.join()
    t2.join()
