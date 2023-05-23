/**
 ****************************************************************************************************
 * @file        demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MO1218模块测试实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "demo.h"
#include "./BSP/ATK_MO1218/atk_mo1218.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"

/**
 * @brief       例程演示入口函数
 * @param       无
 * @retval      无
 */
void demo_run(void)
{
    uint8_t ret;
    
    /* 初始化ATK-MO1218模块 */
    ret = atk_mo1218_init(38400);
    if (ret != 0)
    {
        printf("ATK-MO1218 init failed!\r\n");
        while (1)
        {
            LED0_TOGGLE();
            delay_ms(200);
        }
    }
    
    /* 配置ATK-MO1218模块 */
    ret  = atk_mo1218_factory_reset(ATK_MO1218_FACTORY_RESET_REBOOT);
    ret += atk_mo1218_config_output_type(ATK_MO1218_OUTPUT_NMEA, ATK_MO1218_SAVE_SRAM_FLASH);
    ret += atk_mo1218_config_nmea_msg(1, 1, 1, 1, 1, 1, 0, ATK_MO1218_SAVE_SRAM_FLASH);
    ret += atk_mo1218_config_position_rate(ATK_MO1218_POSITION_RATE_5HZ, ATK_MO1218_SAVE_SRAM_FLASH);
    ret += atk_mo1218_config_gnss_for_navigation(ATK_MO1218_GNSS_GPS_BEIDOU, ATK_MO1218_SAVE_SRAM_FLASH);
    if (ret != 0)
    {
        printf("ATK-MO1218 configure failed!\r\n");
        while (1)
        {
            LED0_TOGGLE();
            delay_ms(200);
        }
    }
     
    while (1)
    {
        uint8_t ret;
        atk_mo1218_time_t utc;
        atk_mo1218_position_t position;
        int16_t altitude;
        uint16_t speed;
        atk_mo1218_fix_info_t fix_info;
        atk_mo1218_visible_satellite_info_t gps_satellite_info = {0};
        atk_mo1218_visible_satellite_info_t beidou_satellite_info = {0};
        uint8_t satellite_index;
        
        while (1)
        {
            /* 获取并更新ATK-MO1218模块数据 */
            ret = atk_mo1218_update(&utc, &position, &altitude, &speed, &fix_info, NULL, NULL, 5000);
            if (ret == ATK_MO1218_EOK)
            {
                /* UTC */
                printf("UTC Time: %04d-%02d-%02d %02d:%02d:%02d.%03d\r\n", utc.year, utc.month, utc.day, utc.hour, utc.minute, utc.second, utc.millisecond);
                
                /* 经纬度（放大了100000倍数） */
                printf("Position: %d.%d'%s %d.%d'%s\r\n", position.longitude.degree / 100000, position.longitude.degree % 100000, (position.longitude.indicator == ATK_MO1218_LONGITUDE_EAST) ? "E" : "W", position.latitude.degree / 100000, position.latitude.degree % 100000, (position.latitude.indicator == ATK_MO1218_LATITUDE_NORTH) ? "N" : "S");
                
                /* 海拔高度（放大了10倍） */
                printf("Altitude: %d.%dm\r\n", altitude / 10, altitude % 10);
                
                /* 速度（放大了10倍） */
                printf("Speed: %d.%dKm/H\r\n", speed / 10, speed % 10);
                
                /* 定位质量 */
                printf("Fix quality: %s\r\n", (fix_info.quality == ATK_MO1218_GPS_UNAVAILABLE) ? "Unavailable" : ((fix_info.quality == ATK_MO1218_GPS_VALID_SPS) ? "SPS mode" : "differential GPS mode"));
                
                /* 用于定位的卫星数量 */
                printf("Satellites Used: %d\r\n", fix_info.satellite_num);
                
                /* 定位方式 */
                printf("Fix type: %s\r\n", (fix_info.type == ATK_MO1218_FIX_NOT_AVAILABLE) ? "Unavailable" : ((fix_info.type == ATK_MO1218_FIX_2D) ? "2D" : "3D"));
                
                /* 用于定位的卫星编号 */
                for (satellite_index=0; satellite_index<fix_info.satellite_num; satellite_index++)
                {
                    if (satellite_index == 0)
                    {
                        printf("Satellite ID:");
                    }
                    printf(" %d", fix_info.satellite_id[satellite_index]);
                    if (satellite_index == fix_info.satellite_num - 1)
                    {
                        printf("\r\n");
                    }
                }
                
                /* 位置、水平、垂直精度因子（扩大了10倍） */
                printf("PDOP: %d.%d\r\n", fix_info.pdop / 10, fix_info.pdop % 10);
                printf("HDOP: %d.%d\r\n", fix_info.hdop / 10, fix_info.hdop % 10);
                printf("VDOP: %d.%d\r\n", fix_info.vdop / 10, fix_info.vdop % 10);
                
                /* 可见的GPS、北斗卫星数量 */
                printf("Number of GPS visible satellite: %d\r\n", gps_satellite_info.satellite_num);
                printf("Number of Beidou visible satellite: %d\r\n", beidou_satellite_info.satellite_num);
                
                printf("\r\n");
            }
            else
            {
                /* ATK-MO1218模块未定位时，
                 * 不输出NMEA协议的GSV语句，
                 * 导致因获取不到可见GPS、北斗卫星的信息而超时失败，
                 * 此时可将函数atk_mo1218_update()的入参gps_satellite_info和beidou_satellite_info
                 * 传入NULL，从而获取未定位时的其他数据
                 */
                printf("Error!\r\n");
            }
            
            delay_ms(1000);
        }
    }
}
