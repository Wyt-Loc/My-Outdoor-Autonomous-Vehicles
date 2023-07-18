# -*- coding: utf-8 -*-
# @Time    : 2023/7/3 21:17
# @Author  : Wyt
# @File    : MyControl.py
import time
import MyTcpServer
import keyboard


class Mykey(MyTcpServer.TcpServer):
    upd = keyboard.KeyboardEvent('upd', 28, 'up')
    downd = keyboard.KeyboardEvent('downd', 29, 'down')
    leftd = keyboard.KeyboardEvent('leftd', 30, 'left')
    rightd = keyboard.KeyboardEvent('rightd', 31, 'right')

    # 数据格式 电机走的方向角度距离 + 舵机方向
    # 帧头
    head = "0xAAAAx0"  # 8
    # 功能帧1
    fun1 = "m"  # 电机参数  #  9
    # 数据帧1
    a = "001230456"  # 对应方向  距离  速度  # 18
    # 功能帧2
    fun2 = "s"  # 舵机参数  # 19
    # 数据帧2
    s = "3000"  # 对应 方向 # 23
    # CRC校验位 高位 和 低位
    height = "gao"  # 3
    low = "di"  # 2

    Medianvalue = 6000  # 舵机中值

    def dataFrame(self):
        # 数据帧拼接
        return self.head + self.fun1 + self.a + self.fun2 + self.s

    def dataGo(self):
        #  前进
        return self.head + self.fun1 + "100300100"  # 方向 向前 速度0.3m/s  距离1m

    def dataBack(self):
        #  后退
        return self.head + self.fun1 + "000300100"  # 方向 向前 速度0.3m/s  距离1m

    def dataLeft(self):
        #  左转
        if self.Medianvalue < 12000:
            return self.head + self.fun2 + str(self.Medianvalue + 100)  # 舵机角度+1°

    def dataRight(self):
        #  右转
        if self.Medianvalue > 0:
            return self.head + self.fun2 + str(self.Medianvalue - 100)  # 舵机角度-1°

    # 共 8 + 8 + 5 + 10 = 31个字节
    def keyfun(self, keyval):
        # 执行键值对应的功能
        if keyval.event_type == 'down' and keyval.name == self.upd.name:
            print("短按up键")
            while True:
                self.sendMessages(self.dataGo())  # 发送数据帧
                if self.receMessages():
                    break
                time.sleep(1)

        if keyval.event_type == 'down' and keyval.name == self.downd.name:
            print("短按down键")
            while True:
                self.sendMessages(self.dataBack())  # 发送数据帧
                if self.receMessages():
                    break
                time.sleep(1)

        if keyval.event_type == 'down' and keyval.name == self.leftd.name:
            print("短按left键")
            while True:
                self.sendMessages(self.dataLeft())  # 发送数据帧
                if self.receMessages():
                    break
                time.sleep(1)

        if keyval.event_type == 'down' and keyval.name == self.rightd.name:
            print("短按right键")
            while True:
                self.sendMessages(self.dataRight())  # 发送数据帧
                if self.receMessages():
                    break
                time.sleep(1)

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
