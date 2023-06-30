#ifndef __ATK_U2E_H
#define __ATK_U2E_H

#include "sys.h"

/**
 * ****************************************************************************
 * @file            u2e.c
 * @author          正点原子团队（ALIENTEK）
 * @version         V1.0
 * @data            2020-04-14
 * @brief           模块驱动代码
 * @copyright       Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 * ****************************************************************************
 * @attention       
 * 
 * 实验平台：正点原子STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 * 
 * 修改说明
 * V1.0 20200414
 * 第一次发布
 * ****************************************************************************
*/
/******************************************************************************************/
/* 引脚 定义 */

/* 模块恢复出厂设置引脚，输出低电平有效 */
#define U2E_MODULE_DF_PORT                  GPIOA
#define U2E_MODULE_DF_PIN                   GPIO_PIN_15
#define U2E_MODULE_DF_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA口时钟使能 */

/* 模块传输状态引脚，读电平 */
#define U2E_MODULE_ST_PORT                  GPIOA
#define U2E_MODULE_ST_PIN                   GPIO_PIN_4
#define U2E_MODULE_ST_CLK_ENABLE()          do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA口时钟使能 */

#define U2E_MODULE_DF(x)   do{ x ? \
                              HAL_GPIO_WritePin(U2E_MODULE_DF_PORT, U2E_MODULE_DF_PIN, GPIO_PIN_SET) : \
                              HAL_GPIO_WritePin(U2E_MODULE_DF_PORT, U2E_MODULE_DF_PIN, GPIO_PIN_RESET); \
                           }while(0)
#define U2E_MODULE_ST      HAL_GPIO_ReadPin(U2E_MODULE_ST_PORT, U2E_MODULE_ST_PIN)     /* 读取状态引脚 */
/****************************************************************************/

#define U2E_RX_CMD_BUF_SIZE (512)

typedef enum
{
    U2E_UDP_SERVER = 0,     /* UDP服务器 */
    U2E_UDP_CLIENT,         /* UDP客户端模式 */
    U2E_TCP_SERVER,         /* TCP服务器 */
    U2E_TCP_CLIENT,         /* TCP客户端 */
    U2E_ATK_CLOUD,          /* 原子云 */
    U2E_UDP_MULTICAST,      /* UDP组播功能 */
} _u2e_work_mode_eu;

typedef struct
{
    uint32_t timeout; /*指令等待超时时间，单位：100ms*/
    char *read_cmd;   /*查询参数指令      请参考ATK-UART2ETH模块AT命令手册说明进行填写*/
    char *write_cmd;  /*配置参数指令      请参考ATK-UART2ETH模块AT命令手册说明进行填写*/
} _u2e_atcmd_st;

void u2e_get_urc_info(uint8_t ch);

void send_data_to_u2e(uint8_t *data, uint32_t size);

static int u2e_enter_transfermode(void);

int u2e_config_init(_u2e_work_mode_eu work_mode);

#endif
