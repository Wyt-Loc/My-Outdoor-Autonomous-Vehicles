# -*- coding: utf-8 -*-
# @Time    : 2023/9/11 12:28
# @Author  : Wyt
# @File    : two.py
import multiprocessing
import time
import one
from multiprocessing import Process, Lock, Pool

q = multiprocessing.Queue(100)


class b:
    def task2(self):
        while True:
            q.put(1)
            time.sleep(1)


if __name__ == '__main__':
    c = one.a()
    d = b()
    p1 = Process(target=c.task1, args=(q,))
    p1.start()
    p2 = Process(target=d.task2(), args=(q,))
    p2.start()
