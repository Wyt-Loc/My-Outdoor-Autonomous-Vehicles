# -*- coding: utf-8 -*-
# @Time    : 2023/9/3 21:28
# @Author  : Wyt
# @File    : paizhao.py
import cv2
import camera_configs

cap = cv2.VideoCapture(1, cv2.CAP_DSHOW)
cap.set(3, 1280)
cap.set(4, 480)  # 打开并设置摄像头

cv2.namedWindow("pic", cv2.WINDOW_AUTOSIZE)

while True:
    ret, frame = cap.read()
    frame1 = frame[0:480, 0:640]
    frame2 = frame[0:480, 640:1280]  # 割开双目图像

    # 依据MATLAB测量数据重建无畸变图片
    img1_rectified = cv2.remap(frame1, camera_configs.left_map1, camera_configs.left_map2, cv2.INTER_LINEAR)
    img2_rectified = cv2.remap(frame2, camera_configs.right_map1, camera_configs.right_map2, cv2.INTER_LINEAR)

    cv2.imshow("left", frame1)
    cv2.imshow("right", frame2)

    key = cv2.waitKey(1)
    if key == ord("q"):
        break

cap.release()
cv2.destroyAllWindows()
