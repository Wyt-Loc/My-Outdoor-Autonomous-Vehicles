# -*- coding: utf-8 -*-
# @Time    : 2023/5/19 10:35
# @Author  : Wyt
# @File    : 百度鹰眼.py

#  创建entry 的代码  以及提交点位的代码

import requests
import json
import time

url = 'https://yingyan.baidu.com/api/v3/entity/add'

data1 = {'ak': 'soUFZ0WCoc0pVQLaZguLREv4BzEKDwGb', 'service_id': '236965', 'entity_name': 'edu1', 'entity_name': 'li',
         'city': '衡水', 'latitude': "1.123213", "longitude": "12.12312"}

r = requests.post(url, data1)
print(r)
print(r.text)
