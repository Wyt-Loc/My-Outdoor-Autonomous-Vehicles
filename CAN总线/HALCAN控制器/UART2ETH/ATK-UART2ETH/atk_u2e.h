#ifndef __ATK_U2E_H
#define __ATK_U2E_H

#include "sys.h"

/**
 * ****************************************************************************
 * @file            u2e.c
 * @author          ����ԭ���Ŷӣ�ALIENTEK��
 * @version         V1.0
 * @data            2020-04-14
 * @brief           ģ����������
 * @copyright       Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 * ****************************************************************************
 * @attention       
 * 
 * ʵ��ƽ̨������ԭ��STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 * 
 * �޸�˵��
 * V1.0 20200414
 * ��һ�η���
 * ****************************************************************************
*/
/******************************************************************************************/
/* ���� ���� */

/* ģ��ָ������������ţ�����͵�ƽ��Ч */
#define U2E_MODULE_DF_PORT                  GPIOA
#define U2E_MODULE_DF_PIN                   GPIO_PIN_15
#define U2E_MODULE_DF_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA��ʱ��ʹ�� */

/* ģ�鴫��״̬���ţ�����ƽ */
#define U2E_MODULE_ST_PORT                  GPIOA
#define U2E_MODULE_ST_PIN                   GPIO_PIN_4
#define U2E_MODULE_ST_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA��ʱ��ʹ�� */

#define U2E_MODULE_DF(x)   do{ x ? \
                              HAL_GPIO_WritePin(U2E_MODULE_DF_PORT, U2E_MODULE_DF_PIN, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(U2E_MODULE_DF_PORT, U2E_MODULE_DF_PIN, GPIO_PIN_RESET); \
                           }while(0)
#define U2E_MODULE_ST      HAL_GPIO_ReadPin(U2E_MODULE_ST_PORT, U2E_MODULE_ST_PIN)     /* ��ȡ״̬���� */
/****************************************************************************/

#define U2E_RX_CMD_BUF_SIZE (512)

typedef enum
{
    U2E_UDP_SERVER = 0,     /* UDP������ */
    U2E_UDP_CLIENT,         /* UDP�ͻ���ģʽ */
    U2E_TCP_SERVER,         /* TCP������ */
    U2E_TCP_CLIENT,         /* TCP�ͻ��� */
    U2E_ATK_CLOUD,          /* ԭ���� */
    U2E_UDP_MULTICAST,      /* UDP�鲥���� */
} _u2e_work_mode_eu;

typedef struct
{
    uint32_t timeout; /*ָ��ȴ���ʱʱ�䣬��λ��100ms*/
    char *read_cmd;   /*��ѯ����ָ��      ��ο�ATK-UART2ETHģ��AT�����ֲ�˵��������д*/
    char *write_cmd;  /*���ò���ָ��      ��ο�ATK-UART2ETHģ��AT�����ֲ�˵��������д*/
} _u2e_atcmd_st;

void u2e_get_urc_info(uint8_t ch);

void send_data_to_u2e(uint8_t *data, uint32_t size);

static int u2e_enter_transfermode(void);

int u2e_config_init(_u2e_work_mode_eu work_mode);

#endif
