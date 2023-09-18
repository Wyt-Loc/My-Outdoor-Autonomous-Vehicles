# -*- coding: utf-8 -*-
# @Time    : 2023/6/8 21:23
# @Author  : Wyt
# @File    : SGBM算法.py

import cv2 as cv2

# 定义摄像头
capture = cv2.VideoCapture(0, cv2.CAP_DSHOW)  # 加上后边这个打开快多了
capture.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)  # 图像分割
capture.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
x1 = 0
y1 = 0
x2 = 640
y2 = 480
# 读取视频
ret, frame = capture.read()
while ret:
    # 是否读取到了帧，读取到了则为True
    ret, frame = capture.read()
    # 切割为左右两张图片
    frame1 = frame[0:480, 0:640]
    frame2 = frame[0:480, 640:1280]
    # cv2.rectangle(frame1, (x1, y1), (x2, y2), (0, 0, 255), 2)
    # cv2.rectangle(frame2, (0, 0), (640, 480), (0, 0, 255), 2)
    # 完成计时，计算帧率
    cv2.imshow("left", frame1)
    # cv2.imshow("right", frame2)
    # if cv2.waitKey(20) & 0xff == ord('w'):
    #     y2 -= 10
    # if cv2.waitKey(20) & 0xff == ord('a'):
    #     x2 -= 10
    # if cv2.waitKey(20) & 0xff == ord('s'):
    #     y1 += 10
    # if cv2.waitKey(20) & 0xff == ord('d'):
    #     x1 += 10
    # 若键盘按下q则退出播放
    # print(x1, y1, x2, y2)
    if cv2.waitKey(20) & 0xff == ord('q'):
        break

# 释放资源
capture.release()

# 关闭所有窗口
cv2.destroyAllWindows()
