# -*- coding: utf-8 -*-
# @Time    : 2023/4/18 20:45
# @Author  : Wyt
# @File    : 改名.py
import os

js = 45
while js <= 101:
    os.rename("F:/CreatCar/wyt_data/Python/OpenCV/number/nun/nun_{}.jpg".format(js),
              "F:/CreatCar/wyt_data/Python/OpenCV/number/nun/num_{}.jpg".format(js))

    # os.rename("yolov5/My_VOCData/images/num{}.jpg".format(js),
    #           "yolov5/My_VOCData/images/num_{}.jpg".format(js))

    js += 1
