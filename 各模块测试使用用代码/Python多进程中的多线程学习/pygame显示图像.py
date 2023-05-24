# -*- coding: utf-8 -*-
# @Time    : 2023/5/24 18:36
# @Author  : Wyt
# @File    : pygame显示图像.py

import pygame
from pygame.locals import *
import numpy as np
import cv2
import math
import sys
import os

pygame.init()
dir = os.path.dirname(sys.argv[0])  # 获取当前运行路径
# 路径
# music = os.path.join(dir, 'src/demo.mp3')
movie = os.path.join(dir, 'C:\\Users\\dell\\Videos\\Captures\\aaa.mp4')
# 窗口
# os.environ['SDL_VIDEO_WINDOW_POS'] = "%d,%d" % (0, 0)
size = width, height = 0, 0
screen = pygame.display.set_mode(size, flags=pygame.FULLSCREEN)
# 加载资源
videoCapture = cv2.VideoCapture(movie)
# pygame.mixer.music.load(music)
# 提前放一下，有时候第一遍会放不出来（不解）
# pygame.mixer.music.play()
pygame.mixer.music.pause()
# 帧率
FPS = 60
# Start Video
# pygame.mixer.music.play()
while 1:
    pygame.time.Clock().tick(FPS)
    ret, frame = videoCapture.read()
    if ret == True:  # 判断视频是否播放完毕
        frame = np.rot90(frame, k=-1)
        frame = pygame.surfarray.make_surface(frame)
        frame = pygame.transform.flip(frame, False, True)
        screen.blit(frame, (0, 0))
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                sys.exit()
        pygame.display.flip()
    else:
        break


