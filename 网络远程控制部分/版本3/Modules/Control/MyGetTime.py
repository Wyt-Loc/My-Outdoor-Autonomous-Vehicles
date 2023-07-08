# -*- coding: utf-8 -*-
# @Time    : 2023/6/30 12:20
# @Author  : Wyt
# @File    : MyGetTime.py
import datetime
import time


class GetTimeInfo:
    """
    得到时间类
    1. 主要是几个时间
        1.年月日时分秒毫 详细的时间
        2.程序运行的时间，从开始运行到当前时刻的时间，最终结果换算为毫秒
        为了方便打log 返回值一律采用 str 类型
    """
    detailedTime = ""
    runningTime = ""
    startTime = ""
    stopTime = ""

    def __init__(self) -> None:
        # 得到服务开启时的时间
        self.startTime = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')
        with open("data.txt", "a", encoding="utf-8") as f:
            f.writelines("In " + self.startTime + " TcpServer start Running\n")

    def __del__(self) -> None:
        # 得到服务关闭时的时间
        self.stopTime = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')
        print(self.stopTime)

    def getDetailedTime(self) -> str:
        # 得到详细时间
        self.detailedTime = str(datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f'))
        return self.detailedTime

    @staticmethod
    def getRunningTime() -> str:
        # 得到当前时间
        curtime = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')
        return curtime

    def getTimeDiff(self) -> str:
        # 得到相对于开始时间的时间差
        runtime = datetime.datetime.strptime(self.getRunningTime(), "%Y-%m-%d %H:%M:%S.%f")
        starttime = datetime.datetime.strptime(self.startTime, "%Y-%m-%d %H:%M:%S.%f")
        diffTime = str(runtime - starttime)
        return diffTime

    def testLog(self):
        time.sleep(5)
        runtime = datetime.datetime.strptime(self.getRunningTime(), "%Y-%m-%d %H:%M:%S.%f")
        starttime = datetime.datetime.strptime(self.startTime, "%Y-%m-%d %H:%M:%S.%f")
        diffTime = str(runtime - starttime)
        return "程序运行时间为 " + diffTime
