# YOLOv5 🚀 by Ultralytics, AGPL-3.0 license
"""
Run YOLOv5 detection inference on images, videos, directories, globs, YouTube, webcam, streams, etc.

Usage - sources:
    $ python detect.py --weights yolov5s.pt --source 0                               # webcam
                                                     img.jpg                         # image
                                                     vid.mp4                         # video
                                                     screen                          # screenshot
                                                     path/                           # directory
                                                     list.txt                        # list of images
                                                     list.streams                    # list of streams
                                                     'path/*.jpg'                    # glob
                                                     'https://youtu.be/Zgi9g1ksQHc'  # YouTube
                                                     'rtsp://example.com/media.mp4'  # RTSP, RTMP, HTTP stream

Usage - formats:
    $ python detect.py --weights yolov5s.pt                 # PyTorch
                                 yolov5s.torchscript        # TorchScript
                                 yolov5s.onnx               # ONNX Runtime or OpenCV DNN with --dnn
                                 yolov5s_openvino_model     # OpenVINO
                                 yolov5s.engine             # TensorRT
                                 yolov5s.mlmodel            # CoreML (macOS-only)
                                 yolov5s_saved_model        # TensorFlow SavedModel
                                 yolov5s.pb                 # TensorFlow GraphDef
                                 yolov5s.tflite             # TensorFlow Lite
                                 yolov5s_edgetpu.tflite     # TensorFlow Edge TPU
                                 yolov5s_paddle_model       # PaddlePaddle
"""

import argparse
import multiprocessing
import os
import platform
import sys
import threading
import time
from pathlib import Path
import torch

import 网络远程控制部分.版本3.Modules.Control.MyControl
from models.common import DetectMultiBackend
from utils.dataloaders import IMG_FORMATS, VID_FORMATS, LoadImages, LoadScreenshots, LoadStreams
from utils.general import (LOGGER, Profile, check_file, check_img_size, check_imshow, check_requirements, colorstr, cv2,
                           increment_path, non_max_suppression, print_args, scale_boxes, strip_optimizer, xyxy2xywh)
from utils.plots import Annotator, colors, save_one_box
from utils.torch_utils import select_device, smart_inference_mode
import 网络远程控制部分.版本3.Modules.Control.MyLidar as Lidar
import heapq
import socket
import numpy as np
import matplotlib

matplotlib.use("Qt5Agg")
import matplotlib.pyplot as plt
import time
import threading
import serial.tools.list_ports

FILE = Path(__file__).resolve()
ROOT = FILE.parents[0]  # YOLOv5 root directory
if str(ROOT) not in sys.path:
    sys.path.append(str(ROOT))  # add ROOT to PATH
ROOT = Path(os.path.relpath(ROOT, Path.cwd()))  # relative

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

# 定义全局变量 没办法了，只能这样取值了
# 得到标签值
gettxt = []
px = 0
py = 0

lableIngo = multiprocessing.Queue(100)


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
                        # self.print("当前方向角为: ", zcar_jiao)
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


# 地址 192.168.0.100
# 端口 8487


class Lidar(网络远程控制部分.版本3.Modules.Control.MyControl.Mykey):
    lidarData = []
    lidarmap = []
    lidarDistance = []
    lidarAngle = []  # 原始角度
    lidarAngle_old = []
    lidarDis_old = []
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
        self.s.connect_ex(('192.168.0.100', 8487))
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
                    self.lidarDistance.append(distancef)
                    self.lidarDis_old.append(distancef)
                    if self.lidarData[i + 2] & 0x40:
                        self.lidarDistance[0] += 1
                    anglef = (self.lidarData[i + 2] & 0x3F)
                    anglef <<= 7
                    anglef += self.lidarData[i + 3] & 0x7F
                    anglef = anglef / 16
                    self.lidarAngle.append(anglef)
                    self.lidarAngle_old.append(anglef)
                    # print("方向 = ", anglef, "距离 = ", distancef)

    def generationMap(self):
        # 生成地图
        for i in range(len(self.lidarAngle)):
            self.lidarAngle[i] = self.lidarAngle[i] * (np.pi / 180)  # 极坐标
        self.x1, self.y1 = self.polar_to_cartesian(self.lidarDistance, self.lidarAngle)
        # for i in range(len(self.x1)):
        #     self.x1[i] = self.x1[i]
        #     self.y1[i] = self.y1[i]
        # xmax = int(max(self.x1) + 1)
        # ymax = int(max(self.y1) + 1)
        # print("xmax = ", xmax, "ymax =", ymax)
        # if xmax < 100 or ymax < 300:
        #     return
        # self.lidarmap = [[0 for _ in range(ymax)] for _ in range(xmax)]
        # num_rows = len(self.lidarmap)
        # num_cols = len(self.lidarmap[0])
        # for i in range(len(self.x1)):
        #     row = int(self.x1[i])
        #     col = int(self.y1[i])
        #     for t in range(max(0, row - 1), min(num_rows, row + 10)):
        #         for j in range(max(0, col - 1), min(num_cols, col + 10)):
        #             self.lidarmap[t][j] = 1
        # print(mymap)

    def findRoad(self, start, goal):
        self.path = self.astar(start, goal)
        if self.path:
            print("找到了")
            # print("路径：", self.path)
        else:
            print("无法到达目标节点")

    def displayImg(self, lableIngo):  # 参数为： 物体的坐标和标签
        plt.figure(figsize=(10.8, 9.6), dpi=100)  # 设置画布大小 1080 * 960 的画布
        plt.autoscale(enable=False, tight=False)
        plt.xlabel('x')
        plt.ylabel('y')
        # 寻路的坐标点，之后应该是一个栈来存储。
        # start1 = (0, 0)
        # goal1 = (100, 300)
        while True:
            xobj = []
            yobj = []
            obj = []
            lidar_loc = []
            showdatax = []
            showdatay = []
            roadx = []
            roady = []
            while lableIngo.qsize() != 0:
                txt, x, y = lableIngo.get()
                obj.append(txt)
                xobj.append(x)
                yobj.append(y)
                lidar_loc.append(x / 20)  # 一份20px宽,得到目标块号 [0 - 32]
            # print("x = ", xobj, "y = ", yobj)
            """
            采用网格法 将图像数据映射到雷达图像。
            """
            length = len(xobj)
            # 求取 对应的雷达坐标
            # 580 -- 850 对应为 89 -- 108° 对应px 为 250 -- 440
            # 所以 这三个椅子的位置 对应为lidar_loc 在 12 -- 22 之间的都为有效
            lidar_loc.sort()  # 得到对应的块号[0-32]排序。1对应范围为18 - 22°，2对应22°-38°, 3对应 38°-58°
            # print(lidar_loc)  # 6 9 18 19 21  50 - 140° 对应 0 - 640  20° = 7*20px  7倍的关系
            for i in range(length):  # 根据不同的块号显示不同区域的图像，从左到右
                if i < length - 1:
                    mina = lidar_loc[i] * 20 - 5  # 最小x坐标
                    maxa = lidar_loc[i] * 20 + 5  # 最大x坐标
                    # print("范围为", (mina, maxa))
                    len1 = len(self.lidarAngle_old)
                    print(len1)
                    for j in range(1, len1):
                        if self.lidarDis_old[j] < 50:
                            pass
                            # self.Control.Mykey.dataStop()  # 距离过近直接停车

                        if ((50 + zcar_jiao + mina) / 8) < (self.lidarAngle_old[j] + zcar_jiao) < (
                                (50 + zcar_jiao + maxa) / 8):  # 注意要找到图像坐标对应的雷达的角度信息
                            self.lidarAngle_old[j] = self.lidarAngle_old[j] * (np.pi / 180)  # 极坐标
                            x11, y11 = self.polar_to_cartesian(self.lidarDis_old[j], self.lidarAngle_old[j])
                            showdatax.append(x11)
                            showdatay.append(y11)

            # 加入九轴传感器使得图像对应的角度始终为车辆正前方
            valdata = (90 + zcar_jiao) * (np.pi / 180)  # 极坐标
            for i in range(50):
                x11, y11 = self.polar_to_cartesian(i * 20, valdata)
                roadx.append(x11)
                roady.append(y11)
            # print(showdatax, showdatay)
            # print(lidar_x, lidar_loc)
            self.parsingLidarData()  # 解析数据
            self.generationMap()  # 生成地图
            # self.findRoad(start1, goal1)  # 显示
            # xx = []
            # yy = []
            # print(len(self.path))
            # for i in range(len(self.path)):
            #     xx.append(self.path[i][0])
            #     yy.append(self.path[i][1])
            plt.clf()  # 清图
            plt.scatter(0, 0, s=40, marker="^")  # 原点
            plt.scatter(roadx, roady, s=40, marker="^")  # 90°路径
            plt.scatter(self.x1, self.y1, s=1)  # 地图
            plt.scatter(showdatax, showdatay, s=10, marker="o")  # 障碍物
            # for i in range(length):
            #     plt.text(showdatax[i], showdatay[i], obj[i])
            # plt.scatter(xx, yy, s=1)  # 路径
            plt.xlim((-1300, 1300))
            plt.ylim((-600, 1300))
            plt.title('Cartesian Coordinate System')
            plt.grid(True)
            plt.pause(0.001)  # 暂停一段时间，不然画的太快会卡住显示不出来


@smart_inference_mode()
def run(
        weights=ROOT / 'yolov5n.pt',  # model path or triton URL
        source=ROOT / 'data/images',  # file/dir/URL/glob/screen/0(webcam)
        data=ROOT / 'data/coco128.yaml',  # dataset.yaml path
        imgsz=(640, 640),  # inference size (height, width)
        conf_thres=0.25,  # confidence threshold
        iou_thres=0.45,  # NMS IOU threshold
        max_det=1000,  # maximum detections per image
        device='',  # cuda device, i.e. 0 or 0,1,2,3 or cpu
        view_img=False,  # show results
        save_txt=False,  # save results to *.txt
        save_conf=False,  # save confidences in --save-txt labels
        save_crop=False,  # save cropped prediction boxes
        nosave=False,  # do not save images/videos
        classes=None,  # filter by class: --class 0, or --class 0 2 3
        agnostic_nms=False,  # class-agnostic NMS
        augment=False,  # augmented inference
        visualize=False,  # visualize features
        update=False,  # update all models
        project=ROOT / 'runs/detect',  # save results to project/name
        name='exp',  # save results to project/name
        exist_ok=False,  # existing project/name ok, do not increment
        line_thickness=3,  # bounding box thickness (pixels)
        hide_labels=False,  # hide labels
        hide_conf=False,  # hide confidences
        half=False,  # use FP16 half-precision inference
        dnn=False,  # use OpenCV DNN for ONNX inference
        vid_stride=1,  # video frame-rate stride
):
    source = str(source)
    save_img = not nosave and not source.endswith('.txt')  # save inference images
    is_file = Path(source).suffix[1:] in (IMG_FORMATS + VID_FORMATS)
    is_url = source.lower().startswith(('rtsp://', 'rtmp://', 'http://', 'https://'))
    webcam = source.isnumeric() or source.endswith('.streams') or (is_url and not is_file)
    screenshot = source.lower().startswith('screen')
    if is_url and is_file:
        source = check_file(source)  # download

    # Directories
    save_dir = increment_path(Path(project) / name, exist_ok=exist_ok)  # increment run
    (save_dir / 'labels' if save_txt else save_dir).mkdir(parents=True, exist_ok=True)  # make dir

    # Load model
    device = select_device(device)
    model = DetectMultiBackend(weights, device=device, dnn=dnn, data=data, fp16=half)
    stride, names, pt = model.stride, model.names, model.pt
    imgsz = check_img_size(imgsz, s=stride)  # check image size

    # Dataloader
    bs = 1  # batch_size
    if webcam:
        view_img = check_imshow(warn=True)
        dataset = LoadStreams(source, img_size=imgsz, stride=stride, auto=pt, vid_stride=vid_stride)
        bs = len(dataset)
    elif screenshot:
        dataset = LoadScreenshots(source, img_size=imgsz, stride=stride, auto=pt)
    else:
        dataset = LoadImages(source, img_size=imgsz, stride=stride, auto=pt, vid_stride=vid_stride)
    vid_path, vid_writer = [None] * bs, [None] * bs

    # Run inference
    model.warmup(imgsz=(1 if pt or model.triton else bs, 3, *imgsz))  # warmup
    seen, windows, dt = 0, [], (Profile(), Profile(), Profile())
    for path, im, im0s, vid_cap, s in dataset:
        with dt[0]:
            im = torch.from_numpy(im).to(model.device)
            im = im.half() if model.fp16 else im.float()  # uint8 to fp16/32
            im /= 255  # 0 - 255 to 0.0 - 1.0
            if len(im.shape) == 3:
                im = im[None]  # expand for batch dim

        # Inference
        with dt[1]:
            visualize = increment_path(save_dir / Path(path).stem, mkdir=True) if visualize else False
            pred = model(im, augment=augment, visualize=visualize)

        # NMS
        with dt[2]:
            pred = non_max_suppression(pred, conf_thres, iou_thres, classes, agnostic_nms, max_det=max_det)

        # Second-stage classifier (optional)
        # pred = utils.general.apply_classifier(pred, classifier_model, im, im0s)

        # Process predictions
        for i, det in enumerate(pred):  # per image
            seen += 1
            if webcam:  # batch_size >= 1
                p, im0, frame = path[i], im0s[i].copy(), dataset.count
                s += f'{i}: '
            else:
                p, im0, frame = path, im0s.copy(), getattr(dataset, 'frame', 0)

            p = Path(p)  # to Path
            save_path = str(save_dir / p.name)  # im.jpg
            txt_path = str(save_dir / 'labels' / p.stem) + ('' if dataset.mode == 'image' else f'_{frame}')  # im.txt
            s += '%gx%g ' % im.shape[2:]  # print string
            gn = torch.tensor(im0.shape)[[1, 0, 1, 0]]  # normalization gain whwh
            imc = im0.copy() if save_crop else im0  # for save_crop

            annotator = Annotator(im0, line_width=line_thickness, example=str(names))
            if len(det):
                # Rescale boxes from img_size to im0 size
                det[:, :4] = scale_boxes(im.shape[2:], det[:, :4], im0.shape).round()

                # Print results
                for c in det[:, 5].unique():
                    n = (det[:, 5] == c).sum()  # detections per class
                    s += f"{n} {names[int(c)]}{'s' * (n > 1)}, "  # add to string

                # Write results
                for *xyxy, conf, cls in reversed(det):
                    if save_txt:  # Write to file
                        xywh = (xyxy2xywh(torch.tensor(xyxy).view(1, 4)) / gn).view(-1).tolist()  # normalized xywh
                        line = (cls, *xywh, conf) if save_conf else (cls, *xywh)  # label format
                        with open(f'{txt_path}.txt', 'a') as f:
                            f.write(('%g ' * len(line)).rstrip() % line + '\n')

                    if save_img or save_crop or view_img:  # Add bbox to image
                        c = int(cls)  # integer class
                        label = None if hide_labels else (names[c] if hide_conf else f'{names[c]} {conf:.2f}')
                        annotator.box_label(xyxy, label, color=colors(c, True))
                        if label == "person" or "persons":
                            p1, p2 = (int(xyxy[0]), int(xyxy[1])), (int(xyxy[2]), int(xyxy[3]))
                            p_x = (p2[0] - p1[0]) / 2 + p1[0]
                            p_y = (p2[1] - p1[1]) / 2 + p1[1]
                            # print("px = ", p_x)
                            if p_x <= 640:
                                lableIngo.put((label, p_x, p_y))

                        # 打印坐标信息
                        # print("中心点的坐标" + str(Center))
                    if save_crop:
                        save_one_box(xyxy, imc, file=save_dir / 'crops' / names[c] / f'{p.stem}.jpg', BGR=True)

            # Stream results
            im0 = annotator.result()
            if view_img:
                if platform.system() == 'Linux' and p not in windows:
                    windows.append(p)
                    cv2.namedWindow(str(p), cv2.WINDOW_NORMAL | cv2.WINDOW_KEEPRATIO)  # allow window resize (Linux)
                    cv2.resizeWindow(str(p), im0.shape[1], im0.shape[0])

                # print("画面显示")

                cv2.imshow("EyeHeaven", im0)
                cv2.waitKey(1)  # 1 millisecond

            # Save results (ismage with detection)
            if save_img:
                if dataset.mode == 'image':
                    cv2.imwrite(save_path, im0)
                else:  # 'video' or 'stream'
                    if vid_path[i] != save_path:  # new video
                        vid_path[i] = save_path
                        if isinstance(vid_writer[i], cv2.VideoWriter):
                            vid_writer[i].release()  # release previous video writer
                        if vid_cap:  # video
                            fps = vid_cap.get(cv2.CAP_PROP_FPS)
                            w = int(vid_cap.get(cv2.CAP_PROP_FRAME_WIDTH))
                            h = int(vid_cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
                        else:  # stream
                            fps, w, h = 30, im0.shape[1], im0.shape[0]
                        save_path = str(Path(save_path).with_suffix('.mp4'))  # force *.mp4 suffix on results videos
                        vid_writer[i] = cv2.VideoWriter(save_path, cv2.VideoWriter_fourcc(*'mp4v'), fps, (w, h))
                    vid_writer[i].write(im0)

        # Print time (inference-only)
        LOGGER.info(f"{s}{'' if len(det) else '(no detections), '}{dt[1].dt * 1E3:.1f}ms")

    # Print results
    t = tuple(x.t / seen * 1E3 for x in dt)  # speeds per image
    LOGGER.info(f'Speed: %.1fms pre-process, %.1fms inference, %.1fms NMS per image at shape {(1, 3, *imgsz)}' % t)
    if save_txt or save_img:
        s = f"\n{len(list(save_dir.glob('labels/*.txt')))} labels saved to {save_dir / 'labels'}" if save_txt else ''
        LOGGER.info(f"Results saved to {colorstr('bold', save_dir)}{s}")
    if update:
        strip_optimizer(weights[0])  # update model (to fix SourceChangeWarning)


#  default='runs/train/exp6/weights/best.pt',
# F:/CreatCar/wyt_data/Python/yolo/yolov5/runs/train/exp6/weights/best.pt

def parse_opt():
    parser = argparse.ArgumentParser()
    # parser.add_argument('--weights', nargs='+', type=str, default='runs/train/exp6/weights/best.pt',
    #                     help='model path or triton URL')
    parser.add_argument('--weights', nargs='+', type=str, default='yolov5n.pt',
                        help='model path or triton URL')
    parser.add_argument('--source', type=str, default=0, help='source')
    parser.add_argument('--data', type=str, default=ROOT / 'data/coco128.yaml', help='(optional) dataset.yaml path')
    parser.add_argument('--imgsz', '--img', '--img-size', nargs='+', type=int, default=[1280, 640],
                        help='inference size h,w')
    parser.add_argument('--conf-thres', type=float, default=0.25, help='confidence threshold')
    parser.add_argument('--iou-thres', type=float, default=0.45, help='NMS IoU threshold')
    parser.add_argument('--max-det', type=int, default=1000, help='maximum detections per image')
    parser.add_argument('--device', default='', help='cuda device, i.e. 0 or 0,1,2,3 or cpu')
    parser.add_argument('--view-img', action='store_true', help='show results')
    parser.add_argument('--save-txt', action='store_true', help='save results to *.txt')
    parser.add_argument('--save-conf', action='store_true', help='save confidences in --save-txt labels')
    parser.add_argument('--save-crop', action='store_true', help='save cropped prediction boxes')
    parser.add_argument('--nosave', action='store_true', help='do not save images/videos')
    parser.add_argument('--classes', nargs='+', type=int, help='filter by class: --classes 0, or --classes 0 2 3')
    parser.add_argument('--agnostic-nms', action='store_true', help='class-agnostic NMS')
    parser.add_argument('--augment', action='store_true', help='augmented inference')
    parser.add_argument('--visualize', action='store_true', help='visualize features')
    parser.add_argument('--update', action='store_true', help='update all models')
    parser.add_argument('--project', default=ROOT / 'runs/detect', help='save results to project/name')
    parser.add_argument('--name', default='exp', help='save results to project/name')
    parser.add_argument('--exist-ok', action='store_true', help='existing project/name ok, do not increment')
    parser.add_argument('--line-thickness', default=3, type=int, help='bounding box thickness (pixels)')
    parser.add_argument('--hide-labels', default=False, action='store_true', help='hide labels')
    parser.add_argument('--hide-conf', default=False, action='store_true', help='hide confidences')
    parser.add_argument('--half', action='store_true', help='use FP16 half-precision inference')
    parser.add_argument('--dnn', action='store_true', help='use OpenCV DNN for ONNX inference')
    parser.add_argument('--vid-stride', type=int, default=1, help='video frame-rate stride')
    opt = parser.parse_args()
    opt.imgsz *= 2 if len(opt.imgsz) == 1 else 1  # expand
    print_args(vars(opt))
    return opt


# class YoloAndLidar(网络远程控制部分.版本3.Modules.自动避障.EmergencyStop.EmergencyStop):
#     pass


def main(opt):
    check_requirements(exclude=('tensorboard', 'thop'))
    run(**vars(opt))


def yolo_start():
    opt = parse_opt()
    main(opt)


"""
思路 :
 在摄像头图像的某个范围内 雷达是可以计算出物体的距离，但是在图像上方或者下方，是没法得到具体距离的
 对于每个2D边界框，图像检测提供类别。对于每个LiDAR投影点，我们有准确的距离。
 细节处理 1. 我们得到的边界框一般都会比实际物体大，
        解决方案 1. 得到中心点去向周围动态扩散指定大小，简单，但是对于太小的物体会使其范围变大。
        解决方案 2. 采用图像分割。
融合思路：
    识别出位置，且雷达去重点测量这个地方周围的距离，最后入栈，
    
将双目摄像头数据在图像中标注出来。地图标注
    思路： 雷达和图像分为两个进程去处理， 在这要加入进程同步。确保图像和雷达数据是一组。
    之后通过 多进程 管道， 将图像得到的 物体信息大概位置传递到 雷达进程，
    之后雷达进程去处理这个信息，之后显示在雷达图像中，做出标识。
    之后在引入A*寻路。
    
    测试得到 图像坐标在(270,270),(450,400) 范围内
    大致对应于雷达正中的位置
"""


class DataMerge:
    # 数据融合展示
    def dataMergeDisplay(self):
        t3 = myThread_zitai("姿态", True)
        t3.start()
        t1 = threading.Thread(target=yolo_start)  # 实例化进程对象
        t1.start()
        p2 = multiprocessing.Process(target=Lidar().displayImg(lableIngo))
        p2.start()


if __name__ == '__main__':
    a = DataMerge()
    a.dataMergeDisplay()
