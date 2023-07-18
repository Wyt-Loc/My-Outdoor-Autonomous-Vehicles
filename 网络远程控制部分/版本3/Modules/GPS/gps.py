# -*- coding: utf-8 -*-
# @Time    : 2023/7/8 18:17
# @Author  : Wyt
# @File    : gps.py
import time

import serial
import re

utctime = ''
lat = ''
ulat = ''
lon = ''
ulon = ''
numSv = ''
msl = ''
cogt = ''
cogm = ''
sog = ''
kph = ''
gps_t = 0

ser = serial.Serial("COM8", 9600)

if ser.isOpen():
    print("GPS Serial Opened! Baudrate=9600")
else:
    print("GPS Serial Open Failed!")


def Convert_to_degrees(in_data1, in_data2):
    len_data1 = len(in_data1)
    str_data2 = "%05d" % int(in_data2)
    temp_data = int(in_data1)
    symbol = 1
    if temp_data < 0:
        symbol = -1
    degree = int(temp_data / 100.0)
    str_decimal = str(in_data1[len_data1 - 2]) + str(in_data1[len_data1 - 1]) + str(str_data2)
    f_degree = int(str_decimal) / 60.0 / 100000.0
    # print("f_degree:", f_degree)
    if symbol > 0:
        result = degree + f_degree
    else:
        result = degree - f_degree
    return result


def GPS_read():
    global utctime
    global lat
    global ulat
    global lon
    global ulon
    global numSv
    global msl
    global cogt
    global cogm
    global sog
    global kph
    global gps_t
    if ser.inWaiting():
        if ser.read(1) == b'G':
            if ser.inWaiting():
                if ser.read(1) == b'N':
                    if ser.inWaiting():
                        choice = ser.read(1)
                        if choice == b'G':
                            if ser.inWaiting():
                                if ser.read(1) == b'G':
                                    if ser.inWaiting():
                                        if ser.read(1) == b'A':
                                            # utctime = ser.read(7)
                                            GGA = ser.read(70)
                                            GGA_g = re.findall(r"\w+(?=,)|(?<=,)\w+", str(GGA))
                                            # print(GGA_g)
                                            if len(GGA_g) < 13:
                                                print("GPS no found")
                                                gps_t = 0
                                                return 0
                                            else:
                                                utctime = GGA_g[0]
                                                # lat = GGA_g[2][0]+GGA_g[2][1]+'°'+GGA_g[2][2]+GGA_g[2][3]+'.'+GGA_g[3]+'\''
                                                lat = "%.8f" % Convert_to_degrees(str(GGA_g[2]), str(GGA_g[3]))
                                                ulat = GGA_g[4]
                                                # lon = GGA_g[5][0]+GGA_g[5][1]+GGA_g[5][2]+'°'+GGA_g[5][3]+GGA_g[5][4]+'.'+GGA_g[6]+'\''
                                                lon = "%.8f" % Convert_to_degrees(str(GGA_g[5]), str(GGA_g[6]))
                                                ulon = GGA_g[7]
                                                numSv = GGA_g[9]
                                                msl = GGA_g[12] + '.' + GGA_g[13] + GGA_g[14]
                                                # print(GGA_g)
                                                gps_t = 1
                                                return 1
                        elif choice == b'V':
                            if ser.inWaiting():
                                if ser.read(1) == b'T':
                                    if ser.inWaiting():
                                        if ser.read(1) == b'G':
                                            if gps_t == 1:
                                                VTG = ser.read(40)
                                                VTG_g = re.findall(r"\w+(?=,)|(?<=,)\w+", str(VTG))
                                                cogt = VTG_g[0] + '.' + VTG_g[1] + 'T'
                                                if VTG_g[3] == 'M':
                                                    cogm = '0.00'
                                                    sog = VTG_g[4] + '.' + VTG_g[5]
                                                    kph = VTG_g[7] + '.' + VTG_g[8]
                                                elif VTG_g[3] != 'M':
                                                    cogm = VTG_g[3] + '.' + VTG_g[4]
                                                    sog = VTG_g[6] + '.' + VTG_g[7]
                                                    kph = VTG_g[9] + '.' + VTG_g[10]
                                            # print(kph)


gpsdata = [0]
js = 0

try:
    while True:
        if GPS_read():
            time.sleep(30)
            js += 1
            print(js)
            gpsdata[0] = "utctime " + utctime + " latitude " + lat + ulat + " longitude " + lon + ulon \
                         + " num " + numSv + " hangxiang " + cogt + " speed " + kph + "\n"
            with open("gpsdata.txt", 'a') as f:
                f.write(str(gpsdata[0]))

except KeyboardInterrupt:
    ser.close()
    print("GPS serial Close!")
