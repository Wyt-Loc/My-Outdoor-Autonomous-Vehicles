# -*- coding: utf-8 -*-
# @Time    : 2023/7/7 21:57
# @Author  : Wyt
# @File    : 解析x,y类型数据.py
import math

from matplotlib import pyplot as plt

data = []
dis = []
angle = []

filename = ''

# 打开文本文件
with open(filename, 'r') as file:
    # 逐行读取数据
    for i in file:
        a = file.readline().split()
        data.append(str(a).split(','))

for i in range(len(data)):
    try:
        a = str(data[i][0]).lstrip("['")
        b = str(data[i][1]).rstrip("']")
        dis.append(a)
        angle.append(b)
    except:
        continue
x = []
y = []
# 转为直角坐标系
for i in range(len(data)):
    x.append(float(dis[i]) * math.cos(float(angle[i])))
    y.append(float(dis[i]) * math.sin(float(angle[i])))
print(x)
print(y)

plt.clf()  # 清图
plt.plot(x, y, marker='*', linestyle='None')
plt.pause(0.01)  # 暂停一段时间，不然画的太快会卡住显示不出来
# plt.ioff()  # 关闭画图窗口
plt.show()  # 显示
