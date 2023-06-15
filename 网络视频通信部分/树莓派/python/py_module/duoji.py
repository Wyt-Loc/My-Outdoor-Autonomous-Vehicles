import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM) #GPIO引脚编号的两种方式之一，另一种是BCM。所用编号方式不同，pin也不同。
GPIO.setup(18,GPIO.OUT) # pin为你所接的数据输出GPIO引脚。
p = GPIO.PWM(18,50) # 在pin脚上产生频率为50HZ的PWM信号。
p.start(0) # 在引脚上设置一个初始PWM信号。
p.ChangeDutyCycle(7) 
time.sleep(5)

p.stop() # 停止PWM。
GPIO.cleanup() # 清理GPIO。

