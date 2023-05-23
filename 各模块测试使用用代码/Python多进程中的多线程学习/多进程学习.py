# -*- coding: utf-8 -*-
# @Time    : 2023/5/20 21:28
# @Author  : Wyt
# @File    : 多进程学习.py

import time
from multiprocessing import Process


def fun1(name):
    while True:
        time.sleep(1)
        print('测试%s多进程' % name)


if __name__ == '__main__':
    process_list = []  # 进程池
    for i in range(5):  # 开启5个子进程执行fun1函数
        p = Process(target=fun1, args=(i,))  # 实例化进程对象
        p.start()
        process_list.append(p)

    for i in process_list:
        p.join()

    print('结束测试')
