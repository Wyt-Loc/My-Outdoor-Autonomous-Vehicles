# -*- coding: utf-8 -*-
# @Time    : 2023/6/24 19:13
# @Author  : Wyt
# @File    : 闭包.py

# 匿名函数 lambda
# 高阶函数
# 迭代器 iter and  next  and __iter__  and  __next__
# 闭包   def(): def():
# 生成器
# 装饰器

# 匿名函数
mylist = [1, 2, 3, 4]
print(list(map(lambda x: x + 1, mylist)))


# 高阶函数
def aaa(x):
    return x ** 2


def bbb(x, y, f):
    return x + y + f(x) + f(y)


# 即函数调用函数
print(bbb(1, 2, aaa))


# 闭包
def who(name):
    def context(data):
        print(name, end=" say:")
        print(data)

    return context


zhangsay = who("张三")
lisi = who("lisi")
zhangsay('1111')
lisi("222")
