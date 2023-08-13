# -*- coding: utf-8 -*-
# @Time    : 2023/8/13 14:48
# @Author  : Wyt
# @File    : 自动驾驶L1级实现.py
import Modules.Control.MyprintLog
import Modules.Control.MyLidar


class L1LEVEL(Modules.Control.MyprintLog.PrintLog, Modules.Control.MyLidar):
    flag = 1

    def getCarDistance(self):
        # 得到车距
        dis = 1
        if dis > 50:
            self.flag = 0
        elif dis < 50:
            self.flag = 1

    def JudgmentHaveCar(self):
        # 判断前方是否有车
        if self.flag == 1:
            print("前方有车")
            return 1
        if self.flag == 0:
            print("前方没车")
            return 0

    # L1级别自动驾驶
    # ACC自适应巡航 启动条件速度大于某阙值自动介入
    def myAcc(self):
        print("ACC启动")
        self.MyprintLogInfos("DetailedTime", "ACC自动介入")
        # 分为2种情况 1.前方没车
        if self.JudgmentHaveCar() == 0:
            self.MyprintLogInfos("DetailedTime", "前方没车，自适应巡航")
        # 2.前方有车
        elif self.JudgmentHaveCar() == 1:
            self.MyprintLogInfos("DetailedTime", "前方有车，开启跟车巡航")


if __name__ == '__main__':
    l1 = L1LEVEL()
    l1.myAcc()
