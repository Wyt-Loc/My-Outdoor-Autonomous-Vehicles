# -*- coding: utf-8 -*-
# @Time    : 2023/5/30 17:13
# @Author  : Wyt
# @File    : xxqg.py

from selenium import webdriver
from selenium.webdriver.support.ui import WebDriverWait
import time
import base64
import numpy as np


# 登陆学习强国
def log_in():
    # 打开学习强国
    web = webdriver.Chrome(executable_path='/Users/a../Documents/machine_learning/spider/chromedriver')
    web.get('https://www.xuexi.cn/')
    time.sleep(2)
    # 点击登录
    log_in = web.find_element_by_xpath('//*[@id="root"]/div/div[1]/header/div[2]/div[2]/a[2]')
    log_in.click()
    time.sleep(5)
    # 保存登录的二维码
    web.switch_to.window(web.window_handles[-1])
    web.switch_to.frame('ddlogin-iframe')
    time.sleep(2)
    img_qr = web.find_element_by_xpath('//*[@id="app"]/div/div[1]/div/div[1]/div[1]/img').get_attribute('src')
    b64 = img_qr.split(',')[1]
    b = base64.b64decode(b64)
    with open('test.png', 'wb') as png:
        png.write(b)
    time.sleep(15)
    return web


# 点击6个新闻
def read_articles(web):
    news = web.find_elements_by_class_name('text-link-item-title')
    rand = np.random.randint(0, 50, 6)
    for i in rand:
        web.execute_script("arguments[0].scrollIntoView();", news[i])
        time.sleep(5)
        news[i].click()
        web.execute_script('arguments[0].click();', news[i])
        web.switch_to.window(web.window_handles[-1])
        time.sleep(5)
        news_title = web.find_element_by_class_name('render-detail-title')
        web.execute_script("arguments[0].scrollIntoView();", news_title)
        button_voice = web.find_element_by_class_name('voice-lang-switch')
        web.execute_script('arguments[0].click();', button_voice)
        time.sleep(70)
        web.close()
        web.switch_to.window(web.window_handles[-1])


# 开始每日答题
def meiridati(web):
    # 进入我的积分页面
    button_jifen = web.find_element_by_xpath('//*[@id="page-main"]/div/section/div/div[4]')
    button_jifen.click()
    web.switch_to.window(web.window_handles[-1])
    time.sleep(2)

    # 进入答题页面
    button_dati = web.find_element_by_xpath('//*[@id="app"]/div/div[2]/div/div[3]/div[2]/div[5]/div[2]/div[2]/div')
    button_dati.click()
    web.switch_to.window(web.window_handles[-1])
    time.sleep(2)


# 根据题目类型答题
def dati(web):
    # 答单选题的函数
    def danxuanti(web):
        # 点击“查看提示”按钮
        button_tips = web.find_element_by_class_name('tips')
        button_tips.click()
        # 返回答案
        answer_danxuan = web.find_element_by_xpath(
            '//*[@id="body-body"]/div[4]/div/div/div/div[2]/div/div/div/font').get_attribute('textContent')
        chooseable = web.find_elements_by_class_name('q-answer.choosable')
        time.sleep(2)
        for n, i in enumerate(chooseable):
            if i.text.split('.')[-1].strip() == answer_danxuan:
                button_sel = web.find_element_by_xpath(
                    '//*[@id="app"]/div/div[2]/div/div[4]/div[1]/div[4]/div[' + str(n + 1) + ']')
                web.execute_script('arguments[0].click();', button_sel)
                button_queding = web.find_element_by_xpath('//*[@id="app"]/div/div[2]/div/div[4]/div[2]/button')
                web.execute_script('arguments[0].click();', button_queding)
        time.sleep(2)

    # 答多选题的函数
    def duoxuanti(web):
        # 点击查看提示按钮
        button_tips = web.find_element_by_class_name('tips')
        button_tips.click()
        time.sleep(1)
        # 返回答案所在elements
        answer_duoxuan = web.find_elements_by_xpath('//*[@id="body-body"]/div[4]/div/div/div/div[2]/div/div/div/font')
        # 获取答案的text
        time.sleep(2)
        tmp = []
        for i in answer_duoxuan:
            tmp.append(i.text)
        # 获取选项所在element
        chooseable = web.find_elements_by_class_name('q-answer.choosable')
        # 将答案与选项匹配
        for n, i in enumerate(chooseable):
            if i.text.split('.')[-1].strip() in tmp:
                button_sel = web.find_element_by_xpath(
                    '//*[@id="app"]/div/div[2]/div/div[4]/div[1]/div[4]/div[' + str(n + 1) + ']')
                web.execute_script('arguments[0].click();', button_sel)
        time.sleep(1)
        # 点击确定
        button_queding = web.find_element_by_xpath('//*[@id="app"]/div/div[2]/div/div[4]/div[2]/button')
        web.execute_script('arguments[0].click();', button_queding)
        time.sleep(2)

    # 答填空题的函数
    def tiankongti(web):
        # 获取答案
        button_tips = web.find_element_by_class_name('tips')
        button_tips.click()
        time.sleep(3)
        # 找到待填空的元素
        blank_fill = web.find_element_by_class_name('blank')
        button_queding = web.find_element_by_xpath('//*[@id="app"]/div/div[2]/div/div[4]/div[2]/button')
        # 判断是否为视频题
        if web.find_element_by_xpath('//*[@id="body-body"]/div[4]/div/div/div/div[2]/div/div/div').text == '请观看视频':
            # 是视频题就乱做
            blank_fill.send_keys('123')
            web.execute_script('arguments[0].click();', button_queding)
            time.sleep(2)
        else:
            # 不是视频题就找答案
            answer_tiankong = web.find_element_by_xpath(
                '//*[@id="body-body"]/div[4]/div/div/div/div[2]/div/div/div/font').get_attribute('textContent')
            blank_fill.send_keys(answer_tiankong)
        time.sleep(1)
        # 点击确定
        button_queding = web.find_element_by_xpath('//*[@id="app"]/div/div[2]/div/div[4]/div[2]/button')
        web.execute_script('arguments[0].click();', button_queding)
        time.sleep(2)

    # 开始答题
    question = web.find_element_by_class_name('q-header').text
    time.sleep(1)
    if question == '填空题':
        tiankongti(web)
    elif question == '单选题':
        danxuanti(web)
    elif question == '多选题':
        duoxuanti(web)

    # 看视频


def kanshipin(web):
    # 找到百灵element并点击
    bailing = web.find_element_by_id('JEDXfdDkvQ')
    web.execute_script("arguments[0].scrollIntoView();", bailing)
    time.sleep(5)
    # web.execute_script('arguments[0].click();',bailing)
    bailing.click()
    web.switch_to.window(web.window_handles[-1])
    time.sleep(3)

    # 获取视频信息
    video_info = web.find_elements_by_class_name('video-info.c-time')
    video_info2 = list(map(lambda x: x.text.split(':'), video_info))  # 视频时长
    video_time = list(map(lambda x: int(x[0]) * 60 + int(x[1]), video_info2))
    videos = web.find_elements_by_class_name('Iuu474S1L6y5p7yalKQbW.grid-cell')

    # 看6个超一分钟的视频
    sum_time = 0
    count = 0
    list_to_watch = []
    for i in video_time:
        if i >= 60:
            list_to_watch.append(count)
        count += 1
    list_to_watch = list_to_watch + [30, 31, 32]
    for i in list_to_watch[0:min(6, len(list_to_watch))]:
        videos[i].click()
        web.switch_to.window(web.window_handles[-1])
        time.sleep(5)
        video_player = web.find_element_by_class_name('gr-video-player__aliplayer.prism-player')
        # 滚动到视频播放器处
        web.execute_script("arguments[0].scrollIntoView();", video_player)
        time.sleep(video_time[i])
        web.close()
        web.switch_to.window(web.window_handles[-1])


# 执行
web = log_in()
meiridati(web)
for i in range(5):
    dati(web)
web.close()
web.switch_to.window(web.window_handles[-1])
read_articles(web)
kanshipin(web)
web.quit()
