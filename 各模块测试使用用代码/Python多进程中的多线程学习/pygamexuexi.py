import pygame

pygame.init()

# 设置窗口大小
WINDOW_SIZE = (800, 600)

# 创建窗口
screen = pygame.display.set_mode(WINDOW_SIZE)

# 字体和字号
font = pygame.font.SysFont("Arial", 30)

# 设置按键的按下时间
keys_down = {}

while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            exit()
        elif event.type == pygame.KEYDOWN:
            # 记录按键按下的时间
            keys_down[event.key] = pygame.time.get_ticks()

    # 检查按键按下的时间是否在范围内
    now = pygame.time.get_ticks()
    for key, time_down in list(keys_down.items()):
        time_passed = now - time_down
        if time_passed > 300:
            print(f"长按事件: {pygame.key.name(key)}")
            keys_down.pop(key)
        elif time_passed > 200 and time_passed < 300:
            print(f"短按事件: {pygame.key.name(key)}")
            keys_down.pop(key)

    # 更新窗口
    pygame.display.update()
