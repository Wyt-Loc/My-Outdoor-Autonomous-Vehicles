import tcptest
import xintiao
import usart_send
import sys
from multiprocessing import Process

def main_all():
    sys.stdin.fileno()
    p1 = Process(target=usart_send.main_usart)
    p1.start
    data = input("输入99来开启视频传输")
    p2 = Process(target=xintiao.main_sp,args=(data,))
    p2.start()

if __name__ == "__main__":
    main_all()
