# 导入模块
import socket
import threading
import re


iplist = [1,1]

info = b''
infoflag = 0


# 接收PC端消息
def recvi_pc(client_socket, ip_port):
    global info,infoflag
    flag = 0
    while True:
        client_text = client_socket.recv(1024)
        print(client_text.decode("gbk"))
        if flag == 0:
            if client_text.decode('gbk') == 'smpsmp': # 端口保存
                iplist[0] = ip_port
            if client_text.decode('gbk') == 'pcpc':
                iplist[1] = ip_port
            if iplist[0] != 1 and iplist[1] != 1 and flag == 0:
                print(iplist)
                print("TCP连接完成")
                flag = 1
        # 如果接收的消息长度不为0，则将其解码输出
        if client_text:
            if infoflag == 0:
                info = client_text #将消息存储到info里
                infoflag = 1 
                print("[客户端消息]", ip_port, ":", client_text.decode("gbk"))
            # 给客户端响应
          #  client_socket.send("收到\n".encode())
        # 当客户端断开连接时，会一直发送''空字符串，所以长度为0已下线
        else:
            print("客户端", ip_port, "已下线")
            client_socket.close()
            break



# 将消息转发给树莓派
def recvi_smp(client_socket, ip_port):
    global info,infoflag
    while True:
        if infoflag == 1:
            print("转发数据")
            client_socket.send(info)
            infoflag = 0

# 程序主入口
def main_tcp():
    js = 0
    print("开启进程2--tcp")
    # 创建TCP套接字
    tcp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # 设置端口复用
    tcp_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, True)
    # 绑定端口
    tcp_socket.bind(("172.24.34.71", 8887))
    # 设置为被动监听状态，128表示最大连接数
    tcp_socket.listen(5)
    while True:
        # 等待客户端连接
        client_socket, ip_port = tcp_socket.accept()
        print("[新客户端]:", ip_port, "已连接")
     #   iplist.append(ip_port)
        # 有客户端连接后，创建一个线程将客户端套接字，IP端口传入recv函数
        if js == 0:
            t1 = threading.Thread(target=recvi_pc, args=(client_socket, ip_port))
            # 设置线程守护
            t1.setDaemon(True)
            # 启动线程
            t1.start()
            js = 1
        if js == 1:
            t2 = threading.Thread(target=recvi_smp, args=(client_socket,iplist[0]))
            t2.setDaemon(True)
            t2.start()


#if __name__ == '__main__':
 #   main()


