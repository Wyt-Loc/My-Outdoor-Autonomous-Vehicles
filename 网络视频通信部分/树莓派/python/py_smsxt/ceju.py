# -*- coding: utf-8 -*-
# @Time    : 2023/4/15 14:12
# @Author  : Wyt
# @File    : 测距.py

import cv2
import camera_configs
import math

# 定义摄像头
cap = cv2.VideoCapture(0)  # 加上后边这个打开快多了
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)   # 图像分割
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)


# 由于变成了一个大镜头，所以不用这种方式了，改用分割镜头的方法
# cap1 = cv2.VideoCapture(1 + cv2.CAP_DSHOW)
# cv2.VideoCapture.set(cap1, 3, 640)  # 这里Property identifier要求是int
# cv2.VideoCapture.set(cap1, 4, 640)  # 见下图,3表示CV_CAP_PROP_FRAME_WIDTH,设为270.（设为1和270的效果一样）
# # 注意顺序，先设置分辨率，再实例化另一个摄像头。否则依旧报错。
# cap2 = cv2.VideoCapture(2 + cv2.CAP_DSHOW)
# cv2.VideoCapture.set(cap2, 3, 640)
# cv2.VideoCapture.set(cap2, 4, 640)


# 鼠标回调函数
def onmouse_pick_points(event, x, y, flags, param):
    if event == cv2.EVENT_LBUTTONDOWN:
        threeD = param
        print('\n像素坐标 x = %d, y = %d' % (x, y))
        # print("世界坐标是：", threeD[y][x][0], threeD[y][x][1], threeD[y][x][2], "mm")
        print("世界坐标xyz 是：", threeD[y][x][0] / 1000.0, threeD[y][x][1] / 1000.0, threeD[y][x][2] / 1000.0, "m")

        distance = math.sqrt(threeD[y][x][0] ** 2 + threeD[y][x][1] ** 2 + threeD[y][x][2] ** 2)
        distance = distance / 1000.0  # mm -> m
        print("距离是：", distance, "m")


WIN_NAME = 'Deep'
cv2.namedWindow(WIN_NAME, cv2.WINDOW_AUTOSIZE)


while True:
    ret1, frame = cap.read()
    frame1 = frame[:, :int(1280 / 2), :]
    frame2 = frame[:, int(1280 / 2):, :]
    imgL = cv2.cvtColor(frame1, cv2.COLOR_BGR2GRAY)  # 将BGR格式转换成灰度图片
    imgR = cv2.cvtColor(frame2, cv2.COLOR_BGR2GRAY)

    # cv2.remap 重映射，就是把一幅图像中某位置的像素放置到另一个图片指定位置的过程。
    # 依据MATLAB测量数据重建无畸变图片
    img1_rectified = cv2.remap(imgL, camera_configs.left_map1, camera_configs.left_map2, cv2.INTER_LINEAR)
    img2_rectified = cv2.remap(imgR, camera_configs.right_map1, camera_configs.right_map2, cv2.INTER_LINEAR)

    imageL = cv2.cvtColor(img1_rectified, cv2.COLOR_GRAY2BGR)
    imageR = cv2.cvtColor(img2_rectified, cv2.COLOR_GRAY2BGR)

    # BM
    numberOfDisparities = ((640 // 8) + 15) & -16  # 640对应是分辨率的宽

    stereo = cv2.StereoBM_create(numDisparities=16, blockSize=9)  # 立体匹配
    stereo.setROI1(camera_configs.validPixROI1)
    stereo.setROI2(camera_configs.validPixROI2)
    stereo.setPreFilterCap(31)
    stereo.setBlockSize(15)
    stereo.setMinDisparity(0)
    stereo.setNumDisparities(numberOfDisparities)
    stereo.setTextureThreshold(10)
    stereo.setUniquenessRatio(15)
    stereo.setSpeckleWindowSize(100)
    stereo.setSpeckleRange(32)
    stereo.setDisp12MaxDiff(1)

    disparity = stereo.compute(img1_rectified, img2_rectified)  # 计算视差

    disp = cv2.normalize(disparity, disparity, alpha=0, beta=255, norm_type=cv2.NORM_MINMAX, dtype=cv2.CV_8U)  # 归一化函数算法

    threeD = cv2.reprojectImageTo3D(disparity, camera_configs.Q, handleMissingValues=True)  # 计算三维坐标数据值
    threeD = threeD * 16

    # threeD[y][x] x:0~640; y:0~480;   !!!!!!!!!!
    cv2.setMouseCallback(WIN_NAME, onmouse_pick_points, threeD)

    cv2.imshow("left", frame1)
    # cv2.imshow("right", frame2)
    # cv2.imshow("left_r", imgL)
    # cv2.imshow("right_r", imgR)
    cv2.imshow(WIN_NAME, disp)  # 显示深度图的双目画面

    key = cv2.waitKey(1)

    if key == ord("q"):
        break

cap.release()
cv2.destroyAllWindows()
