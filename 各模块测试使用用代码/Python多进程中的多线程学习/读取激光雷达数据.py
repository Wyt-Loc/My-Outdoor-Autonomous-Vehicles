# -*- coding: utf-8 -*-
# @Time    : 2023/6/15 10:34
# @Author  : Wyt
# @File    : 读取激光雷达数据.py
import math
import socket
from matplotlib import pyplot as plt


def jiaoyan(A, B, C, D):
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


# 地址 192.168.0.100
# 端口 8487
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
try:
    s.connect(('192.168.0.100', 8487))
    print("激光雷达连接成功")
except:
    print("激光雷达连接失败")

while True:
    x = []
    y = []
    z = []
    data = s.recv(1024)
    print(type(data))

    for id, i in enumerate(data):
        if i & 0x80 == 0x00 \
                and data[id + 1] & 0x80 == 0x00 \
                and data[id + 2] & 0x80 == 0x00 \
                and data[id + 3] & 0x80 == 0x80:  # 满足数据格式
            if jiaoyan(data[id], data[id + 1], data[id + 2], data[id + 3]):
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
