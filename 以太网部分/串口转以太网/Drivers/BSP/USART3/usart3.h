/**
 ****************************************************************************************************
 * @file        rs485.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       USART3 ��������
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
 * �޸�˵��
 * V1.0 20200424
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#ifndef __USART3_H
#define __USART3_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* USART3 ���� �� ���� ���� 
 * Ĭ�������USART3��.
 * ע��: ͨ���޸�����궨��, ����֧��USART1~UART7����һ������.
 */
#define USART3_TX_GPIO_PORT                  GPIOB
#define USART3_TX_GPIO_PIN                   GPIO_PIN_10
#define USART3_TX_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB��ʱ��ʹ�� */

#define USART3_RX_GPIO_PORT                  GPIOB
#define USART3_RX_GPIO_PIN                   GPIO_PIN_11
#define USART3_RX_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB��ʱ��ʹ�� */

#define USART3_UX                            USART3
#define USART3_UX_IRQn                       USART3_IRQn
#define USART3_UX_IRQHandler                 USART3_IRQHandler
#define USART3_UX_CLK_ENABLE()               do{ __HAL_RCC_USART3_CLK_ENABLE(); }while(0)  /* USART3 ʱ��ʹ�� */

/******************************************************************************************/

#define USART3_REC_LEN               256             /* �����������ֽ��� 256 */

extern uint8_t g_USART3_rx_buf[USART3_REC_LEN];      /* ���ջ���,���USART3_REC_LEN���ֽ� */

void usart3_init( uint32_t baudrate);                /* USART3��ʼ�� */
void usart3_send_data(uint8_t *buf, uint8_t len);    /* USART3�������� */
void usart3_receive_data(uint8_t *buf, uint8_t *len);/* USART3�������� */
void RingBuffer_Reset(void);
uint16_t usart3_getRxData(uint8_t *buf, uint16_t len);/* ���ԴӴ��ڻ�����ȡ���� */
uint8_t res_usart3_revdata(void);                     /* ��鴮���Ƿ������ݽ��� */

#endif
















