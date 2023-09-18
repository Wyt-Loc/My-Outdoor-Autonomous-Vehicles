# -*- coding: utf-8 -*-
# @Time    : 2023/9/6 20:34
# @Author  : Wyt
# @File    : 滤波.py
import math
import socket
import queue

# 地址 192.168.0.100
# 端口 8487
import numpy as np
from matplotlib import pyplot as plt


class Lidar:
    lidarData = []  # 接收Socket的列表
    angleQueue = []
    disQueue = []
    mapQueue = queue.Queue(maxsize=1300)
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def __init__(self):
        RECV_BUF_SIZE = 4096
        self.s.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, RECV_BUF_SIZE)
        self.s.connect(('192.168.0.100', 8487))
        print("激光雷达连接成功")

    def getCrcPackage4Byte(self, a, b, c, d) -> bool:
        cbit = [
            0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
            1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
            1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
            2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
            1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
            2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
            2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
            3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8]
        numone = (cbit[b] + cbit[c] + cbit[d]) & 0x07  # 得到 1 的个数
        if numone == ((a & 0x70) >> 4):
            return True
        else:
            return False

    def parsingLidarData(self):
        for i, val in enumerate(self.lidarData):
            # 对应4个字节 防止越界
            if i <= (len(self.lidarData) - 4) \
                    and self.lidarData[i] & 0x80 == 0 \
                    and self.lidarData[i + 1] & 0x80 == 0 \
                    and self.lidarData[i + 2] & 0x80 == 0 \
                    and self.lidarData[i + 3] & 0x80 == 0x80:
                # 校验 BCD
                if self.getCrcPackage4Byte(self.lidarData[i], self.lidarData[i + 1], self.lidarData[i + 2],
                                           self.lidarData[i + 3]):
                    distancef = (self.lidarData[i] & 0x0F)
                    distancef <<= 7
                    distancef += self.lidarData[i + 1] & 0x7F
                    distancef <<= 1
                    if distancef > 600 or distancef < 10:  # 10cm - 20m
                        continue
                    self.disQueue.append(distancef)
                    anglef = (self.lidarData[i + 2] & 0x3F)
                    anglef <<= 7
                    anglef += self.lidarData[i + 3] & 0x7F
                    anglef = anglef / 16
                    self.angleQueue.append(anglef)

    def polar_to_cartesian(self, r, theta):
        x = r * np.cos(theta)
        y = r * np.sin(theta)
        return x, y

    def SRG_filtering(self):
        plt.figure(figsize=(10.8, 9.6), dpi=100)  # 设置画布大小 1080 * 960 的画布
        ax = plt.subplot(projection='polar')
        ax.set_thetagrids(np.arange(0.0, 360.0, 30.0))
        ax.set_thetamin(0.0)  # 设置极坐标图开始角度为0°
        ax.set_thetamax(360.0)  # 设置极坐标结束角度为360°
        while True:
            self.lidarData = self.s.recv(4096)
            self.angleQueue = []
            self.disQueue = []
            self.parsingLidarData()
            for i in range(len(self.angleQueue)):
                self.angleQueue[i] = self.angleQueue[i] * (np.pi / 180)  # 极坐标
            plt.clf()  # 清图
            plt.scatter(0, 0, s=1)
            plt.xlim((-300, 300))
            plt.ylim((-300, 300))
            plt.scatter(self.angleQueue, self.disQueue, s=1)
            plt.xlabel('x')
            plt.ylabel('y')
            plt.title('Cartesian Coordinate System')
            plt.grid(True)
            plt.pause(0.0001)  # 暂停一段时间，不然画的太快会卡住显示不出来

    def receLidarData(self):

        self.SRG_filtering()


if __name__ == '__main__':
    a = Lidar()
    a.receLidarData()
