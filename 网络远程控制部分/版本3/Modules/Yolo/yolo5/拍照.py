# -*- coding: utf-8 -*-
# @Time    : 2023/4/13 21:23
# @Author  : Wyt
# @File    : 单击拍照.py

import cv2

js = 1

cap1 = cv2.VideoCapture(0 + cv2.CAP_DSHOW)
cv2.VideoCapture.set(cap1, 3, 640)  # 这里Property identifier要求是int
cv2.VideoCapture.set(cap1, 4, 640)  # 见下图,3表示CV_CAP_PROP_FRAME_WIDTH,设为270.（设为1和270的效果一样）
# 注意顺序，先设置分辨率，再实例化另一个摄像头。否则依旧报错。

cap2 = cv2.VideoCapture(1 + cv2.CAP_DSHOW)
cv2.VideoCapture.set(cap2, 3, 640)
cv2.VideoCapture.set(cap2, 4, 640)

ret1, frame1 = cap1.read()
ret2, frame2 = cap2.read()
while ret1 and ret2:
    ret1, frame1 = cap1.read()
    ret2, frame2 = cap2.read()
    cv2.imshow('frame1', frame1)
    cv2.imshow('frame2', frame2)
    key = cv2.waitKey(1)

    if key == ord('s'):
        cv2.imwrite("./number/nun1/num_{}.jpg".format(js), frame1)  # 自己设置拍摄的照片的存储位置
        # cv2.imwrite("right0.bmp", frame2)  # 自己设置拍摄的照片的存储位置
        js += 1

    if key == ord("q"):
        break

cap1.release()
cap2.release()
cv2.destroyAllWindows()
