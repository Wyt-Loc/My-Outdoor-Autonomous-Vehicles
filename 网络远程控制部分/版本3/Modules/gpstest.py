# -*- coding: utf-8 -*-
# @Time    : 2023/8/18 23:39
# @Author  : Wyt
# @File    : gpstest.py
# @Description : 这个文件是用来

import serial.tools.list_ports


class Test:
    timeflag = 0  # 时间戳标识

    # 串口检测
    def usartFind(self):
        pass

    def receData(self):
        portx = "COM11"  # COM2口用来读数
        bps = 921600
        # 设置并打开串口
        ser = serial.Serial(portx, int(bps), timeout=1, parity=serial.PARITY_NONE, stopbits=1)
        while True:
            line = ser.read(80)  # 递归读取串口
            if line[0] == 89 and line[1] == 83:
                print("TID = ", line[2], line[3])
                # print("LEN = ", line[4])
                # 16 个一组
                if line[4] == 28:
                    # print("ok")
                    pass


if __name__ == '__main__':
    t = Test()
    t.usartFind()
    t.receData()
