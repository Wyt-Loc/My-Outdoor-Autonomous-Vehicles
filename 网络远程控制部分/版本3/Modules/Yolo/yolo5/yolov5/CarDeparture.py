# -*- coding: utf-8 -*-
# @Time    : 2023/6/17 11:24
# @Author  : Wyt
# @File    : CarDeparture.py
import multiprocessing
from detect import *
from Lidar import *
import subprocess


def main1():
    process1 = subprocess.run(
        ['python', 'Lidar.py'],
        stdin=subprocess.PIPE, stdout=subprocess.PIPE
    )


def main2():
    opt = parse_opt()
    main(opt)


if __name__ == '__main__':
    p1 = multiprocessing.Process(target=main2, )
    p1.start()
    p1.join()
    p2 = multiprocessing.Process(target=main1, )
    p2.start()
    p2.join()
