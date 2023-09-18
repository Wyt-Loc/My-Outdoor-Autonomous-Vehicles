from math import sin, cos
import socket
import time
import threading
from matplotlib import pyplot as plt  # 用来绘制图形
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import serial.tools.list_ports
import math
import re
import os

# 设备当前相对角度
xcar_jiao = 0
ycar_jiao = 0
zcar_jiao = 0
# 设备当前相对位置
xcar = 0
ycar = 0
zcar = 0
# 地图
xyz = True  # 锁：当对xyz修改的时候不允许使用
x = []  # 右正
y = []  # 前正
z = []  # 上正
# 总开关
OK = True


# 串口检测
def usart_jiance():
    port_list = list(serial.tools.list_ports.comports())
    # print(port_list)
    if len(port_list) == 0:
        input("'无可用串口,任意键退出")
        exit()
    else:
        for i in range(0, len(port_list)):
            print(port_list[i])


usart_jiance()


# 鼠标控制函数
def onclick(event):
    global OK
    if event.button == 3:  # 鼠标右键退出系统
        OK = False


# 姿态
class myThread_zitai(threading.Thread):
    def __init__(self, name, showok):
        threading.Thread.__init__(self)
        self.name = name
        self.showok = showok
        self.use = True
        self.LEN = 28
        self.LENJIA = 12
        self.LENJIAO = 12
        self.jiuzhou_data = []  # 加速度，实时计算所在位置 # 角速度，积分得到当前角度
        self.lasetime = [time.time()]  # 为了对速度和角速度进行积分
        self.js = 0
        self.lasetime2 = time.time()  # 为了配合js实现输出速度效果
        self.lastxv = 0
        self.lastyv = 0
        self.lastzv = 0
        self.lastxj = 0
        self.lastyj = 0
        self.lastzj = 0
        self.ser = None

    def print(self, *string):
        if self.showok:
            print(self.name + " - - ", end="")
            for i in string:
                print(i, end=' ')
            print()

    def end(self):
        self.use = False

    def updata_weizhi(self):
        global xcar, ycar, zcar, xcar_jiao, ycar_jiao, zcar_jiao
        if self.jiuzhou_data:  # 非空
            if len(self.jiuzhou_data) == len(self.lasetime) - 1:  # 且等大，否则就是有问题
                for i in range(0, len(self.jiuzhou_data)):
                    oncetime = self.lasetime[i + 1] - self.lasetime[i]  # 计算时间间隔
                    """以下位置、方向信息直接写到全局变量中，方便使用"""
                    # print(i,len(self.jiuzhou_data),len(self.jiuzhou_data[0]))  # 测试数据
                    xc = xcar_jiao + oncetime * (self.jiuzhou_data[i][3] + self.lastxj) / 2  # 计算角度值
                    yc = ycar_jiao + oncetime * (self.jiuzhou_data[i][4] + self.lastyj) / 2
                    zc = zcar_jiao + oncetime * (self.jiuzhou_data[i][5] + self.lastzj) / 2
                    # print(self.jiuzhou_data[i][5]-self.lastzj)
                    if self.jiuzhou_data[i][3] - self.lastxj < 300 and self.jiuzhou_data[i][4] - self.lastyj < 300 and \
                            self.jiuzhou_data[i][5] - self.lastzj < 300:  # 防止突变，其实还是数据验证有问题
                        xcar_jiao = xc + 0.00011  # 千辛万苦标定出来的数据，抵抗固有偏移
                        ycar_jiao = yc + 0.00078  # 静态精度每二分之一cmd不超过0.1°的偏移
                        zcar_jiao = zc - 0.000412  # 也可采用程序标定，静止防止一定时间，终值除以次数，以后这个数值不准确了再用
                        self.lastxj = self.jiuzhou_data[i][3]
                        self.lastyj = self.jiuzhou_data[i][4]
                        self.lastzj = self.jiuzhou_data[i][5]
                        self.print("当前方向角为: ", xcar_jiao, ycar_jiao, zcar_jiao)
                        """通过uwb定位系统进行定位，暂时不使用惯导，时间长了难免漂移，
                        另外，还需要在以下代码基础上考虑如何去掉重力加速度，现实状况下z轴不可能时刻竖直，
                        所以xyz方向上都有分量，具体该如何去除，现在有点想法，是利用实际线速度为0时候，
                        记录此时的各个加速度值，每次计算进行扣除，想法留待以后实验
                        """
                        """
                        xcar = xcar + self.lastxv * oncetime + self.jiuzhou_data[i][0] * oncetime * oncetime / 2
                        ycar = ycar + self.lastyv * oncetime + self.jiuzhou_data[i][1] * oncetime * oncetime / 2
                        zcar = zcar + self.lastzv * oncetime + self.jiuzhou_data[i][2] * oncetime * oncetime / 2
                        self.lastxv = self.lastxv + self.jiuzhou_data[i][0] * oncetime
                        self.lastyv = self.lastyv + self.jiuzhou_data[i][1] * oncetime
                        self.lastzv = self.lastzv + self.jiuzhou_data[i][2] * oncetime
                        self.print(xcar, "   ", ycar, "   ", zcar)
                        """
                """将处理完毕的数据进行截除"""
                self.jiuzhou_data = []  # 处理完了，清空
                self.lasetime = self.lasetime[-1:]  # 只剩下最后一项作为一下次的首位时间
            else:
                self.print("数据长度错误，不进行解决将会产生严重错误，将自行退出姿态线程")
                self.end()

    def sudu(self):
        """每次成功存一帧数据之后，必须存此时的时间戳"""
        self.lasetime.append(time.time())
        """进行输出显示"""
        self.js = self.js + 1
        if self.js == 100:
            self.print((self.lasetime[-1] - self.lasetime2) / 100, "s/帧")
            self.lasetime2 = self.lasetime[-1]
            self.js = 0

    def jisuan(self, a, b, c, d):
        """负数用补码（大坑啊）"""
        byte16 = (a << 0) | (b << 8) | (c << 16) | (d << 24)
        if ((byte16 & 0x80000000) == 0):
            result = byte16
        else:
            byte16 = byte16 ^ 0xffffffff
            byte16 = byte16 + 1
            result = -byte16
        return result * 0.000001

    def yanzheng(self, data, id0, id1):
        """求和验证数据传输准确性
            下述方法是经验之谈，和文档有出入，
            可以看到验证数据时候我只使用低8位进行检验，实属无奈之举，
            上面的几位总是不一致，通过不了验证，所以我才放低了条件，事实说明用起来还可以
        """
        CK1 = 0
        CK2 = 0
        for i in data[id0:id1 + 1]:
            CK1 = CK1 + i
            CK2 = CK2 + CK1
        CK = (CK2 << 8 | CK1) & 0x0000ff  # 冗余代码
        CKc = (data[id1 + 2] << 8 | data[id1 + 1]) & 0x0000ff  # 冗余代码
        if CK == CKc:
            return True
        else:
            # print(bin(CK))  # 用于测试
            # print(bin(CKc))
            # print('bbbbb:', data[id0-2:id1+5])
            return False

    def jiexi(self, data):
        for i, ii in enumerate(data):
            if ii == 89 and data[i + 1] == 83:  # 标志位检验
                if data[i + 4] == self.LEN:  # 验证数据可靠性
                    if data[i + 5] == 0x10 and data[i + 6] == self.LENJIA and data[i + 19] == 0x20 and data[
                        i + 20] == self.LENJIAO:  # 后面的12字节是加速度值、后面的12字节是角速度值
                        if self.yanzheng(data, i + 2, i + 32):
                            xa = self.jisuan(data[i + 7], data[i + 8], data[i + 9], data[i + 10])  # x加速度
                            ya = self.jisuan(data[i + 11], data[i + 12], data[i + 13], data[i + 14])  # y加速度
                            za = self.jisuan(data[i + 15], data[i + 16], data[i + 17], data[i + 18])  # z加速度
                            xwa = self.jisuan(data[i + 21], data[i + 22], data[i + 23], data[i + 24])  # x角加速度
                            ywa = self.jisuan(data[i + 25], data[i + 26], data[i + 27], data[i + 28])  # y角加速度
                            zwa = self.jisuan(data[i + 29], data[i + 30], data[i + 31], data[i + 32])  # z角加速度
                            self.jiuzhou_data.append([xa, ya, za, xwa, ywa, zwa])
                            self.sudu()  # 每执行一个数据解析则进行一次时间采集
                            # self.print(onezhen)
                            return self.jiexi(data[i + 35:])  # CK1、CK2也算上  # 递归查找下一帧，直到return data
        return data

    def run(self):
        self.print("线程启动")
        try:
            self.ser = serial.Serial("COM4", 921600)  # 位置1
            self.ser.flushInput()  # 位置2
            self.print("串口初始化完成")
        except:
            self.print("串口初始化失败!!!")
            self.end()
        recv = []
        while self.use and OK:
            count = self.ser.inWaiting()
            if count != 0:
                recv = recv + list(self.ser.read(count))  # list连接
                self.ser.flushInput()  # 清空缓冲区
                if len(recv) > 200:
                    try:
                        # print("rece = ", recv)
                        recv = self.jiexi(recv)  # 解析数据
                        self.updata_weizhi()  # 将所有得到的数据进行计算，得到位置信息到全局变量中
                    except BaseException as e:
                        pass
                        # self.print("解析错误:", e)
        self.print("线程退出")


# 雷达
class myThread_leida(threading.Thread):
    def __init__(self, name, showok):
        threading.Thread.__init__(self)
        self.name = name
        self.showok = showok
        self.use = True
        self.s = None
        self.data = b''

    def print(self, *string):
        if self.showok:
            print(self.name + " - - ", end="")
            for i in string:
                print(i, end=' ')
            print()

    def end(self):
        self.use = False

    def jiaoyan(self, A, B, C, D):
        """ABCD为原数据，返回验证成功或者失败"""
        js = 0
        for i in range(8):
            if (B & (0x01 << i)) != 0x00:
                js = js + 1
            if (C & (0x01 << i)) != 0x00:
                js = js + 1
            if (D & (0x01 << i)) != 0x00:
                js = js + 1
        if ((A & 0x70) >> 4) == (js & 0x07):
            # self.print("数据验证通过")
            return True
        else:
            # self.print("数据验证失败")
            return False

    def jiexi(self, data):
        global x, y, z
        # data = list(data)  # 转化为list会是数据解析过程变得直观，否则字符串看起来奇奇怪怪的，python十六进制的转化效果不好，也不知道编码协议上的事儿
        for id, i in enumerate(data):
            if i & 0x80 == 0x00 \
                    and data[id + 1] & 0x80 == 0x00 \
                    and data[id + 2] & 0x80 == 0x00 \
                    and data[id + 3] & 0x80 == 0x80:  # 满足数据格式
                """数据校验"""
                if self.jiaoyan(data[id], data[id + 1], data[id + 2], data[id + 3]):
                    """按照文档解析数据,单位是cm和角度"""
                    juli = ((data[id] & 0x0f) << 7) | ((data[id + 1] & 0x7f) << 1) | ((data[
                                                                                           id + 2] & 0x40) >> 6)  # 按文档的说法第一项位移8位才对，我也这么认为，但是案例中代码表示位移7就对了，这样的话，实际上是A0和B6是存在或的关系的，而不是简单的排列了，但是事实就是如此，所以说这里绝对存在问题，是厂家的下位机的问题，而上位机人员进行了妥协，但是文档缺还是按照设计之初所想的那样写的，这是我的理解
                    jiao = (((data[id + 2] & 0x3f) << 7) | (data[id + 3] & 0x7f)) / 16
                    if juli < 350:
                        if len(x) < 4480:
                            x.append(-math.cos(jiao / 180 * 3.1415926) * juli)
                            y.append(math.sin(jiao / 180 * 3.1415926) * juli)
                            z.append(0)
                        else:
                            x = [-math.cos(jiao / 180 * 3.1415926) * juli]
                            y = [math.sin(jiao / 180 * 3.1415926) * juli]
                            z = [0]
                        # print(juli, jiao)
                    return self.jiexi(data[id + 4:])  # 去除已解析的数据，递归查询所有
        return data

    def run(self):
        global xyz
        self.print("线程启动")
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            s.connect(('192.168.0.100', 8487))
            self.print("激光雷达连接成功")
        except:
            self.print("激光雷达连接失败")
            self.end()

        while self.use and OK:
            self.data = self.data + s.recv(1024)
            xyz = False
            self.data = self.jiexi(self.data)
            xyz = True

        self.print("线程退出")


# uwb模块
class myThread_uwb(threading.Thread):
    def __init__(self, name, showok):
        threading.Thread.__init__(self)
        self.name = name
        self.showok = showok
        self.use = True
        self.m0 = bytes(self.Modbus10Send_init(0x01, 0x28, 0x10, [0x00, 0x01]))  # 一次定位需要发送的数据，直接就修改好
        self.m1 = bytes(self.Modbus03Send_init(0x01, 0x00, 0x03, 0x6A))
        self.m2 = bytes(self.Modbus10Send_init(0x01, 0x28, 0x10, [0x00, 0x04]))
        self.m3 = bytes(self.Modbus10Send_init(0x01, 0x28, 0x10, [0x00, 0x00]))
        self.data = b''
        self.ser = None

    def print(self, *string):
        if self.showok:
            print(self.name + " - - ", end="")
            for i in string:
                print(i, end=' ')
            print()

    def end(self):
        self.use = False

    def Crc16(self, buffer):
        Table_crc_hi = [
            0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
            0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
            0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
            0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
            0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
            0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
            0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
            0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
            0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
            0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
            0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
            0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
            0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
            0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
            0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
            0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
            0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
            0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
            0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
            0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
            0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
            0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
            0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
            0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
            0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
            0x80, 0x41, 0x00, 0xC1, 0x81, 0x40]
        Table_crc_lo = [
            0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
            0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
            0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
            0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
            0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
            0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
            0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
            0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
            0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
            0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
            0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
            0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
            0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
            0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
            0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
            0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
            0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
            0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
            0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
            0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
            0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
            0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
            0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
            0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
            0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
            0x43, 0x83, 0x41, 0x81, 0x80, 0x40]
        crc_hi = 0xFF
        crc_lo = 0xFF
        for i in range(len(buffer)):
            t = crc_hi ^ buffer[i]
            crc_hi = crc_lo ^ Table_crc_hi[t]
            crc_lo = Table_crc_lo[t]
        return (crc_hi << 8) | crc_lo

    def Modbus03Send_init(self, id, add, what, num):
        result = []
        result.append(id)
        result.append(what)
        result.append(add >> 8)
        result.append(add & 0x00ff)
        result.append(num >> 8)
        result.append(num & 0x00ff)
        cec = self.Crc16(result)
        result.append(cec >> 8)
        result.append(cec & 0x00ff)
        return result

    def Modbus10Send_init(self, id, add, what, data):
        """
        功能：进行一次定位
        参数：
        id （byte）            从设备名称
        add（byte）           寄存器开始地址
        what（byte）          事件号 、 命令号
        data (list :: byte)   是一个list，存储的数据为byte
        """
        result = []
        result.append(id)
        result.append(what)
        result.append(add >> 8)
        # print(add & 0x00ff)
        result.append(add & 0x00ff)
        num = int(len(data) / 2)
        result.append(num >> 8)
        result.append(num & 0x00ff)
        result.append(num * 2)
        for i in range(num * 2):
            result.append(data[i])
        cec = self.Crc16(result)
        result.append(cec >> 8)
        result.append(cec & 0x00ff)
        # print(result)
        return result

    def one_uwb(self):
        """发一次测一次，且需手动查询"""
        self.ser.write(self.m0)
        self.print("发送单次测距命令：", self.m0)

    def many_uwb(self):
        """自动返回测量数据，使用此函数必须结束，否则一直测距，浪费资源"""
        self.ser.write(self.m2)
        self.print("发送持续测距命令：", self.m2)

    def end_uwb(self):
        """配合many_uwb使用，停止测量"""
        self.ser.write(self.m3)
        self.print("发送停止测距命令：", self.m3)

    def cha(self):
        """主动查询定位后的数据，配合one_uwb"""
        data = '\x01\x03\x00\x2A\x00\x0D\xA5\xC7'
        self.ser.write(data.encode())
        self.print("发送查询命令：", data.encode())

    def lianjie(self):
        self.ser.write(self.m3)
        self.print("启动时查询命令：", self.m3)

    def jiexi(self):
        """
        将串口得到的数据解析为能用的 x y z
        data:b'\x01\x03*\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x043\x00,\x1eC\x1e\xca\x1c\xbd\x06\xdd\x00{\xbb\xd2\x07\xf5'
        """
        global xcar, ycar, zcar
        self.data = list(self.data)
        while len(self.data) > 0:
            try:
                if self.data[0] == 0x01 and self.data[0 + 1] == 0x03 and self.data[0 + 2] == 0x2a and self.data[
                    0 + 3] == 0x00 and self.data[0 + 4] == 0x00:
                    # print(self.data)
                    if self.data[5] & 0x01 == 1:  # 成功定位标志
                        """data[5]<<8 | data[6]这个数据的解释：第 1~8bit 为对应 A~H 基站测距成功标志，第 9bit 为定位计算成功标志"""
                        xcar = self.data[7] << 8 | self.data[8]
                        ycar = self.data[9] << 8 | self.data[10]
                        zcar = self.data[11] << 8 | self.data[12]
                        # self.print(xcar, ' ', ycar, ' ', zcar)
                    self.data = self.data[47:]  # 本帧数据解析完毕
                else:
                    self.data = self.data[1:]  # 如果当前不是有效数据，则向前推进
            except:  # 唯一的错误可能就越界，直接推进清空就可以
                self.data = self.data[1:]  # 如果当前不是有效数据，则向前推进
        self.data = bytes(self.data)

    def recv(self):
        count = self.ser.inWaiting()  # 获得缓冲区长度
        recv = b''
        if count != 0:
            try:
                recv = self.ser.read(count)  # 读取缓冲区数据
            except:
                pass
            # self.print("usart recv data ：", recv)
        return recv

    def run(self):
        self.print("线程启动")
        # 初始化串口
        try:
            self.ser = serial.Serial('COM3', 115200)  # 打开串口
            # print(self.ser)  # 串口信息
            self.ser.flushInput()  # 清除缓冲区
            self.print("串口COM3初始化成功")
        except:
            self.print("error:串口初始化失败")
            self.end()
        # 主循环开始
        # self.lianjie()
        # self.recv()
        self.many_uwb()  # 持续定位，下位机一直返回数据，直到收到停止命令
        time.sleep(0.1)
        # self.end_uwb()  # 测试时候一般都是直接结束程序，所以无法运行到下面的end，只能取消此注释进行手动停止
        self.data = b''
        while self.use and OK:
            self.data = self.data + self.recv()
            self.jiexi()

        while 1:
            self.end_uwb()
            time.sleep(0.1)
            # ser.write(''.encode())  # 串口’写‘函数
            c = self.recv()
            if len(c) == 8:
                self.print("uwb持续定位已结束")
                break
        self.print("线程退出")


def main():
    t0 = time.time()
    threads = []
    # 创建新线程
    # thread1 = myThread_huizhi("绘图", True)
    # 创建新线程
    # thread2 = myThread_leida("激光雷达", True)
    # 创建新线程
    thread3 = myThread_zitai("姿态传感器", True)
    # 创建新线程
    # thread4 = myThread_uwb("uwb模块", True)
    # thread1.start()
    # thread1.join(timeout=1)      # 守护线程# 主程序一秒后结束
    # thread2.start()
    # thread2.join(timeout=1)      # 守护线程# 主程序一秒后结束
    thread3.start()
    thread3.join(timeout=1)  # 守护线程# 主程序一秒后结束
    # thread4.start()
    # thread4.join(timeout=1)      # 守护线程# 主程序一秒后结束
    while OK:
        t1 = time.time()
        if (t1 - t0 > 60):
            print("系统持续运行时间{}s".format(t1 - t0))  # 8 \8 \
            t0 = t1
        time.sleep(1)
    print("无人驾驶系统：系统退出")


if __name__ == '__main__':
    main()
