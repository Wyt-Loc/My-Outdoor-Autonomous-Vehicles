# -*- coding: utf-8 -*-
# @Time    : 2023/6/15 11:32
# @Author  : Wyt
# @File    : 激光雷达数据解析.py

# import math
import socket

import numpy as np
from matplotlib import pyplot as plt


# import struct
# import time

# 地址 192.168.0.100
# 端口 8487


# data = b't\x00\x0b\xb4s\x7f\x0b\xb9\x03\x7f\x0b\xbdS~K\xc2c\x7f\x0b\xc5t\x00\x0b\xc9' \
#        b'\x14\x00K\xce\x04\x00K\xd2\x14\x00K\xd6\x14\x00K\xda4\x01\x0b\xdf\x14\x02\x0b\xe3'


# 转为2进制
# bin_data = ''.join(format(byte, '08b') for byte in data)
# print(type(bin_data), bin_data)


# 得到原始数据串 数据为4个字节一个包， 但是不能每次只读取4个字节
# 读取多个数据进行删选， 一般来说找到第一个对的，之后都是连续的了
# 速度上不会差很多


class Lidar:
    """
    雷达类，封装雷达基本函数
    """

    @staticmethod
    def getDistance(data1: int, data2: int, data3: int) -> int:
        """
        :param data1: A
        :param data2: B
        :param data3: C
        :return: 距离值（m）
        """
        return ((data1 & 0x1) << 14) + ((data2 & 0x7F) << 7) + (data3 & 0x7F)

    @staticmethod  # 没有使用self 设为静态函数即可
    def getCrcPackage4Byte(data1: int, data2: int, data3: int) -> int:
        """
        数据帧校验
        :param data1: B
        :param data2: C
        :param data3: D
        :return: BCD 三个字节的 1 的个数和的低三位
        """
        # cbit 为对应 0~255 的 1 的个数表
        cbit = [
            0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
            1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
            1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
            2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
            1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
            2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
            2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
            3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8]

        return (cbit[data1] + cbit[data2] + cbit[data3]) & 0x07


distance = [0]
angle = [0]
distancef = [0]

if __name__ == '__main__':
    lidar = Lidar
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        s.connect(('192.168.0.100', 8487))
        print("激光雷达连接成功")
    except ValueError:
        print("激光雷达连接失败")

    azim = -90
    elev = 90
    while True:
        data = s.recv(1024)

        for i, val in enumerate(data):
            # 对应4个字节 防止越界
            if i <= (len(data) - 4) \
                    and data[i] & 0x80 == 0 \
                    and data[i + 1] & 0x80 == 0 \
                    and data[i + 2] & 0x80 == 0 \
                    and data[i + 3] & 0x80 == 0x80:
                # 校验 BCD
                print(lidar.getCrcPackage4Byte(data[i + 1], data[i + 2], data[i + 3]))
                # ABC 计算距离
                distance.append(lidar.getDistance(data[i], data[i + 1], data[i + 2]))
                # distance[0] = lidar.getDistance(data[i], data[i + 1], data[i + 2])
                print("距离 = ", distance)
                angle.append(((data[i] & 0x0f) << 7) | ((data[i + 1] & 0x7f) << 1) | ((data[i + 2] & 0x40) >> 6))
                # angle[0] = ((data[i] & 0x0f) << 7) | ((data[i + 1] & 0x7f) << 1) | ((data[i + 2] & 0x40) >> 6)
                print("角度 = ", angle)

                plt.ion()
                plt.clf()  # 清除之前画的图
                import matplotlib.pyplot as plt

                plt.show()
                plt.pause(0.002)  # 暂停一段时间，不然画的太快会卡住显示不出来
                # print("角度 = ", angle)
                # print("%%%")
