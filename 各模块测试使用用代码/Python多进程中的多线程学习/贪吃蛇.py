# -*- coding: utf-8 -*-
# @Time    : 2023/5/22 17:41
# @Author  : Wyt
# @File    : 贪吃蛇.py

import pygame
import random

# 初始化pygame库
pygame.init()

# 定义一些常量
WIDTH = 500
HEIGHT = 500
CELL_SIZE = 20
CELL_WIDTH = WIDTH // CELL_SIZE
CELL_HEIGHT = HEIGHT // CELL_SIZE
FONT_SIZE = 36
BG_COLOR = (0, 0, 0)
FOOD_COLOR = (255, 0, 0)
SNAKE_COLOR = (0, 255, 0)

# 创建游戏窗口
screen = pygame.display.set_mode((WIDTH, HEIGHT))

# 创建时钟对象
clock = pygame.time.Clock()

# 创建贪吃蛇的变量
snake = [(CELL_WIDTH // 2, CELL_HEIGHT // 2)]
dx = 1
dy = 0

# 创建食物的变量
food = (random.randint(0, CELL_WIDTH - 1), random.randint(0, CELL_HEIGHT - 1))

# 游戏循环
while True:
    # 处理事件
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            exit()
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_UP and dy == 0:
                dx = 0
                dy = -1
            elif event.key == pygame.K_DOWN and dy == 0:
                dx = 0
                dy = 1
            elif event.key == pygame.K_LEFT and dx == 0:
                dx = -1
                dy = 0
            elif event.key == pygame.K_RIGHT and dx == 0:
                dx = 1
                dy = 0

    # 移动贪吃蛇
    x, y = snake[-1]
    x += dx
    y += dy
    if x < 0 or x >= CELL_WIDTH or y < 0 or y >= CELL_HEIGHT:
        pygame.quit()
        exit()
    snake.append((x, y))
    if (x, y) == food:
        food = (random.randint(0, CELL_WIDTH - 1), random.randint(0, CELL_HEIGHT - 1))
    else:
        snake.pop(0)

    # 绘制游戏界面
    screen.fill(BG_COLOR)
    for x, y in snake:
        pygame.draw.rect(screen, SNAKE_COLOR, (x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE))
    pygame.draw.rect(screen, FOOD_COLOR, (food[0] * CELL_SIZE, food[1] * CELL_SIZE, CELL_SIZE, CELL_SIZE))
    score = len(snake) - 1
    pygame.display.flip()

    # 控制游戏帧率
    clock.tick(10)
