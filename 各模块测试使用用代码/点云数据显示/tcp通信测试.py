# -*- coding: utf-8 -*-
# @Time    : 2023/5/21 10:04
# @Author  : Wyt
# @File    : tcp通信测试.py
# coding=utf-8

from socket import *

HOST = '8.130.115.65'  # 服务器的ip
PORT = 8887  # 服务器相同的端口，这块得和服务器的一样了，和UDP不太一样的地方
BUFSIZ = 1024
ADDR = (HOST, PORT)

tcpCliSock = socket(AF_INET, SOCK_STREAM)
tcpCliSock.connect(ADDR)

while True:
    stock_codes = input('the stock_codes is : ')  # 客户输入想输入的stock_codes
    try:
        tcpCliSock.send(stock_codes.encode())  # 向服务器端发送编码后的数据
    except Exception:
        print("发送出错...")
    print(stock_codes)

tcpCliSock.close()
