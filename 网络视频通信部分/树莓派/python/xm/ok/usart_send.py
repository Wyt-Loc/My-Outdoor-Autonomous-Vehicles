import serial
import time
import threading


ser = serial.Serial("/dev/ttyAMA0",115200)

str1 = "123123\r\n"

if ser.isOpen == False:
    ser.open() # 打开串口
ser.write(b'raspberry pi is connected') # 连接成功


# 串口发送
def usat_send():
    try:
        while True:
            str1 = input("请输入要发送的数据：")
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



def main1():
    print("串口收发开始")
    threads = []
    thing1 = threading.Thread(target=usat_send,)
    threads.append(thing1)
    thing2 = threading.Thread(target=usat_tend,)
    threads.append(thing2)
    for thing in threads:
        thing.setDaemon(True)
        thing.start()

    thing1.join()
    thing2.join()

# 函数入口
if __name__ == '__main__':
    main1()




