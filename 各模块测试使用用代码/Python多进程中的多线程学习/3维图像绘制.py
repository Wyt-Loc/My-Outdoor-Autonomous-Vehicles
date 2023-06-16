# -*- coding: utf-8 -*-
# @Time    : 2023/6/16 16:32
# @Author  : Wyt
# @File    : 3维图像绘制.py
from mayavi import mlab

x = [[-1, 1, 1, -1, -1], [-1, 1, 1, -1, -1]]
y = [[-2, -2, -2, -2, -2], [2, 2, 2, 2, 2]]
z = [[3, 3, -3, -3, 3], [3, 3, -3, -3, 3]]
s = mlab.mesh(x, y, z)
mlab.axes(xlabel='x', ylabel='y', zlabel='z')
mlab.outline(s)
mlab.show()
