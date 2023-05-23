#!/usr/bin/env python3
# coding=utf-8

import os
import time
import math


# V1.0.1
class GPS_MAP:
    def __init__(self, debug=False):
        self.PI = 3.1415926535897932384626
        self.A = 6378245.0
        self.EE = 0.00669342162296594323

    
    def str_To_Gps84(self, in_data1, in_data2):
        len_data1 = len(in_data1)
        str_data2 = "%05d" % int(in_data2)
        temp_data = int(in_data1)
        symbol = 1
        if temp_data < 0:
            symbol = -1
        degree = int(temp_data / 100.0)
        str_decimal = str(in_data1[len_data1-2]) + str(in_data1[len_data1-1]) + '.' + str(str_data2)
        f_degree = float(str_decimal)/60.0
        # print("f_degree:", f_degree)
        if symbol > 0:
            result = degree + f_degree
        else:
            result = degree - f_degree
        return result


    def gps84_to_gcj02(self, lat, lon):
        if (self.outOfChina(lat, lon)):
            return [0, 0]
        dLat = self.transformLat(lon - 105.0, lat - 35.0)
        dLon = self.transformLon(lon - 105.0, lat - 35.0)
        radLat = lat / 180.0 * self.PI
        magic = math.sin(radLat)
        magic = 1 - self.EE * magic * magic
        sqrtMagic = math.sqrt(magic)
        dLat = (dLat * 180.0) / ((self.A * (1 - self.EE)) / (magic * sqrtMagic) * self.PI)
        dLon = (dLon * 180.0) / (self.A / sqrtMagic * math.cos(radLat) * self.PI)
        mgLat = lat + dLat
        mgLon = lon + dLon
        return [mgLat, mgLon]

    def gcj02_to_bd09(self, gg_lat, gg_lon):
        x = gg_lon
        y = gg_lat
        z = math.sqrt(x * x + y * y) + 0.00002 * math.sin(y * self.PI)
        theta = math.atan2(y, x) + 0.000003 * math.cos(x * self.PI)
        bd_lon = z * math.cos(theta) + 0.0065
        bd_lat = z * math.sin(theta) + 0.006
        return [bd_lat, bd_lon]


    def outOfChina(self, lat, lon):
        if (lon < 72.004 or lon > 137.8347):
            return True
        if (lat < 0.8293 or lat > 55.8271):
            return True
        return False
    

    def transform(self, lat, lon):
        if (self.outOfChina(lat, lon)):
            return [lat, lon]
        dLat = self.transformLat(lon - 105.0, lat - 35.0)
        dLon = self.transformLon(lon - 105.0, lat - 35.0)
        radLat = lat / 180.0 * self.PI
        magic = math.sin(radLat)
        magic = 1 - self.EE * magic * magic
        sqrtMagic = math.sqrt(magic)
        dLat = (dLat * 180.0) / ((self.A * (1 - self.EE)) / (magic * sqrtMagic) * self.PI)
        dLon = (dLon * 180.0) / (self.A / sqrtMagic * math.cos(radLat) * self.PI)
        mgLat = lat + dLat
        mgLon = lon + dLon
        return [mgLat, mgLon]
    

    def transformLat(self, x, y):
        ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * math.sqrt(abs(x))
        ret += (20.0 * math.sin(6.0 * x * self.PI) + 20.0 * math.sin(2.0 * x * self.PI)) * 2.0 / 3.0
        ret += (20.0 * math.sin(y * self.PI) + 40.0 * math.sin(y / 3.0 * self.PI)) * 2.0 / 3.0
        ret += (160.0 * math.sin(y / 12.0 * self.PI) + 320 * math.sin(y * self.PI / 30.0)) * 2.0 / 3.0
        return ret
    

    def transformLon(self, x, y):
        ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * math.sqrt(abs(x))
        ret += (20.0 * math.sin(6.0 * x * self.PI) + 20.0 * math.sin(2.0 * x * self.PI)) * 2.0 / 3.0
        ret += (20.0 * math.sin(x * self.PI) + 40.0 * math.sin(x / 3.0 * self.PI)) * 2.0 / 3.0
        ret += (150.0 * math.sin(x / 12.0 * self.PI) + 300.0 * math.sin(x / 30.0 * self.PI)) * 2.0 / 3.0
        return ret
    

if __name__ == '__main__':
    gps = GPS_MAP()
    try:
        in_data1 = '11357'
        in_data2 = '94453'
        in_data3 = '2234'
        in_data4 = '98984'
        f_lon = gps.str_To_Gps84(in_data1, in_data2)
        f_lat = gps.str_To_Gps84(in_data3, in_data4)
        print("lat:%f, lon:%f" % (f_lat, f_lon))
        gcj02_data = gps.gps84_to_gcj02(f_lat, f_lon)
        print("gcj02:", gcj02_data)
        bd09_data = gps.gcj02_to_bd09(gcj02_data[0], gcj02_data[1])
        print("bd09:", bd09_data)

    except KeyboardInterrupt:
        pass

