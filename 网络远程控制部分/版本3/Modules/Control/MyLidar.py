# -*- coding: utf-8 -*-
# @Time    : 2023/6/30 13:57
# @Author  : Wyt
# @File    : MyLidar.py
import heapq
import socket
import numpy as np
from matplotlib import pyplot as plt
import 网络远程控制部分.版本3.Modules.Control.MyControl


# 地址 192.168.0.100
# 端口 8487


class Lidar(网络远程控制部分.版本3.Modules.Control.MyControl.Mykey):
    lidarData = []
    lidarmap = []
    lidarDistance = []
    lidarAngle = []  # 原始角度
    x1 = []
    y1 = []
    path = []
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    def __init__(self):
        print("正在连接雷达.....")
        # 连接激光雷达
        RECV_BUF_SIZE = 4096
        self.s.setsockopt(  # 加入 此配置 解决TCP数据延迟问题，在因为默认缓冲区大小维65535 那么第一次可能是要接满
            # 才会有下一次，所以之后数据会一直延迟5秒，加入此项解决这个问题。
            socket.SOL_SOCKET,
            socket.SO_RCVBUF,
            RECV_BUF_SIZE)
        self.s.connect(('192.168.0.100', 8487))
        print("激光雷达连接成功")

    def receLidarData(self):
        # 接收TCP数据
        self.lidarData = self.s.recv(4096)

    def getDistance(self, data1: int, data2: int, data3: int) -> int:
        # 得到距离
        return ((data1 & 0x1) << 14) + ((data2 & 0x7F) << 7) + (data3 & 0x7F)

    # cbit 为对应 0~255 的 1 的个数表

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

    def heuristic(self, a, b):
        # 计算启发式函数（曼哈顿距离）
        return abs(b[0] - a[0]) + abs(b[1] - a[1])

    def astar(self, start, goal):
        # 定义可行动的方向（上、下、左、右和对角线）
        directions = [(-1, 0), (1, 0), (0, -1), (0, 1), (-1, -1), (-1, 1), (1, -1), (1, 1)]
        # 初始化起点和终点
        start_node = (self.heuristic(start, goal), start, 0, None)
        goal_node = (self.heuristic(start, goal), goal, float('inf'), None)
        # 定义开放列表和关闭列表
        open_list = []
        closed_list = set()
        # 将起点加入开放列表
        heapq.heappush(open_list, start_node)
        while open_list:
            # 从开放列表中取出具有最小预估总代价的节点
            current_node = heapq.heappop(open_list)
            current_pos = current_node[1]
            # 到达目标节点
            if current_pos == goal_node[1]:
                path = []
                # 从终点回溯至起点得到路径
                while current_node:
                    path.append(current_node[1])
                    current_node = current_node[3]
                return list(reversed(path))
            # 将当前节点标记为关闭状态
            closed_list.add(current_pos)
            try:
                # 遍历可行动的方向
                for direction in directions:
                    next_pos = (current_pos[0] + direction[0], current_pos[1] + direction[1])
                    # 检查下一个节点是否越界或为障碍物
                    if next_pos[0] < 0 or next_pos[0] >= len(self.lidarmap) or next_pos[1] < 0 or \
                            next_pos[1] >= len(self.lidarmap[0]) or \
                            self.lidarmap[next_pos[0]][next_pos[1]] == 1:
                        continue
                    # 计算下一个节点的代价和预估总代价
                    cost = current_node[2] + 1
                    next_node = (cost + self.heuristic(next_pos, goal), next_pos, cost, current_node)
                    # 如果下一个节点已经在关闭列表中，则跳过
                    if next_pos in closed_list:
                        continue
                    # 如果下一个节点已经在开放列表中且代价更高，则跳过
                    for node in open_list:
                        if next_pos == node[1] and cost >= node[2]:
                            break
                    else:
                        # 将下一个节点加入开放列表
                        heapq.heappush(open_list, next_node)
            except:
                pass
        # 如果开放列表为空，表示无法到达目标节点
        return []

    # 转到笛卡尔坐标系

    def polar_to_cartesian(self, r, theta):
        x = r * np.cos(theta)
        y = r * np.sin(theta)
        return x, y

    def parsingLidarData(self):
        self.lidarDistance = []
        self.lidarAngle = []  # 原始角度
        self.receLidarData()
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
                    self.lidarDistance.append(distancef)
                    if self.lidarData[i + 2] & 0x40:
                        self.lidarDistance[0] += 1
                    anglef = (self.lidarData[i + 2] & 0x3F)
                    anglef <<= 7
                    anglef += self.lidarData[i + 3] & 0x7F
                    anglef = anglef / 16
                    self.lidarAngle.append(anglef)
                    # print("方向 = ", anglef, "距离 = ", distancef)

    def generationMap(self):
        # 生成地图
        for i in range(len(self.lidarAngle)):
            self.lidarAngle[i] = self.lidarAngle[i] * (np.pi / 180)  # 极坐标
        self.x1, self.y1 = self.polar_to_cartesian(self.lidarDistance, self.lidarAngle)
        for i in range(len(self.x1)):
            self.x1[i] = self.x1[i]
            self.y1[i] = self.y1[i]
        xmax = int(max(self.x1) + 1)
        ymax = int(max(self.y1) + 1)
        # print("x = ", xmax, "y=", ymax)
        if xmax < 100 or ymax < 300:
            return
        self.lidarmap = [[0 for _ in range(ymax)] for _ in range(xmax)]
        num_rows = len(self.lidarmap)
        num_cols = len(self.lidarmap[0])
        for i in range(len(self.x1)):
            row = int(self.x1[i])
            col = int(self.y1[i])
            for t in range(max(0, row - 1), min(num_rows, row + 10)):
                for j in range(max(0, col - 1), min(num_cols, col + 10)):
                    self.lidarmap[t][j] = 1
            # print(mymap)

    def findRoad(self, start, goal):
        self.path = self.astar(start, goal)
        if self.path:
            print("找到了")
            # print("路径：", self.path)
        else:
            print("无法到达目标节点")

    def displayImg(self):
        plt.figure(figsize=(10.8, 9.6), dpi=100)  # 设置画布大小 1080 * 960 的画布
        ax = plt.subplot(projection='polar')
        ax.set_thetagrids(np.arange(0.0, 360.0, 30.0))
        ax.set_thetamin(0.0)  # 设置极坐标图开始角度为0°
        ax.set_thetamax(360.0)  # 设置极坐标结束角度为360°
        plt.autoscale(enable=False, tight=False)
        start1 = (0, 0)
        goal1 = (100, 300)
        while True:
            self.parsingLidarData()  # 解析数据
            self.generationMap()  # 生成地图
            self.findRoad(start1, goal1)  # 显示
            xx = []
            yy = []
            print(len(self.path))
            for i in range(len(self.path)):
                xx.append(self.path[i][0])
                yy.append(self.path[i][1])
            plt.clf()  # 清图
            plt.scatter(self.x1, self.y1, s=1)
            plt.scatter(xx, yy, s=1)
            plt.xlabel('x')
            plt.ylabel('y')
            plt.title('Cartesian Coordinate System')
            plt.grid(True)
            plt.pause(0.01)  # 暂停一段时间，不然画的太快会卡住显示不出来

# if __name__ == '__main__':
#     lidar = Lidar()
#     lidar.displayImg()
