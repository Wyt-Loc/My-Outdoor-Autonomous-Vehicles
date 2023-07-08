# -*- coding: utf-8 -*-
# @Time    : 2023/6/30 13:57
# @Author  : Wyt
# @File    : MyLidar.py
import heapq
import math
import socket
import time
import matplotlib.pyplot as plt
import numpy as np


# 地址 192.168.0.100
# 端口 8487


class Lidar:
    """
    雷达类
    """

    @staticmethod
    def getDistance(data1: int, data2: int, data3: int) -> int:
        # 得到距离
        return ((data1 & 0x1) << 14) + ((data2 & 0x7F) << 7) + (data3 & 0x7F)

    # 转到笛卡尔坐标系
    @staticmethod
    def polar_to_cartesian(r, theta):
        x = r * np.cos(theta)
        y = r * np.sin(theta)
        return x, y

    @staticmethod  # 没有使用self 设为静态函数即可
    def getCrcPackage4Byte(a, b, c, d) -> bool:
        # 数据帧校验
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

        numone = (cbit[b] + cbit[c] + cbit[d]) & 0x07  # 得到 1 的个数
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
    plt.autoscale(enable=False, tight=False)

    ditu = []
    x = []
    y = []
    lidar = Lidar
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    RECV_BUF_SIZE = 4096
    s.setsockopt(  # 加入 此配置 解决TCP数据延迟问题，在因为默认缓冲区大小维65535 那么第一次可能是要接满
        # 才会有下一次，所以之后数据会一直延迟5秒，加入此项解决这个问题。
        socket.SOL_SOCKET,
        socket.SO_RCVBUF,
        RECV_BUF_SIZE)

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
                    if distancef > 600 or distancef < 10:  # 10cm - 20m
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
                    # print(distancef, end=",")
                    # print(anglef)
                    # print("角度 = ", angle)
                    # print("%%%")

        for i in range(len(angle)):
            angle[i] = angle[i] * (np.pi / 180)  # 极坐标

        x1, y1 = lidar.polar_to_cartesian(distance, angle)

        xmin = int(min(x1) + 1)
        ymin = int(min(y1) + 1)
        for i in range(len(x1)):
            x1[i] = x1[i]
            y1[i] = y1[i]

        xmax = int(max(x1) + 1)
        ymax = int(max(y1) + 1)
        print(xmax, ymax)
        if xmax < 100 or ymax < 300:
            continue

        mymap = [[0 for i in range(ymax)] for j in range(xmax)]

        num_rows = len(mymap)
        num_cols = len(mymap[0])

        # 不能走的画为1  定义地图  由于靠墙太近的地方是不能走的， 所以墙周围20个点之内的东西全部画为1
        for i in range(len(x1)):
            row = int(x1[i])
            col = int(y1[i])
            for i in range(max(0, row - 1), min(num_rows, row + 10)):
                for j in range(max(0, col - 1), min(num_cols, col + 10)):
                    mymap[i][j] = 1

        # print(mymap)
        # 定义可行动的方向（上、下、左、右和对角线）
        directions = [(-1, 0), (1, 0), (0, -1), (0, 1), (-1, -1), (-1, 1), (1, -1), (1, 1)]


        def heuristic(a, b):
            # 计算启发式函数（曼哈顿距离）
            return abs(b[0] - a[0]) + abs(b[1] - a[1])


        def astar(start, goal, map):
            # 初始化起点和终点
            start_node = (heuristic(start, goal), start, 0, None)
            goal_node = (heuristic(start, goal), goal, float('inf'), None)

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

                # 遍历可行动的方向
                for direction in directions:
                    next_pos = (current_pos[0] + direction[0], current_pos[1] + direction[1])

                    # 检查下一个节点是否越界或为障碍物
                    if next_pos[0] < 0 or next_pos[0] >= len(map) or next_pos[1] < 0 or next_pos[1] >= len(map[0]) or \
                            map[next_pos[0]][next_pos[1]] == 1:
                        continue

                    # 计算下一个节点的代价和预估总代价
                    cost = current_node[2] + 1
                    next_node = (cost + heuristic(next_pos, goal), next_pos, cost, current_node)

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

            # 如果开放列表为空，表示无法到达目标节点
            return []


        # 测试代码
        start = (0, 0)
        goal = (100, 300)
        path = astar(start, goal, mymap)
        # print(path)
        if path:
            print("找到了")
            # print("路径：", path)
        else:
            print("无法到达目标节点")

        xx = []
        yy = []
        ditux = []
        dituy = []
        for i in range(len(path)):
            xx.append(path[i][0])
            yy.append(path[i][1])

        plt.clf()  # 清图
        plt.scatter(x1, y1, s=1)
        plt.scatter(xx, yy, s=1)
        plt.xlabel('x')
        plt.ylabel('y')
        plt.title('Cartesian Coordinate System')
        plt.grid(True)
        plt.pause(0.01)  # 暂停一段时间，不然画的太快会卡住显示不出来

plt.ioff()  # 关闭画图窗口
plt.show()

#         x, y = lidar.polar_to_cartesian(distance, angle)
#
#         plt.clf()  # 清图
#         plt.polar(angle, distance, marker='o', ls='none', markersize=0.3, color='b')  # 画雷达图
#         plt.pause(0.01)  # 暂停一段时间，不然画的太快会卡住显示不出来
#
# plt.ioff()  # 关闭画图窗口
# plt.show()  # 显示
