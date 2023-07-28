# -*- coding: utf-8 -*-
# @Time    : 2023/6/30 12:19
# @Author  : Wyt
# @File    : MyprintLog.py

import MyGetTime


class PrintLog(MyGetTime.GetTimeInfo):
    """
    打印Log信息类
    """

    def __init__(self):
        # 记录什么时间开始运行
        super().__init__()  # 不加 提示警告， 加了 也没啥影响， 就是要 init 2次
        with open("data.txt", "a", encoding="utf-8") as f:
            f.writelines("In " + self.getDetailedTime() + " start Runing" + "\n")

    def MyprintDetailedTime(self, level: str):
        # 打印日志 -- 详细时间
        with open("data.txt", "a", encoding="utf-8") as f:
            f.writelines("level:" + level + " In DetailedTime " +
                         self.getDetailedTime() + " start Runing" + "\n")

    def MyprintRunTime(self, level: str):
        # 打印日志 -- 运行时间
        with open("data.txt", "a", encoding="utf-8") as f:
            f.writelines("level:" + level + " In RunningTime " +
                         self.getRunningTime() + " start Runing" + "\n")

    def MyprintLogWarnings(self, timeyypes: str):
        # 打印日志信息 -- 级别警告
        if timeyypes == "DetailedTime":
            self.MyprintDetailedTime("Warning")
        elif timeyypes == "RunningTime":
            self.MyprintRunTime("Warning")

    def MyprintLogInfos(self, timeyypes: str):
        # 打印日志信息 -- 级别日常信息
        if timeyypes == "DetailedTime":
            self.MyprintDetailedTime("Info")
        elif timeyypes == "RunningTime":
            self.MyprintRunTime("Info")

    def MyprintLogErrors(self, timeyypes: str):
        # 打印日志信息 -- 级别错误异常
        if timeyypes == "DetailedTime":
            self.MyprintDetailedTime("Error")
        elif timeyypes == "RunningTime":
            self.MyprintRunTime("Error")

    def MyprintLogClient(self, client: tuple):
        # 打印日志信息 -- 级别错误异常
        with open("data.txt", "a", encoding="utf-8") as f:
            f.writelines("In DetailedTime " + self.getDetailedTime() + str(client) +
                         " Connection" + "\n")

    def printReceData(self, data: str, client: str):
        # 打印信息 -- 接收到客户端的数据 ：在什么时间接收到谁的什么数据
        with open("data.txt", "a", encoding="utf-8") as f:
            f.writelines("In " + self.getDetailedTime() + "time " + "receiverd" + client +
                         " data is " + data + "\n")

    def printSendData(self, data: str, client: str):
        # 打印信息 -- 接收到客户端的数据 ：在什么时间接收到谁的什么数据
        with open("data.txt", "a", encoding="utf-8") as f:
            f.writelines("In " + self.getDetailedTime() + "time " + "send" + client +
                         " data is " + data + "\n")
