import multiprocessing

# 创建共享内存中的字节数组
shared_bytes = multiprocessing.RawArray('B', 10)


def f(process_num, arr):
    # 在进程中修改共享的字节数组
    arr = b'123'


if __name__ == '__main__':
    # 初始化数组
    shared_bytes = b'2'

    # 创建两个进程
    p1 = multiprocessing.Process(target=f, args=(1, shared_bytes))
    p2 = multiprocessing.Process(target=f, args=(2, shared_bytes))

    # 启动进程
    p1.start()
    p2.start()

    # 等待两个进程结束
    p1.join()
    p2.join()

    # 输出修改后的数组
    print('Final array values:', list(shared_bytes))
