# -*- coding: utf-8 -*-
# @Time    : 2023/6/15 18:39
# @Author  : Wyt
# @File    : 雷达图.py

import matplotlib.pyplot as plt
import numpy as np

plt.figure(figsize=(10.8, 9.6), dpi=100)  # 设置画布大小 1080 * 960 的画布
ax = plt.gca(projection='polar')
ax.set_thetagrids(np.arange(0.0, 360.0, 15.0))
ax.set_thetamin(0.0)  # 设置极坐标图开始角度为0°
ax.set_thetamax(360.0)  # 设置极坐标结束角度为360°

r = 140.0
jiaodu = 120.0

plt.clf()  # 清图。
plt.polar(jiaodu * (np.pi / 180), r, marker='o', markersize=5, color='b')  # 画雷达图
plt.show()  # 显示
