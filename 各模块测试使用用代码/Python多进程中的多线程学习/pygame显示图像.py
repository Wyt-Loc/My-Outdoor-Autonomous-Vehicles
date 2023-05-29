import pygame
from pygame.locals import *
import cv2
import sys
import time

video_path = r'C:\Users\dell\Videos\Captures\aaa.mp4'
video = cv2.VideoCapture(video_path)

FPS = int(round(video.get(cv2.CAP_PROP_FPS)))

FramePerSec = pygame.time.Clock()

Width = 640
Height = 480

pygame.init()
pygame.display.set_caption('OpenCV Video Player on Pygame')

screen = pygame.display.set_mode((Width, Height), 0, 32)
screen.fill([0, 0, 0])
num = 0

while True:

    if num == 0:
        T0 = time.time()

    if time.time() - T0 > num * (1. / FPS):

        ret, frame = video.read()
        TimeStamp = video.get(cv2.CAP_PROP_POS_MSEC)

        if ret == False:
            print('Total Time:', time.time() - T0)
            pygame.quit()
            sys.exit()

        frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

        frame = cv2.transpose(frame)  # 矩阵转换
        frame = pygame.surfarray.make_surface(frame)
        screen.blit(frame, (0, 0))
        pygame.display.update()

        num += 1

    for event in pygame.event.get():
        if event.type == QUIT:
            sys.exit()
