import socket
import serial
import time
import threading


ser = serial.Serial("/dev/ttyAMA0",115200)

str1 = "123123\r\n"

if ser.isOpen == False:
    ser.open() # 打开串口
ser.write(b'raspberry pi is connected') # 连接成功

# 1.创建socket
tcp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# 2. 链接服务器
server_addr = ("8.130.115.65", 8887)
tcp_socket.connect(server_addr)

us_flag = 0
info = b''




# 串口发送
def usat_send():
    try:
        while True:
            ser.write(str1.encode())
            time.sleep(0.05)
    except KeyboardInterrupt:
        ser.close()

# 串口接收
def usat_tend():
    try:
        while True:
            count = ser.inWaiting()
            if count > 0:
                recv = ser.read(count)
                print("\r\n接收的数据为:" + str(recv.decode()))
                time.sleep(0.05)
    except  KeyboardInterrupt:
        ser.close()






def main_tcp():
    global us_flag,info
    flag = 1
    print("开启")
    while True:
        # 3. 发送数据
        if flag == 1:
            send_data = "smpsmp"  # input("请输入要发送的数据：")
            tcp_socket.send(send_data.encode("utf-8"))
            flag = 0

        info = tcp_socket.recv(1024)
        try:
            if info and us_flag == 0:
                us_flag = 1
                print(info.decode('gbk'))
                # tcp_socket.close()
        except:
            pass


def us_send():
    global us_flag,info
    while True:
        if us_flag == 1:
            print("串口发送")
            ser.write(info) # 串口发送接收到的数据
            us_flag = 0


def main_usart():
    threads = []
    thing1 = threading.Thread(target=us_send)
    threads.append(thing1)
    thing2 = threading.Thread(target=main_tcp)
    threads.append(thing2)
    for thing in threads:
        thing.setDaemon(True)
        thing.start()

    thing1.join()
    thing2.join()


# 函数入口
if __name__ == '__main__':
    main_usart()




