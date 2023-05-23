/**
 ****************************************************************************************************
 * @file        atk_mo1218_nmea_msg.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MO1218ģ��NMEA��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "./BSP/ATK_MO1218/atk_mo1218_nmea_msg.h"
#include "./BSP/ATK_MO1218/atk_mo1218.h"
#include <stdio.h>
#include <stdlib.h>

/* ATK-MO1218ģ��NMEA��Ϣ���׺;�ĩ */
#define ATK_MO1218_NMEA_MSG_SS          (0x24)                                  /* Start of sentence: $ */
#define ATK_MO1218_NMEA_MSG_ES          (0x0D0A)                                /* Ending of sentence */

/* ATK-MO1218ģ��NMEA��Ϣ��ַ�� */
#define ATK_MO1218_NMEA_ADDRESS_GNGGA   "GNGGA"                                 /* $GNGGA */
#define ATK_MO1218_NMEA_ADDRESS_GNGLL   "GNGLL"                                 /* $GNGLL */
#define ATK_MO1218_NMEA_ADDRESS_GNGSA   "GNGSA"                                 /* $GNGSA */
#define ATK_MO1218_NMEA_ADDRESS_GPGSA   "GPGSA"                                 /* $GPGSA */
#define ATK_MO1218_NMEA_ADDRESS_BDGSA   "BDGSA"                                 /* $BDGSA */
#define ATK_MO1218_NMEA_ADDRESS_GPGSV   "GPGSV"                                 /* $GPGSV */
#define ATK_MO1218_NMEA_ADDRESS_BDGSV   "BDGSV"                                 /* $BDGSV */
#define ATK_MO1218_NMEA_ADDRESS_GNRMC   "GNRMC"                                 /* $GNRMC */
#define ATK_MO1218_NMEA_ADDRESS_GNVTG   "GNVTG"                                 /* $GNVTG */
#define ATK_MO1218_NMEA_ADDRESS_GNZDA   "GNZDA"                                 /* $GNZDA */

/* ATK-MO1218ģ��NMEA��Ϣ��ַ�γ��� */
#define ATK_MO1218_NMEA_ADDRESS_LEN     5

/**
 * @brief       ��ȡNMEA��Ϣ��ָ�����������ݲ���
 * @param       nmea           : NMEA��Ϣ
 *              parameter_index: ����������
 *              parameter      : ���صĲ�����ָ��NMEA��Ϣ�е����ݣ�
 *              parameter_len  : �����ĳ���
 * @retval      ATK_MO1218_EOK   : �ɹ���ȡNMEA��Ϣ��ָ�����������ݲ���
 *              ATK_MO1218_ERROR : NMEA��Ϣ���Ҳ���ָ�����������ݲ���
 *              ATK_MO1218_EINVAL: ������������
 */
static uint8_t atk_mo1218_decode_nmea_parameter(uint8_t *nmea, uint8_t parameter_index, uint8_t **parameter, uint16_t *parameter_len)
{
    uint8_t *nmea_point;
    uint16_t parameter_counter = 0;
    
    if (nmea == NULL)
    {
        return ATK_MO1218_EINVAL;
    }
    
    if (*parameter == NULL)
    {
        return ATK_MO1218_EINVAL;
    }
    
    for (nmea_point=(nmea+1);(*nmea_point!=0x0D)&&(*(nmea_point+1)!=0x0A); nmea_point++)
    {
        if (parameter_index == 0)
        {
            *parameter = nmea_point - parameter_counter;
            
            if (parameter_len == NULL)
            {
                break;
            }
            
            if ((*nmea_point != ',') && (*nmea_point != '*'))
            {
                parameter_counter++;
                continue;
            }
            else
            {
                *parameter_len = parameter_counter;
                break;
            }
        }
        
        if ((*nmea_point == ',') || (*nmea_point == '*'))
        {
            parameter_index--;
        }
    }
    
    if (parameter_index != 0)
    {
        return ATK_MO1218_ERROR;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       �����Ŵ�����ֵ�С��λ��
 * @param       num      : ����������
 *              float_len: numԭ����С��λ��
 *              fix_len  : num�������С��λ��
 * @retval      ��
 */
static void atk_mo1218_fit_num(int32_t *num, uint8_t float_len, uint8_t fix_len)
{
    if (num == NULL)
    {
        return;
    }
    
    while (float_len > fix_len)
    {
        *num /= 10;
        float_len--;
    }
    
    while (float_len < fix_len)
    {
        *num *= 10;
        float_len++;
    }
}

/**
 * @brief       ��NMEA��Ϣ�е��ַ�����������','��'*'��س���0x0D����β��תΪ����
 * @param       str      : NMEA��Ϣ�е��ַ�������
 *              num      : ת���������
 *              float_len: ת���������ʵ��С�����λ��
 * @retval      ATK_MO1218_EOK   : ת���ɹ�
 *              ATK_MO1218_EINVAL: ������������
 */
static uint8_t atk_mo1218_nmea_str2num(uint8_t *str, int32_t *num, uint8_t *float_len)
{
    uint8_t *str_point = str;
    int8_t negative_coefficient;
    uint8_t str_index_num;
    int32_t _num = 0;
    uint8_t _float_len = 0;
    
    if ((str == NULL) || (num == NULL))
    {
        return ATK_MO1218_EINVAL;
    }
    
    if ((*str_point != '-') && ((*str_point < '0') || (*str_point > '9')))
    {
        return ATK_MO1218_EINVAL;
    }
    
    if (*str_point == '-')
    {
        str_point++;
        negative_coefficient = -1;
    }
    else
    {
        negative_coefficient = 1;
    }
    
    while ((*str_point == '.') || ((*str_point >= '0') && (*str_point <= '9')))
    {
        if (_float_len != 0)
        {
            _float_len++;
        }
        
        if (*str_point == '.')
        {
            _float_len++;
            str_point++;
            continue;
        }
        
        str_index_num = *str_point - '0';
        _num = _num * 10 + str_index_num;
        str_point++;
    }
    
    *num = _num * negative_coefficient;
    if (float_len != NULL)
    {
        *float_len = (_float_len != 0) ? (_float_len - 1) : _float_len;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       �����ݻ����л�ȡָ�����ͺ�������NMEA��Ϣ
 * @param       buf      : ���ݻ���
 *              nmea     : ָ����ȡ��NMEA��Ϣ����
 *              msg_index: ָ����ȡ��NMEA��Ϣ���������ݻ����е�ָ������
 *                              0: ��ȡ���ݻ����е�һ��ָ��NMEA��Ϣ���͵���Ϣ
 *                              1: ��ȡ���ݻ����е�һ��ָ��NMEA��Ϣ���͵���Ϣ
 *                              2: ��ȡ���ݻ����еڶ���ָ��NMEA��Ϣ���͵���Ϣ
 *                              3: ��ȡ���ݻ����е�����ָ��NMEA��Ϣ���͵���Ϣ
 *                              �Դ�����......
 *              msg      : ��ȡ����NMEA��Ϣ��ָ�����ݻ����е����ݣ�
 * @retval      ATK_MO1218_EOK   : �ɹ������ݻ����л�ȡ��ָ�����ͺ�������NMEA��Ϣ
 *              ATK_MO1218_ERROR : ���ݻ������Ҳ���ָ�����ͻ�������NMEA��Ϣ
 *              ATK_MO1218_EINVAL: ������������
 */
uint8_t atk_mo1218_get_nmea_msg_from_buf(uint8_t *buf, atk_mo1218_nmea_msg_t nmea, uint8_t msg_index, uint8_t **msg)
{
    char address[ATK_MO1218_NMEA_ADDRESS_LEN + 1];
    uint8_t *buf_point;
    uint8_t address_index;
    
    if ((buf == NULL) || (msg == NULL))
    {
        return ATK_MO1218_EINVAL;
    }
    
    if (msg_index == 0)
    {
        msg_index = 1;
    }
    
    switch (nmea)
    {
        case ATK_MO1218_NMEA_MSG_GNGGA:
        {
            sprintf(address, ATK_MO1218_NMEA_ADDRESS_GNGGA);
            break;
        }
        case ATK_MO1218_NMEA_MSG_GNGLL:
        {
            sprintf(address, ATK_MO1218_NMEA_ADDRESS_GNGLL);
            break;
        }
        case ATK_MO1218_NMEA_MSG_GNGSA:
        {
            sprintf(address, ATK_MO1218_NMEA_ADDRESS_GNGSA);
            break;
        }
        case ATK_MO1218_NMEA_MSG_GPGSA:
        {
            sprintf(address, ATK_MO1218_NMEA_ADDRESS_GPGSA);
            break;
        }
        case ATK_MO1218_NMEA_MSG_BDGSA:
        {
            sprintf(address, ATK_MO1218_NMEA_ADDRESS_BDGSA);
            break;
        }
        case ATK_MO1218_NMEA_MSG_GPGSV:
        {
            sprintf(address, ATK_MO1218_NMEA_ADDRESS_GPGSV);
            break;
        }
        case ATK_MO1218_NMEA_MSG_BDGSV:
        {
            sprintf(address, ATK_MO1218_NMEA_ADDRESS_BDGSV);
            break;
        }
        case ATK_MO1218_NMEA_MSG_GNRMC:
        {
            sprintf(address, ATK_MO1218_NMEA_ADDRESS_GNRMC);
            break;
        }
        case ATK_MO1218_NMEA_MSG_GNVTG:
        {
            sprintf(address, ATK_MO1218_NMEA_ADDRESS_GNVTG);
            break;
        }
        case ATK_MO1218_NMEA_MSG_GNZDA:
        {
            sprintf(address, ATK_MO1218_NMEA_ADDRESS_GNZDA);
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    /* �������ݻ��� */
    for (buf_point=buf; *buf_point!='\0'; buf_point++)
    {
        /* �ҵ����ף�'$'��0x24�� */
        if (*buf_point == ATK_MO1218_NMEA_MSG_SS)
        {
            /* �ж��Ƿ�Ϊָ�����͵�NMEA��Ϣ */
            for (address_index=0; address_index<ATK_MO1218_NMEA_ADDRESS_LEN; address_index++)
            {
                /* ������ָ�����͵�NMEA��Ϣ�����˳�ȥѰ����һ������ */
                if (*(buf_point+1+address_index) != address[address_index])
                {
                    break;
                }
            }
            
            if (address_index == ATK_MO1218_NMEA_ADDRESS_LEN)
            {
                if (--msg_index == 0)
                {
                    *msg = buf_point;
                    return ATK_MO1218_EOK;
                }
            }
        }
    }
    
    return ATK_MO1218_ERROR;
}

/**
 * @brief       ����$XXGGA���͵�NMEA��Ϣ
 * @param       xxgga_msg : ��������$XXGGA����NMEA��Ϣ
 *              decode_msg: �����������
 * @retval      ATK_MO1218_EOK   : ����$XXGGA���͵�NMEA��Ϣ�ɹ�
 *              ATK_MO1218_ERROR : ����$XXGGA���͵�NMEA��Ϣʧ��
 *              ATK_MO1218_EINVAL: ������������
 */
uint8_t atk_mo1218_decode_nmea_xxgga(uint8_t *xxgga_msg, atk_mo1218_nmea_gga_msg_t *decode_msg)
{
    uint8_t ret;
    uint8_t *parameter;
    int32_t _num;
    uint8_t float_len;
    
    if ((xxgga_msg == NULL) || (decode_msg == NULL))
    {
        return ATK_MO1218_EINVAL;
    }
    
    /* Address */
    ret = atk_mo1218_decode_nmea_parameter(xxgga_msg, 0, &parameter, NULL);
    if ((ret != ATK_MO1218_EOK) || ((parameter[2] != 'G') || (parameter[3] != 'G') || (parameter[4] != 'A')))
    {
        return ATK_MO1218_EINVAL;
    }
    
    /* UTC Time */
    ret  = atk_mo1218_decode_nmea_parameter(xxgga_msg, 1, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if ((ret != ATK_MO1218_EOK) || (float_len != 3))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->utc_time.hour = (_num / 10000000) % 100;
    decode_msg->utc_time.minute = (_num / 100000) % 100;
    decode_msg->utc_time.second = (_num / 1000) % 100;
    decode_msg->utc_time.millisecond = (_num / 1) % 1000;
    
    /* Latitude */
    ret  = atk_mo1218_decode_nmea_parameter(xxgga_msg, 2, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    atk_mo1218_fit_num(&_num, float_len, 5);
    decode_msg->latitude.degree = (_num / 10000000) * 100000;
    decode_msg->latitude.degree += ((_num % 10000000) / 60);
    
    /* N/S Indicator */
    ret = atk_mo1218_decode_nmea_parameter(xxgga_msg, 3, &parameter, NULL);
    if ((ret != ATK_MO1218_EOK) || ((parameter[0] != 'N') && (parameter[0] != 'S')))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->latitude.indicator = (parameter[0] == 'N') ? ATK_MO1218_LATITUDE_NORTH : ATK_MO1218_LATITUDE_SOUTH;
    
    /* Longitude */
    ret  = atk_mo1218_decode_nmea_parameter(xxgga_msg, 4, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    atk_mo1218_fit_num(&_num, float_len, 5);
    decode_msg->longitude.degree = (_num / 10000000) * 100000;
    decode_msg->longitude.degree += ((_num % 10000000) / 60);
    
    /* E/W Indicator */
    ret = atk_mo1218_decode_nmea_parameter(xxgga_msg, 5, &parameter, NULL);
    if ((ret != ATK_MO1218_EOK) || ((parameter[0] != 'E') && (parameter[0] != 'W')))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->longitude.indicator = (parameter[0] == 'E') ? ATK_MO1218_LONGITUDE_EAST : ATK_MO1218_LONGITUDE_WEST;
    
    /* GPS quality indicator */
    ret = atk_mo1218_decode_nmea_parameter(xxgga_msg, 6, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if ((ret != ATK_MO1218_EOK) || (float_len != 0))
    {
        return ATK_MO1218_ERROR;
    }
    switch (_num)
    {
        case 0:
        {
            decode_msg->gps_quality = ATK_MO1218_GPS_UNAVAILABLE;
            break;
        }
        case 1:
        {
            decode_msg->gps_quality = ATK_MO1218_GPS_VALID_SPS;
            break;
        }
        case 2:
        {
            decode_msg->gps_quality = ATK_MO1218_GPS_VALID_DIFFERENTIAL;
            break;
        }
        default:
        {
            return ATK_MO1218_ERROR;
        }
    }
    
    /* Satellites Used */
    ret  = atk_mo1218_decode_nmea_parameter(xxgga_msg, 7, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if ((ret != ATK_MO1218_EOK) || (float_len != 0))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->satellite_num = _num;
    
    /* HDOP */
    ret  = atk_mo1218_decode_nmea_parameter(xxgga_msg, 8, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    atk_mo1218_fit_num(&_num, float_len, 1);
    decode_msg->hdop = _num;
    
    /* Altitude */
    ret  = atk_mo1218_decode_nmea_parameter(xxgga_msg, 9, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    atk_mo1218_fit_num(&_num, float_len, 1);
    decode_msg->altitude = _num;
    
    /* Geoidal Separation */
    
    /* DGPS Station ID */
    ret  = atk_mo1218_decode_nmea_parameter(xxgga_msg, 14, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if ((ret != ATK_MO1218_EOK) || (float_len != 0))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->dgps_id = _num;
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       ����$XXGLL���͵�NMEA��Ϣ
 * @param       xxgll_msg : ��������$XXGLL����NMEA��Ϣ
 *              decode_msg: �����������
 * @retval      ATK_MO1218_EOK   : ����$XXGLL���͵�NMEA��Ϣ�ɹ�
 *              ATK_MO1218_ERROR : ����$XXGLL���͵�NMEA��Ϣʧ��
 *              ATK_MO1218_EINVAL: ������������
 */
uint8_t atk_mo1218_decode_nmea_xxgll(uint8_t *xxgll_msg, atk_mo1218_nmea_gll_msg_t *decode_msg)
{
    uint8_t ret;
    uint8_t *parameter;
    int32_t _num;
    uint8_t float_len;
    
    if ((xxgll_msg == NULL) || (decode_msg == NULL))
    {
        return ATK_MO1218_EINVAL;
    }
    
    /* Address */
    ret = atk_mo1218_decode_nmea_parameter(xxgll_msg, 0, &parameter, NULL);
    if ((ret != ATK_MO1218_EOK) || ((parameter[2] != 'G') || (parameter[3] != 'L') || (parameter[4] != 'L')))
    {
        return ATK_MO1218_EINVAL;
    }
    
    /* Latitude */
    ret  = atk_mo1218_decode_nmea_parameter(xxgll_msg, 1, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    atk_mo1218_fit_num(&_num, float_len, 5);
    decode_msg->latitude.degree = (_num / 10000000) * 100000;
    decode_msg->latitude.degree += ((_num % 10000000) / 60);
    
    /* N/S Indicator */
    ret = atk_mo1218_decode_nmea_parameter(xxgll_msg, 2, &parameter, NULL);
    if ((ret != ATK_MO1218_EOK) || ((parameter[0] != 'N') && (parameter[0] != 'S')))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->latitude.indicator = (parameter[0] == 'N') ? ATK_MO1218_LATITUDE_NORTH : ATK_MO1218_LATITUDE_SOUTH;
    
    /* Longitude */
    ret  = atk_mo1218_decode_nmea_parameter(xxgll_msg, 3, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    atk_mo1218_fit_num(&_num, float_len, 5);
    decode_msg->longitude.degree = (_num / 10000000) * 100000;
    decode_msg->longitude.degree += ((_num % 10000000) / 60);
    
    /* E/W Indicator */
    ret = atk_mo1218_decode_nmea_parameter(xxgll_msg, 4, &parameter, NULL);
    if ((ret != ATK_MO1218_EOK) || ((parameter[0] != 'E') && (parameter[0] != 'W')))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->longitude.indicator = (parameter[0] == 'E') ? ATK_MO1218_LONGITUDE_EAST : ATK_MO1218_LONGITUDE_WEST;
    
    /* UTC Time */
    ret  = atk_mo1218_decode_nmea_parameter(xxgll_msg, 5, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if ((ret != ATK_MO1218_EOK) || (float_len != 3))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->utc_time.hour = (_num / 10000000) % 100;
    decode_msg->utc_time.minute = (_num / 100000) % 100;
    decode_msg->utc_time.second = (_num / 1000) % 100;
    decode_msg->utc_time.millisecond = (_num / 1) % 1000;
    
    /* Status */
    ret = atk_mo1218_decode_nmea_parameter(xxgll_msg, 6, &parameter, NULL);
    if ((ret != ATK_MO1218_EOK) || ((parameter[0] != 'A') && (parameter[0] != 'V')))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->status = (parameter[0] == 'A') ? ATK_MO1218_XXGLL_DATA_VALID : ATK_MO1218_XXGLL_DATA_NO_VALID;
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       ����$XXGSA���͵�NMEA��Ϣ
 * @param       xxgsa_msg : ��������$XXGSA����NMEA��Ϣ
 *              decode_msg: �����������
 * @retval      ATK_MO1218_EOK   : ����$XXGSA���͵�NMEA��Ϣ�ɹ�
 *              ATK_MO1218_ERROR : ����$XXGSA���͵�NMEA��Ϣʧ��
 *              ATK_MO1218_EINVAL: ������������
 */
uint8_t atk_mo1218_decode_nmea_xxgsa(uint8_t *xxgsa_msg, atk_mo1218_nmea_gsa_msg_t *decode_msg)
{
    uint8_t ret;
    uint8_t *parameter;
    uint16_t parameter_len = 0;
    int32_t _num;
    uint8_t float_len;
    uint8_t satellite_index;
    
    if ((xxgsa_msg == NULL) || (decode_msg == NULL))
    {
        return ATK_MO1218_EINVAL;
    }
    
    /* Address */
    ret = atk_mo1218_decode_nmea_parameter(xxgsa_msg, 0, &parameter, NULL);
    if ((ret != ATK_MO1218_EOK) || ((parameter[2] != 'G') || (parameter[3] != 'S') || (parameter[4] != 'A')))
    {
        return ATK_MO1218_EINVAL;
    }
    
    /* Mode */
    ret = atk_mo1218_decode_nmea_parameter(xxgsa_msg, 1, &parameter, NULL);
    if ((ret != ATK_MO1218_EOK) || ((parameter[0] != 'M') && (parameter[0] != 'A')))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->mode = (parameter[0] == 'M') ? ATK_MO1218_GPS_OPERATING_MANUAL : ATK_MO1218_GPS_OPERATING_AUTOMATIC;
    
    /* Fix type */
    ret  = atk_mo1218_decode_nmea_parameter(xxgsa_msg, 2, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if ((ret != ATK_MO1218_EOK) || (float_len != 0))
    {
        return ATK_MO1218_ERROR;
    }
    switch (_num)
    {
        case 1:
        {
            decode_msg->type = ATK_MO1218_FIX_NOT_AVAILABLE;
            break;
        }
        case 2:
        {
            decode_msg->type = ATK_MO1218_FIX_2D;
            break;
        }
        case 3:
        {
            decode_msg->type = ATK_MO1218_FIX_3D;
            break;
        }
        default:
        {
            return ATK_MO1218_ERROR;
        }
    }
    
    /* Sstellite ID */
    for (satellite_index=0; satellite_index<12; satellite_index++)
    {
        ret = atk_mo1218_decode_nmea_parameter(xxgsa_msg, 3 + satellite_index, &parameter, &parameter_len);
        if (ret != ATK_MO1218_EOK)
        {
            return ATK_MO1218_ERROR;
        }
        if (parameter_len == 0)
        {
            decode_msg->satellite_id[satellite_index] = 0;
            continue;
        }
        
        ret = atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
        if ((ret != ATK_MO1218_EOK) || (float_len != 0))
        {
            return ATK_MO1218_ERROR;
        }
        decode_msg->satellite_id[satellite_index] = _num;
    }
    
    /* PDOP */
    ret  = atk_mo1218_decode_nmea_parameter(xxgsa_msg, 15, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    atk_mo1218_fit_num(&_num, float_len, 1);
    decode_msg->pdop = _num;
    
    /* HDOP */
    ret  = atk_mo1218_decode_nmea_parameter(xxgsa_msg, 16, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    atk_mo1218_fit_num(&_num, float_len, 1);
    decode_msg->hdop = _num;
    
    /* VDOP */
    ret  = atk_mo1218_decode_nmea_parameter(xxgsa_msg, 17, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    atk_mo1218_fit_num(&_num, float_len, 1);
    decode_msg->vdop = _num;
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       ����$XXGSV���͵�NMEA��Ϣ
 * @param       xxgsv_msg : ��������$XXGSV����NMEA��Ϣ
 *              decode_msg: �����������
 * @retval      ATK_MO1218_EOK   : ����$XXGSV���͵�NMEA��Ϣ�ɹ�
 *              ATK_MO1218_ERROR : ����$XXGSV���͵�NMEA��Ϣʧ��
 *              ATK_MO1218_EINVAL: ������������
 */
uint8_t atk_mo1218_decode_nmea_xxgsv(uint8_t *xxgsv_msg, atk_mo1218_nmea_gsv_msg_t *decode_msg)
{
    uint8_t ret;
    uint8_t *parameter;
    uint16_t parameter_len;
    int32_t _num;
    uint8_t float_len;
    uint8_t msg_num;
    uint8_t msg_index;
    uint8_t *_xxgsv_msg;
    atk_mo1218_nmea_msg_t nmea_type;
    uint8_t satellite_index;
    
    if ((xxgsv_msg == NULL) || (decode_msg == NULL))
    {
        return ATK_MO1218_EINVAL;
    }
    
    /* Address */
    ret = atk_mo1218_decode_nmea_parameter(xxgsv_msg, 0, &parameter, NULL);
    if ((ret != ATK_MO1218_EOK) || ((parameter[2] != 'G') || (parameter[3] != 'S') || (parameter[4] != 'V')))
    {
        return ATK_MO1218_EINVAL;
    }
    if ((parameter[0] == 'G') && (parameter[1] == 'P'))
    {
        nmea_type = ATK_MO1218_NMEA_MSG_GPGSV;
    }
    else if ((parameter[0] == 'B') && (parameter[1] == 'D'))
    {
        nmea_type = ATK_MO1218_NMEA_MSG_BDGSV;
    }
    else
    {
        return ATK_MO1218_ERROR;
    }
    
    /* Number of message */
    ret  = atk_mo1218_decode_nmea_parameter(xxgsv_msg, 1, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if ((ret != ATK_MO1218_EOK) || (float_len != 0))
    {
        return ATK_MO1218_ERROR;
    }
    msg_num = _num;
    
    /* Satellite in view */
    ret  = atk_mo1218_decode_nmea_parameter(xxgsv_msg, 3, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if ((ret != ATK_MO1218_EOK) || (float_len != 0) || (_num < (msg_num - 1) * 4) || (_num > msg_num * 4))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->satellite_view = _num;
    
    if (decode_msg->satellite_view > 12)
    {
        decode_msg->satellite_view = 12;
        msg_num = (12 / 4) + (((12 % 4) == 0) ? 0 : 1);
    }
    
    for (msg_index=0; msg_index<msg_num; msg_index++)
    {
        ret = atk_mo1218_get_nmea_msg_from_buf(xxgsv_msg, nmea_type, msg_index + 1, &_xxgsv_msg);
        if (ret != ATK_MO1218_EOK)
        {
            return ATK_MO1218_ERROR;
        }
        
        /* Number of message */
        ret  = atk_mo1218_decode_nmea_parameter(_xxgsv_msg, 1, &parameter, NULL);
        ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
        if ((ret != ATK_MO1218_EOK) || (float_len != 0) || (_num < msg_num))
        {
            return ATK_MO1218_ERROR;
        }
        
        /* Sequence number */
        ret  = atk_mo1218_decode_nmea_parameter(_xxgsv_msg, 2, &parameter, NULL);
        ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
        if ((ret != ATK_MO1218_EOK) || (float_len != 0) || (_num != msg_index + 1))
        {
            return ATK_MO1218_ERROR;
        }
        
        /* Satellite in view */
        ret  = atk_mo1218_decode_nmea_parameter(_xxgsv_msg, 3, &parameter, NULL);
        ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
        if ((ret != ATK_MO1218_EOK) || (float_len != 0) || (_num < decode_msg->satellite_view))
        {
            return ATK_MO1218_ERROR;
        }
        
        
        for (satellite_index=0; satellite_index<4; satellite_index++)
        {
            /* Satellite ID */
            ret = atk_mo1218_decode_nmea_parameter(_xxgsv_msg, 4 + 4 * satellite_index + 0, &parameter, NULL);
            ret = atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
            if ((ret != ATK_MO1218_EOK) || (float_len != 0))
            {
                return ATK_MO1218_ERROR;
            }
            decode_msg->satellite_info[msg_index * 4 + satellite_index].satellite_id = _num;
            
            /* Elevation */
            ret  = atk_mo1218_decode_nmea_parameter(_xxgsv_msg, 4 + 4 * satellite_index + 1, &parameter, NULL);
            ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
            if ((ret != ATK_MO1218_EOK) || (float_len != 0))
            {
                return ATK_MO1218_ERROR;
            }
            decode_msg->satellite_info[msg_index * 4 + satellite_index].elevation = _num;
            
            /* Azimuth */
            ret  = atk_mo1218_decode_nmea_parameter(_xxgsv_msg, 4 + 4 * satellite_index + 2, &parameter, NULL);
            ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
            if ((ret != ATK_MO1218_EOK) || (float_len != 0))
            {
                return ATK_MO1218_ERROR;
            }
            decode_msg->satellite_info[msg_index * 4 + satellite_index].azimuth = _num;
            
            /* SNR */
            ret = atk_mo1218_decode_nmea_parameter(_xxgsv_msg, 4 + 4 * satellite_index + 3, &parameter, &parameter_len);
            if (ret != ATK_MO1218_EOK)
            {
                return ATK_MO1218_ERROR;
            }
            if (parameter_len == 0)
            {
                decode_msg->satellite_info[msg_index * 4 + satellite_index].snr = 0;
                if ((msg_index == msg_num - 1) && (satellite_index == (4 - (decode_msg->satellite_view % 4)) - 1))
                {
                    break;
                }
                else
                {
                    continue;
                }
            }
            ret = atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
            if ((ret != ATK_MO1218_EOK) || (float_len != 0))
            {
                return ATK_MO1218_ERROR;
            }
            decode_msg->satellite_info[msg_index * 4 + satellite_index].snr = _num;
            
            if ((msg_index == msg_num - 1) && (satellite_index == (4 - (decode_msg->satellite_view % 4)) - 1))
            {
                break;
            }
        }
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       ����$XXRMC���͵�NMEA��Ϣ
 * @param       xxrmc_msg : ��������$XXRMC����NMEA��Ϣ
 *              decode_msg: �����������
 * @retval      ATK_MO1218_EOK   : ����$XXRMC���͵�NMEA��Ϣ�ɹ�
 *              ATK_MO1218_ERROR : ����$XXRMC���͵�NMEA��Ϣʧ��
 *              ATK_MO1218_EINVAL: ������������
 */
uint8_t atk_mo1218_decode_nmea_xxrmc(uint8_t *xxrmc_msg, atk_mo1218_nmea_rmc_msg_t *decode_msg)
{
    uint8_t ret;
    uint8_t *parameter;
    int32_t _num;
    uint8_t float_len;
    
    if ((xxrmc_msg == NULL) || (decode_msg == NULL))
    {
        return ATK_MO1218_EINVAL;
    }
    
    /* Address */
    ret = atk_mo1218_decode_nmea_parameter(xxrmc_msg, 0, &parameter, NULL);
    if ((ret != ATK_MO1218_EOK) || ((parameter[2] != 'R') || (parameter[3] != 'M') || (parameter[4] != 'C')))
    {
        return ATK_MO1218_EINVAL;
    }
    
    /* UTC Time */
    ret  = atk_mo1218_decode_nmea_parameter(xxrmc_msg, 1, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if ((ret != ATK_MO1218_EOK) || (float_len != 3))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->utc_time.hour = (_num / 10000000) % 100;
    decode_msg->utc_time.minute = (_num / 100000) % 100;
    decode_msg->utc_time.second = (_num / 1000) % 100;
    decode_msg->utc_time.millisecond = (_num / 1) % 1000;
    
    /* Status */
    ret = atk_mo1218_decode_nmea_parameter(xxrmc_msg, 2, &parameter, NULL);
    if ((ret != ATK_MO1218_EOK) || ((parameter[0] != 'V') && (parameter[0] != 'A')))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->status = (parameter[0] == 'V') ? ATK_MO1218_NAVIGATION_WARNING : ATK_MO1218_NAVIGATION_VALID;
    
    /* Latitude */
    ret  = atk_mo1218_decode_nmea_parameter(xxrmc_msg, 3, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    atk_mo1218_fit_num(&_num, float_len, 5);
    decode_msg->latitude.degree = (_num / 10000000) * 100000;
    decode_msg->latitude.degree += ((_num % 10000000) / 60);
    
    /* N/S Indicator */
    ret = atk_mo1218_decode_nmea_parameter(xxrmc_msg, 4, &parameter, NULL);
    if ((ret != ATK_MO1218_EOK) || ((parameter[0] != 'N') && (parameter[0] != 'S')))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->latitude.indicator = (parameter[0] == 'N') ? ATK_MO1218_LATITUDE_NORTH : ATK_MO1218_LATITUDE_SOUTH;
    
    /* Longitude */
    ret  = atk_mo1218_decode_nmea_parameter(xxrmc_msg, 5, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    atk_mo1218_fit_num(&_num, float_len, 5);
    decode_msg->longitude.degree = (_num / 10000000) * 100000;
    decode_msg->longitude.degree += ((_num % 10000000) / 60);
    
    /* E/W Indicator */
    ret = atk_mo1218_decode_nmea_parameter(xxrmc_msg, 6, &parameter, NULL);
    if ((ret != ATK_MO1218_EOK) || ((parameter[0] != 'E') && (parameter[0] != 'W')))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->longitude.indicator = (parameter[0] == 'E') ? ATK_MO1218_LONGITUDE_EAST : ATK_MO1218_LONGITUDE_WEST;
    
    /* Speed over ground */
    ret  = atk_mo1218_decode_nmea_parameter(xxrmc_msg, 7, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    atk_mo1218_fit_num(&_num, float_len, 1);
    decode_msg->speed_ground = _num;
    
    /* Course over ground */
    ret  = atk_mo1218_decode_nmea_parameter(xxrmc_msg, 8, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    atk_mo1218_fit_num(&_num, float_len, 1);
    decode_msg->course_ground = _num;
    
    /* UTC Date */
    ret  = atk_mo1218_decode_nmea_parameter(xxrmc_msg, 9, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if ((ret != ATK_MO1218_EOK) || (float_len != 0))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->utc_date.year = (_num / 1) % 100 + 2000;
    decode_msg->utc_date.month = (_num / 100) % 100;
    decode_msg->utc_date.day = (_num / 10000) % 100;
    
    /* Mode indicator */
    ret = atk_mo1218_decode_nmea_parameter(xxrmc_msg, 12, &parameter, NULL);
    if ((ret != ATK_MO1218_EOK) || ((parameter[0] != 'N') && (parameter[0] != 'A') && (parameter[0] != 'D') && (parameter[0] != 'E')))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->position_system_mode = (parameter[0] == 'N') ? ATK_MO1218_POS_SYS_DATA_NOT_VALID : ((parameter[0] == 'A') ? ATK_MO1218_POS_SYS_AUTONOMOUS : ((parameter[0] == 'D') ? ATK_MO1218_POS_SYS_DIFFERENTIAL : ATK_MO1218_POS_SYS_ESTIMATED));
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       ����$XXVTG���͵�NMEA��Ϣ
 * @param       xxvtg_msg : ��������$XXVTG����NMEA��Ϣ
 *              decode_msg: �����������
 * @retval      ATK_MO1218_EOK   : ����$XXVTG���͵�NMEA��Ϣ�ɹ�
 *              ATK_MO1218_ERROR : ����$XXVTG���͵�NMEA��Ϣʧ��
 *              ATK_MO1218_EINVAL: ������������
 */
uint8_t atk_mo1218_decode_nmea_xxvtg(uint8_t *xxvtg_msg, atk_mo1218_nmea_vtg_msg_t *decode_msg)
{
    uint8_t ret;
    uint8_t *parameter;
    uint16_t parameter_len;
    int32_t _num;
    uint8_t float_len;
    
    if ((xxvtg_msg == NULL) || (decode_msg == NULL))
    {
        return ATK_MO1218_EINVAL;
    }
    
    /* Address */
    ret = atk_mo1218_decode_nmea_parameter(xxvtg_msg, 0, &parameter, NULL);
    if ((ret != ATK_MO1218_EOK) || ((parameter[2] != 'V') || (parameter[3] != 'T') || (parameter[4] != 'G')))
    {
        return ATK_MO1218_EINVAL;
    }
    
    /* Course */
    ret  = atk_mo1218_decode_nmea_parameter(xxvtg_msg, 1, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if ((ret != ATK_MO1218_EOK) || (float_len != 1))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->course_true = _num;
    
    /* Course */
    ret = atk_mo1218_decode_nmea_parameter(xxvtg_msg, 3, &parameter, &parameter_len);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    if (parameter_len == 0)
    {
        decode_msg->course_magnetic = 0;
    }
    else
    {
        ret = atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
        if (ret != ATK_MO1218_EOK)
        {
            return ATK_MO1218_ERROR;
        }
        decode_msg->course_magnetic = _num;
    }
    
    /* Speed */
    ret  = atk_mo1218_decode_nmea_parameter(xxvtg_msg, 5, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if ((ret != ATK_MO1218_EOK) || (float_len != 1))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->speed_knots = _num;
    
    /* Speed */
    ret  = atk_mo1218_decode_nmea_parameter(xxvtg_msg, 7, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if ((ret != ATK_MO1218_EOK) || (float_len != 1))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->speed_kph = _num;
    
    /* Mode */
    ret = atk_mo1218_decode_nmea_parameter(xxvtg_msg, 9, &parameter, NULL);
    if ((ret != ATK_MO1218_EOK) || ((parameter[0] != 'N') && (parameter[0] != 'A') && (parameter[0] != 'D') && (parameter[0] != 'E')))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->position_system_mode = (parameter[0] == 'N') ? ATK_MO1218_POS_SYS_DATA_NOT_VALID : ((parameter[0] == 'A') ? ATK_MO1218_POS_SYS_AUTONOMOUS : ((parameter[0] == 'D') ? ATK_MO1218_POS_SYS_DIFFERENTIAL : ATK_MO1218_POS_SYS_ESTIMATED));
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       ����$XXZDA���͵�NMEA��Ϣ
 * @param       xxzda_msg : ��������$XXZDA����NMEA��Ϣ
 *              decode_msg: �����������
 * @retval      ATK_MO1218_EOK   : ����$XXZDA���͵�NMEA��Ϣ�ɹ�
 *              ATK_MO1218_ERROR : ����$XXZDA���͵�NMEA��Ϣʧ��
 *              ATK_MO1218_EINVAL: ������������
 */
uint8_t atk_mo1218_decode_nmea_xxzda(uint8_t *xxzda_msg, atk_mo1218_nmea_zda_msg_t *decode_msg)
{
    uint8_t ret;
    uint8_t *parameter;
    int32_t _num;
    uint8_t float_len;
    
    if ((xxzda_msg == NULL) || (decode_msg == NULL))
    {
        return ATK_MO1218_EINVAL;
    }
    
    /* Address */
    ret = atk_mo1218_decode_nmea_parameter(xxzda_msg, 0, &parameter, NULL);
    if ((ret != ATK_MO1218_EOK) || ((parameter[2] != 'Z') || (parameter[3] != 'D') || (parameter[4] != 'A')))
    {
        return ATK_MO1218_EINVAL;
    }
    
    /* UTC Time */
    ret  = atk_mo1218_decode_nmea_parameter(xxzda_msg, 1, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if ((ret != ATK_MO1218_EOK) || (float_len != 3))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->utc_time.hour = (_num / 10000000) % 100;
    decode_msg->utc_time.minute = (_num / 100000) % 100;
    decode_msg->utc_time.second = (_num / 1000) % 100;
    decode_msg->utc_time.millisecond = (_num / 1) % 1000;
    
    /* UTC day */
    ret  = atk_mo1218_decode_nmea_parameter(xxzda_msg, 2, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if ((ret != ATK_MO1218_EOK) || (float_len != 0))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->utc_date.day = _num;
    
    /* UTC month */
    ret  = atk_mo1218_decode_nmea_parameter(xxzda_msg, 3, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if ((ret != ATK_MO1218_EOK) || (float_len != 0))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->utc_date.month = _num;
    
    /* UTC year */
    ret  = atk_mo1218_decode_nmea_parameter(xxzda_msg, 4, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if ((ret != ATK_MO1218_EOK) || (float_len != 0))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->utc_date.year = _num;
    
    /* Local zone hours */
    ret  = atk_mo1218_decode_nmea_parameter(xxzda_msg, 5, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if ((ret != ATK_MO1218_EOK) || (float_len != 0))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->local_zone_hour = _num;
    
    /* Local zone mintues */
    ret  = atk_mo1218_decode_nmea_parameter(xxzda_msg, 6, &parameter, NULL);
    ret += atk_mo1218_nmea_str2num(parameter, &_num, &float_len);
    if ((ret != ATK_MO1218_EOK) || (float_len != 0))
    {
        return ATK_MO1218_ERROR;
    }
    decode_msg->local_zone_minute = _num;
    
    return ATK_MO1218_EOK;
}
