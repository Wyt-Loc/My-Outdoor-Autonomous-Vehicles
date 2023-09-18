# -*- coding: utf-8 -*-
# @Time    : 2023/9/11 12:28
# @Author  : Wyt
# @File    : one.py
import time

import matplotlib
from multiprocessing import Process, Lock


class a:
    def task1(self, info):
        while True:
            print(info.get())
            time.sleep(1)
