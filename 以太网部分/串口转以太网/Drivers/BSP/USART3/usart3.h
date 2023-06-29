/**
 ****************************************************************************************************
 * @file        rs485.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       USART3 驱动代码
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
 * 修改说明
 * V1.0 20200424
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __USART3_H
#define __USART3_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* USART3 引脚 和 串口 定义 
 * 默认是针对USART3的.
 * 注意: 通过修改这个宏定义, 可以支持USART1~UART7任意一个串口.
 */
#define USART3_TX_GPIO_PORT                  GPIOB
#define USART3_TX_GPIO_PIN                   GPIO_PIN_10
#define USART3_TX_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB口时钟使能 */

#define USART3_RX_GPIO_PORT                  GPIOB
#define USART3_RX_GPIO_PIN                   GPIO_PIN_11
#define USART3_RX_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB口时钟使能 */

#define USART3_UX                            USART3
#define USART3_UX_IRQn                       USART3_IRQn
#define USART3_UX_IRQHandler                 USART3_IRQHandler
#define USART3_UX_CLK_ENABLE()               do{ __HAL_RCC_USART3_CLK_ENABLE(); }while(0)  /* USART3 时钟使能 */

/******************************************************************************************/

#define USART3_REC_LEN               256             /* 定义最大接收字节数 256 */

extern uint8_t g_USART3_rx_buf[USART3_REC_LEN];      /* 接收缓冲,最大USART3_REC_LEN个字节 */

void usart3_init( uint32_t baudrate);                /* USART3初始化 */
void usart3_send_data(uint8_t *buf, uint8_t len);    /* USART3发送数据 */
void usart3_receive_data(uint8_t *buf, uint8_t *len);/* USART3接收数据 */
void RingBuffer_Reset(void);
uint16_t usart3_getRxData(uint8_t *buf, uint16_t len);/* 尝试从串口缓存中取数据 */
uint8_t res_usart3_revdata(void);                     /* 检查串口是否有数据接收 */

#endif
















