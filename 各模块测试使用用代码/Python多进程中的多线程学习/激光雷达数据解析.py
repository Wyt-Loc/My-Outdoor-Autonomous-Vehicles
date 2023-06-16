# -*- coding: utf-8 -*-
# @Time    : 2023/6/15 11:32
# @Author  : Wyt
# @File    : 激光雷达数据解析.py

import socket
import matplotlib.pyplot as plt
import numpy as np


# 地址 192.168.0.100
# 端口 8487

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
    def getCrcPackage4Byte(a, b, c, d) -> bool:
        """
        数据帧校验
        :param a: a
        :param b: b
        :param c: c
        :param d: d
        :return: true or false
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

        numone = (cbit[b] + cbit[c] + cbit[d]) & 0x07
        if numone == ((a & 0x70) >> 4):
            return True
        else:
            return False


distance = [0]
angle = [0]
distancef = 0
anglef = 0

if __name__ == '__main__':

    plt.figure(figsize=(10.8, 9.6), dpi=100)  # 设置画布大小 1080 * 960 的画布
    ax = plt.gca(projection='polar')
    ax.set_thetagrids(np.arange(0.0, 360.0, 30.0))
    ax.set_thetamin(0.0)  # 设置极坐标图开始角度为0°
    ax.set_thetamax(360.0)  # 设置极坐标结束角度为360°

    lidar = Lidar
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        s.connect(('192.168.0.100', 8487))
        print("激光雷达连接成功")
    except ValueError:
        print("激光雷达连接失败")

    while True:
        data = s.recv(4096)
        distance = []
        angle = []
        for i, val in enumerate(data):
            # 对应4个字节 防止越界
            if i <= (len(data) - 4) \
                    and data[i] & 0x80 == 0 \
                    and data[i + 1] & 0x80 == 0 \
                    and data[i + 2] & 0x80 == 0 \
                    and data[i + 3] & 0x80 == 0x80:
                # 校验 BCD
                if lidar.getCrcPackage4Byte(data[i], data[i + 1], data[i + 2], data[i + 3]):
                    # ABC 计算距离
                    # distance.append(lidar.getDistance(data[i], data[i + 1], data[i + 2]))
                    distancef = data[i] & 0x0F
                    distancef <<= 7
                    distancef += data[i + 1] & 0x7F
                    distancef <<= 1
                    if distancef > 1500 or distancef < 100:
                        continue
                    distance.append(distancef)
                    if data[i + 2] & 0x40:
                        distance[0] += 1
                    # print("距离 = ", distance, "cm")
                    # angle.append(((data[i] & 0x0f) << 7) | ((data[i + 1] & 0x7f) << 1) | ((data[i + 2] & 0x40) >> 6))
                    anglef = (data[i + 2] & 0x3F)
                    anglef <<= 7
                    anglef += data[i + 3] & 0x7F
                    anglef = anglef / 16
                    angle.append(anglef)
                    # print("角度 = ", angle)
                    # print("角度 = ", angle)
                    # print("%%%")

        for i in range(len(angle)):
            angle[i] = angle[i] * (np.pi / 180)
        plt.clf()  # 清图
        plt.polar(angle, distance, marker='o', ls='none', markersize=2, color='b')  # 画雷达图
        plt.pause(0.01)  # 暂停一段时间，不然画的太快会卡住显示不出来
plt.ioff()  # 关闭画图窗口
plt.show()  # 显示
