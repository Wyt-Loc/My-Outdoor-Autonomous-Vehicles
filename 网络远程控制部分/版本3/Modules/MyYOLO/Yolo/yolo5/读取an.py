# -*- coding: utf-8 -*-
# @Time    : 2023/4/22 14:29
# @Author  : Wyt
# @File    : 读取an.py

import torch
import attempt_load

model = attempt_load('./weights/best.pt', map_location=torch.device('cpu'))
m = model.module.model[-1] if hasattr(model, 'module') else model.model[-1]
print(m.anchor_grid)
