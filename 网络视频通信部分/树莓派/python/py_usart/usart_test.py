import serial
import time

ser = serial.Serial("/dev/ttyAMA0",115200)

if not ser.isOpen():
    print("open failed")
else:
    print("open success: ")
    print(ser)

try:
    while True:
        count = ser.inWaiting()
        if count > 0:
            recv = ser.read(count)
            print("recv: " + recv)
            ser.write(recv)
        time.sleep(0.05)
except KeyboardInterrupt:
    if ser != None:
        ser.close()
