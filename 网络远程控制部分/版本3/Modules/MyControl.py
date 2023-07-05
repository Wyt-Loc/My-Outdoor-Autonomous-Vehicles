# -*- coding: utf-8 -*-
# @Time    : 2023/7/3 21:17
# @Author  : Wyt
# @File    : MyControl.py
import time
import MyTcpServer
import keyboard


class Mykey(MyTcpServer.TcpServer):
    upd = keyboard.KeyboardEvent('down', 28, 'up')
    downd = keyboard.KeyboardEvent('down', 29, 'down')
    leftd = keyboard.KeyboardEvent('down', 30, 'left')
    rightd = keyboard.KeyboardEvent('down', 31, 'right')
    upu = keyboard.KeyboardEvent('up', 32, 'up')
    downu = keyboard.KeyboardEvent('up', 33, 'down')
    leftu = keyboard.KeyboardEvent('up', 34, 'left')
    rightu = keyboard.KeyboardEvent('up', 35, 'right')

    # 数据格式 电机走的方向角度距离 + 舵机方向
    # 帧头
    head = "0xAAAAx0"  # 8
    # 功能帧1
    fun1 = "m"  # 电机参数  # 13
    # 数据帧1
    a = "013141314"  # 对应方向  距离  速度  # 9
    # 功能帧2
    fun2 = "s"  # 舵机参数  # 1
    # 数据帧2
    s = "3000"  # 对应 方向 # 4
    # CRC校验位 高位 和 低位
    height = "gao"  # 3
    low = "di"  # 2

    def dataFrame(self):
        # 数据帧拼接
        return self.head + self.fun1 + self.a + self.fun2 + self.s + self.height + self.low

    # 共 8 + 8 + 5 + 10 = 31个字节
    def keyfun(self, keyval):
        # 执行键值对应的功能
        # 判断按键是否为同一个，是同一个那么不进行时间的重置，左右一组，上下一组，这两组互不干扰
        if keyval.event_type == 'down' and keyval.name == self.upd.name:
            print("短按up键")
        if keyval.event_type == 'down' and keyval.name == self.downd.name:
            print("短按down键")
        if keyval.event_type == 'down' and keyval.name == self.leftd.name:
            print("短按left键")
        if keyval.event_type == 'down' and keyval.name == self.rightd.name:
            print("短按right键")
            self.sendMessages(self.dataFrame())  # 发送数据帧
            while True:
                if self.receMessages():
                    break

    def getKeyValue(self):
        # 得到键值执行对应的函数
        keyboard.hook(self.keyfun)
        keyboard.wait()


if __name__ == '__main__':
    mykey = Mykey()
    # 直到有一个客户端连接  暂不考虑多用户情况
    while True:
        if mykey.getConnObj():
            break
    mykey.getKeyValue()
