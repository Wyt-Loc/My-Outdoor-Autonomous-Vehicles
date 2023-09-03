# -*- coding: utf-8 -*-
# @Time    : 2023/7/3 21:17
# @Author  : Wyt
# @File    : MyControl.py
import time
import 网络远程控制部分.版本3.Modules.Control.MyTcpServer
import keyboard


class Mykey(网络远程控制部分.版本3.Modules.Control.MyTcpServer.TcpServer):
    upd = keyboard.KeyboardEvent('upd', 28, 'up')
    downd = keyboard.KeyboardEvent('downd', 29, 'down')
    leftd = keyboard.KeyboardEvent('leftd', 30, 'left')
    rightd = keyboard.KeyboardEvent('rightd', 31, 'right')
    spaced = keyboard.KeyboardEvent('spaced', 32, 'space')
    w = keyboard.KeyboardEvent('w', 33, 'w')
    ss = keyboard.KeyboardEvent('s', 34, 's')
    # 数据格式 电机走的方向角度距离 + 舵机方向
    # 帧头
    head = 0xAA  # 1
    # 功能帧1
    fun1 = "m"  # 电机参数  #  2
    # 数据帧1
    a = "001230456"  # 对应方向  距离  速度  # 11
    # 功能帧2
    fun2 = "s"  # 舵机参数  # 12
    # 数据帧2
    s = "3000"  # 对应 方向 # 16
    # 功能帧3
    fun3 = "p"
    # 数据帧3
    stop = 'stop'
    # CRC校验位 高位 和 低位
    height = "gao"  #
    low = "di"  # 2
    # 帧尾
    tail = 0XBB  # 17

    Medianvalue = 10  # 舵机中值 范围 0 - 100   0° - 200°  每差0.5对应一个角度。 中值为100 左右各100各灵敏度。

    flag = 0
    bit = 0

    def isReceOk(self, data):
        while True:
            self.sendMessages(data)  # 发送数据帧
            if self.receMessages():
                break
            time.sleep(1)

    def dataFrame(self):
        # 数据帧拼接
        return str(self.head) + self.fun1 + self.a + self.fun2 + self.s

    def dataGo(self):
        #  前进
        return str(self.head) + self.fun1 + "108210932"  # 方向 向前 速度9.32m/s  距离8.21m

    def dataGol(self):
        #  长前进
        return str(self.head) + self.fun1 + "gol"  # 方向 向前 速度9.32m/s  距离8.21m

    def dataBack(self):
        return str(self.head) + self.fun1 + "008210932"  # 方向 向前 速度0.3m/s  距离1m

    def dataBackl(self):
        return str(self.head) + self.fun1 + "backl"  # 方向 向前 速度0.3m/s  距离1m

    def dataLeft(self):
        #  右转
        if self.Medianvalue <= 199:
            self.Medianvalue = self.Medianvalue + 1
        print(self.bit)
        if 0 <= self.Medianvalue < 10:
            return str(self.head) + self.fun2 + "00" + str(self.Medianvalue) + str(self.bit)  # 舵机角度+1°
        elif 10 <= self.Medianvalue <= 99:
            return str(self.head) + self.fun2 + "0" + str(self.Medianvalue) + str(self.bit)  # 舵机角度+1°
        else:
            return str(self.head) + self.fun2 + str(self.Medianvalue) + str(self.bit)  # 舵机角度+1°

    def dataRight(self):
        #  左转
        if self.Medianvalue >= 1:  # 对应角度计算为 5 / 1000 * 200
            self.Medianvalue = self.Medianvalue - 1
        print(self.bit)
        if 0 <= self.Medianvalue < 10:
            return str(self.head) + self.fun2 + "00" + str(self.Medianvalue) + str(self.bit)  # 舵机角度+1°
        elif 10 <= self.Medianvalue <= 99:
            return str(self.head) + self.fun2 + "0" + str(self.Medianvalue) + str(self.bit)  # 舵机角度+1°
        else:
            return str(self.head) + self.fun2 + str(self.Medianvalue) + str(self.bit)  # 舵机角度+1°

    def dataStop(self):
        print("发送急停信号")
        return str(self.head) + self.fun3 + "stop"  # 舵机角度-1°

    # 共 8 + 8 + 5 + 10 = 31个字节
    def keyfun(self, keyval):
        # 执行键值对应的功能
        if keyval.event_type == 'down' and keyval.name == self.upd.name:
            print("短按up键")
            self.sendMessages(self.dataGo())  # 发送数据帧
            if self.flag == 1:
                self.isReceOk(self.dataGo())

        if keyval.event_type == 'down' and keyval.name == self.w.name:
            print("短按W键1111")
            self.sendMessages(self.dataGol())  # 发送数据帧
            if self.flag == 1:
                self.isReceOk(self.dataGol())

        if keyval.event_type == 'down' and keyval.name == self.ss.name:
            print("短按S键1111")
            self.sendMessages(self.dataBackl())  # 发送数据帧
            if self.flag == 1:
                self.isReceOk(self.dataBackl())

        if keyval.event_type == 'down' and keyval.name == self.downd.name:
            print("短按down键")
            self.sendMessages(self.dataBack())  # 发送数据帧
            if self.flag == 1:
                self.isReceOk(self.dataBack())

        if keyval.event_type == 'down' and keyval.name == self.leftd.name:
            print("短按left键")
            self.sendMessages(self.dataLeft())  # 发送数据帧
            if self.flag == 1:
                self.isReceOk(self.dataLeft())

        if keyval.event_type == 'down' and keyval.name == self.rightd.name:
            print("短按right键")
            self.sendMessages(self.dataRight())  # 发送数据帧
            if self.flag == 1:
                self.isReceOk(self.dataRight())

        if keyval.event_type == 'down' and keyval.name == 'space':
            print("短按空格键")
            self.sendMessages(self.dataStop())  # 发送数据帧
            if self.flag == 1:
                self.isReceOk(self.dataStop())

    def getKeyValue(self):
        # 得到键值执行对应的函数
        keyboard.hook(self.keyfun)
        keyboard.wait()

    def dataToCommand(self):
        while True:
            self.tcpReceData()
            self.info = self.info.decode('gbk')

            # 接收到的信息转换为指令发送到单片机
            if self.info == "advancess":
                self.sendMessages(self.dataGo())  # 发送数据帧
                if self.flag == 1:
                    self.isReceOk(self.dataGo())
                print("执行{}短按命令".format(self.info))

            if self.info == "backs":
                self.sendMessages(self.dataBack())  # 发送数据帧
                if self.flag == 1:
                    self.isReceOk(self.dataBack())
                print("执行{}短按命令".format(self.info))

            if self.info == "lefts":
                self.sendMessages(self.dataLeft())  # 发送数据帧
                if self.flag == 1:
                    self.isReceOk(self.dataLeft())
                print("执行{}短按命令".format(self.info))

            if self.info == "rights":
                self.sendMessages(self.dataRight())  # 发送数据帧
                if self.flag == 1:
                    self.isReceOk(self.dataRight())
                print("执行{}短按命令".format(self.info))

            if self.info == "advancesl":
                self.sendMessages(self.dataGol())  # 发送数据帧
                if self.flag == 1:
                    self.isReceOk(self.dataGol())
                print("执行{}长按命令".format(self.info))

            if self.info == "backl":
                self.sendMessages(self.dataBackl())  # 发送数据帧
                if self.flag == 1:
                    self.isReceOk(self.dataBackl())
                print("执行{}长按命令".format(self.info))

            if self.info == "leftl":
                print("执行{}短按命令".format(self.info))
            if self.info == "rightl":
                print("执行{}短按命令".format(self.info))
            self.info = b''


def aaa():
    mykey = Mykey()
    if mykey.debug_dpj == 0:  # 单片机调试模式
        # 直到有一个客户端连接  暂不考虑多用户情况
        while True:
            if mykey.getConnObj():
                break
        mykey.getKeyValue()

    elif mykey.debug_dpj == 1:  # 整体测试
        # 建立连接
        mykey.tcpConnSendFlag()
        while True:
            if mykey.getConnObj():
                break
        # 接收数据 并 转发到单片机
        mykey.dataToCommand()


if __name__ == '__main__':
    mykey = Mykey()
    if mykey.debug_dpj == 0:  # 单片机调试模式
        # 直到有一个客户端连接  暂不考虑多用户情况
        while True:
            if mykey.getConnObj():
                break
        mykey.getKeyValue()

    elif mykey.debug_dpj == 1:  # 整体测试
        # 建立连接
        mykey.tcpConnSendFlag()
        while True:
            if mykey.getConnObj():
                break
            # 接收数据 并 转发到单片机
        mykey.dataToCommand()
