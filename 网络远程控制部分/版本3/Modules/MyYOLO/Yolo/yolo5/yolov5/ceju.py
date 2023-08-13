# -*- coding: utf-8 -*-

import cv2
import numpy as np
import camera_configs

cap = cv2.VideoCapture(1, cv2.CAP_DSHOW)
cap.set(3, 1280)
cap.set(4, 480)  # 打开并设置摄像头


# 鼠标回调函数
def onmouse_pick_points(event, x, y, flags, param):
    if event == cv2.EVENT_LBUTTONDOWN:
        threeD = param
        print('\n像素坐标 x = %d, y = %d' % (x, y))
        # print("世界坐标是：", threeD[y][x][0], threeD[y][x][1], threeD[y][x][2], "mm")
        print("世界坐标xyz 是：", threeD[y][x][0] / 1000.0, threeD[y][x][1] / 1000.0, threeD[y][x][2] / 1000.0, "m")

        mythreeDx = []
        mythreeDy = []
        mythreeDz = []
        # 求threeD   如果大于 40% 我们就将这一块全部填充为 我们想要的那个深度值
        x1 = x - 30
        x2 = x + 30
        y1 = y - 30
        y2 = y + 30
        for i in range(x1, x2):
            for j in range(y1, y2):
                # 得到每一个点的 x y 和 深度信息
                if threeD[j][i][2] < 10000:
                    mythreeDx.append(threeD[j][i][0])
                    mythreeDy.append(threeD[j][i][1])
                    mythreeDz.append(threeD[j][i][2])
                    # print("kaishi")
        z = np.mean(np.unique(mythreeDz))
        print(z)  # 输出Nan 则为 这次没有测得距离

        # distance = math.sqrt(threeD[y][x][0] ** 2 + threeD[y][x][1] ** 2 + threeD[y][x][2] ** 2)
        # distance = distance / 1000.0  # mm -> m
        # print("距离是：", distance, "m")


WIN_NAME = 'Deep disp'
cv2.namedWindow(WIN_NAME, cv2.WINDOW_AUTOSIZE)

while True:
    ret, frame = cap.read()
    frame1 = frame[0:480, 0:640]
    frame2 = frame[0:480, 640:1280]  # 割开双目图像

    imgL = cv2.cvtColor(frame1, cv2.COLOR_BGR2GRAY)  # 将BGR格式转换成灰度图片
    imgR = cv2.cvtColor(frame2, cv2.COLOR_BGR2GRAY)

    # cv2.remap 重映射，就是把一幅图像中某位置的像素放置到另一个图片指定位置的过程。
    # 依据MATLAB测量数据重建无畸变图片
    img1_rectified = cv2.remap(imgL, camera_configs.left_map1, camera_configs.left_map2, cv2.INTER_LINEAR)
    img2_rectified = cv2.remap(imgR, camera_configs.right_map1, camera_configs.right_map2, cv2.INTER_LINEAR)

    # imageL = cv2.cvtColor(img1_rectified, cv2.COLOR_GRAY2BGR)
    # imageR = cv2.cvtColor(img2_rectified, cv2.COLOR_GRAY2BGR)

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

    disp = cv2.normalize(disparity, disparity, alpha=0, beta=255, norm_type=cv2.NORM_MINMAX,
                         dtype=cv2.CV_8U)  # 归一化函数算法

    # 得到坐标信息 xyz 的坐标
    threeD = cv2.reprojectImageTo3D(disparity, camera_configs.Q, handleMissingValues=True)  # 计算三维坐标数据值
    threeD = threeD * 16

    # x = 100
    # y = 100
    # distance = math.sqrt(threeD[y][x][0] ** 2 + threeD[y][x][1] ** 2 + threeD[y][x][2] ** 2)
    # distance = distance / 1000.0  # mm -> m
    # print("距离是：", distance, "m")

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
