# -*- coding: utf-8 -*-
# @Time    : 2023/6/30 12:23
# @Author  : Wyt
# @File    : MainStart.py


import threading
import MyTcpServer

if __name__ == '__main__':
    tcpObj = MyTcpServer.TcpServer()  # 继承log 和 time 类
    # 直到有一个客户端连接  暂不考虑多用户情况
    while True:
        if tcpObj.getConnObj():
            break

    t1 = threading.Thread(target=tcpObj.receMessages)
    t1.start()
    t2 = threading.Thread(target=tcpObj.sendMessages, args=("da",))
    t2.start()
