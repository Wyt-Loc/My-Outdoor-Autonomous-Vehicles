import pygame

pygame.init()

# 创建一个 clock 对象，用于控制帧率
clock = pygame.time.Clock()

WIDTH = 500
HEIGHT = 500

# 创建游戏窗口
pygame.display.set_mode((WIDTH, HEIGHT))

# 创建一个字典，用于记录每个键的按下时间
keydown_dict = {}

while True:
    # 获取键盘事件
    for event in pygame.event.get():
        if event.type == pygame.KEYDOWN:
            # 记录按键按下时的时间
            keydown_dict[event.key] = pygame.time.get_ticks()
        elif event.type == pygame.KEYUP:
            # 判断按键按下时间是否超过 500 毫秒，如果是则为长按
            if pygame.time.get_ticks() - keydown_dict[event.key] > 500:
                print("key", event.key, "long pressed")
            else:
                print("key", event.key, "clicked")
            # 删除记录的按键按下时间
            del keydown_dict[event.key]

    # 控制帧率为 30 帧每秒
    clock.tick(30)
