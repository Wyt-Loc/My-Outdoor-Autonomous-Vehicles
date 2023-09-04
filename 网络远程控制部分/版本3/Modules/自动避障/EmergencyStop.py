# -*- coding: utf-8 -*-
# @Time    : 2023/8/30 8:45
# @Author  : Wyt
# @File    : EmergencyStop.py

import 网络远程控制部分.版本3.Modules.Control.MyLidar


class EmergencyStop(网络远程控制部分.版本3.Modules.Control.MyLidar.Lidar):
    # 控制遇障碍急停

    def Getdata(self):
        # 得到雷达数据并赋值到 self.lidarAngle 和 self.lidarDistance
        self.parsingLidarData()

    def Judgment(self):
        for i in range(len(self.lidarAngle)):
            if 60 < self.lidarAngle[i] < 120 and self.lidarDistance[i] < 50:
                print("在角度为" + str(self.lidarAngle[i]) + "距离为" + str(self.lidarDistance[i]) + "存在障碍物")
                # 执行急停操作
                # self.sendMessages(self.dataStop())  # 发送数据帧
                # if self.flag == 1:
                #     self.isReceOk(self.dataStop())
                # self.MyprintLogInfos("DetailedTime", "执行急停操作")

    def run111(self):
        while True:
            self.Getdata()
            self.Judgment()


if __name__ == '__main__':
    em = EmergencyStop()
    em.run111()
