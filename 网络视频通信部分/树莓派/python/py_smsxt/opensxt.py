import cv2


# 定义摄像头
cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)  # 加上后边这个打开快多了
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)   # 图像分割
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)

js = 102
while True:
    ret, frame = cap.read()
    # 读取每一帧
    cv2.imshow('shuai ge', frame)
    # 显示每一帧
    c = cv2.waitKey(1)  # 判断退出的条件 当按下'Q'键的时候呢，就退出

    if c == ord('s'):
        cv2.imwrite("./DLIB/number{}.jpg".format(js), frame)  # 自己设置拍摄的照片的存储位置
        js += 1
        print(js)

    if c == ord('q'):
        break


cap.release()   # 释放摄像头
cv2.destroyAllWindows()  # 销毁窗口
