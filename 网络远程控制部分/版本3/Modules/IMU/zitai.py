# -*- coding: utf-8 -*-
# @Time    : 2023/8/18 23:52
# @Author  : Wyt
# @File    : zitai.py
# @Description : 这个文件是用来


import time
import threading
import serial.tools.list_ports

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


# 姿态
class MyThreadZitai(threading.Thread):
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
                        zcar_jiao = zc + 0.00012  # 也可采用程序标定，静止防止一定时间，终值除以次数，以后这个数值不准确了再用
                        self.lastxj = self.jiuzhou_data[i][3]
                        self.lastyj = self.jiuzhou_data[i][4]
                        self.lastzj = self.jiuzhou_data[i][5]
                        # self.print("当前方向角为: ", xcar_jiao, ycar_jiao, zcar_jiao)
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
        if byte16 & 0x80000000 == 0:
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
                    if data[i + 5] == 0x10 and data[i + 6] == self.LENJIA \
                            and data[i + 19] == 0x20 and data[i + 20] == self.LENJIAO:
                        # 后面的12字节是加速度值、后面的12字节是角速度值
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
            self.ser = serial.Serial("COM11", 921600)  # 位置1
            self.ser.flushInput()  # 位置2
            self.print("串口初始化完成")
        except ValueError:
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
                        recv = self.jiexi(recv)  # 解析数据
                        self.updata_weizhi()  # 将所有得到的数据进行计算，得到位置信息到全局变量中
                        print("当前方向角为: ", xcar_jiao, ycar_jiao, zcar_jiao)
                    except BaseException as e:
                        pass
                        self.print("解析错误:", e)
        self.print("线程退出")


if __name__ == '__main__':
    zit = MyThreadZitai("姿态传感器", True)
    zit.start()
    zit.join(timeout=1)  # 守护线程# 主程序一秒后结束
