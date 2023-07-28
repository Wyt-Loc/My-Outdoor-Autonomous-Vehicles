# -*- coding: utf-8 -*-
# @Time    : 2023/7/20 21:36
# @Author  : Wyt
# @File    : 自动化发弹幕脚本.py

import requests
import time
from tkinter import *
import random

lis_text = ['1', '2', '3']


def send():
    a = 0
    while True:
        time.sleep(2)
        send_meg = random.choice(lis_text)
        roomid = entry.get()
        ti = int(time.time())
        url = 'https://api.live.bilibili.com/msg/send'
        data = {
            'color': '16777215',
            'fontsize': '25',
            'mode': '1',
            'msg': send_meg,
            'rnd': '{}'.format(ti),
            'roomid': '{}'.format(21481459),
            'bubble': '0',
            'csrf_token': '748aab25091142e695d26147fbc0ecc3',
            'csrf': '748aab25091142e695d26147fbc0ecc3'
        }

        headers = {
            'cookie': "buvid3=F1BAD787-3806-98C8-89BA-AD5C1E702C8507302infoc; b_nut=1680430507; i-wanna-go-back=-1; _uuid=B49CA10BC-BF61-AEC6-FE63-48B1056B37DD107730infoc; buvid_fp=3237e5180acbed91cb74d4d311929918; buvid4=40DA4FB7-924C-5C06-688F-765D2440E47208761-023040218-npnFjWKNXoaLNHJKZSx1xw%3D%3D; header_theme_version=CLOSE; DedeUserID=634452845; DedeUserID__ckMd5=42b39b45194cb214; CURRENT_PID=40c35a50-d140-11ed-9a41-9b73b1687c87; rpdid=|(m~)lm~Rl)0J'uY)|RJ|mRk; b_ut=5; nostalgia_conf=-1; CURRENT_QUALITY=80; CURRENT_FNVAL=16; FEED_LIVE_VERSION=V8; home_feed_column=5; browser_resolution=1536-751; innersign=0; b_lsid=986181046_1897381AA59; SESSDATA=046d7ceb%2C1705412027%2C23e66%2A71Ev-KgrJkxTlZWKfwvqRPsjV_0vWqX9BsSyK4AhzgJwomsSNtq7EpAN5B2keO1XNt3ZgvDQAALQA; bili_jct=748aab25091142e695d26147fbc0ecc3; sid=7uueblq0; LIVE_BUVID=AUTO5616898600416745; PVID=2; Hm_lvt_8a6e55dbd2870f0f5bc9194cddf32a02=1689860503; Hm_lpvt_8a6e55dbd2870f0f5bc9194cddf32a02=1689860503",
            'origin': 'https://live.bilibili.com',
            'referer': 'https://live.bilibili.com/blanc/1029?liteVersion=true',
            'user-agent': 'Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/81.0.4044.138 Safari/537.36',
        }
        a += 1
        response = requests.post(url=url, data=data, headers=headers)
        print(response)
        text.insert(END, '第{}条弹幕发送成功'.format(a))
        # 文本框滚动
        text.see(END)
        # 更新
        text.update()
        text.insert(END, '发送内容：{}'.format(send_meg))


root = Tk()
root.title('B站自动发送弹幕')
root.geometry('560x450+400+200')

label = Label(root, text='请输入房间ID:', font=('华文行楷', 20))
label.grid()

entry = Entry(root, font=('隶书', 20))
entry.grid(row=0, column=1)

text = Listbox(root, font=('隶书', 16), width=50, heigh=15)
text.grid(row=2, columnspan=2)

button1 = Button(root, text='开始发送', font=('隶书', 15), command=send)
button1.grid(row=3, column=0)

button2 = Button(root, text='退出程序', font=('隶书', 15), command=root.quit)
button2.grid(row=3, column=1)

root.mainloop()
