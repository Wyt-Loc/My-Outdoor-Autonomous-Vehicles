/**
 ****************************************************************************************************
 * @file        usart3.c
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

#include "usart3.h"
#include "delay.h"
#include "ringbuffer.h"

UART_HandleTypeDef g_usart3_handler;        /* USART3���ƾ��(����) */

uint8_t g_usart3_rx_buf[USART3_REC_LEN];    /* ���ջ���, ��� USART3_REC_LEN ���ֽ�. */
ringbuffer_t uart3RxFifo;

/**
 * @brief       ���ý��ջ���
 * @note        ���ڳ�ʼ�����ڽ����ڴ棬���ߵ��ô˽ӿڶ��������ѻ�������
 * @param       ��
 * @retval      ��
 */
void RingBuffer_Reset(void)
{
    ringbuffer_init(&uart3RxFifo, g_usart3_rx_buf, USART3_REC_LEN);
}

/**
 * @brief       ��鴮���Ƿ��������յ�
 * @param       ��
 * @retval uint8_t: 0 Ϊ�� 1 ��Ϊ�գ������ݣ�
 */
uint8_t res_usart3_revdata()
{
   return !(ringbuffer_isEmpty(&uart3RxFifo));
}


/**
 * @brief       ���ڽ����жϴ�����
 * @param       ��
 * @retval      ��
 */
void USART3_UX_IRQHandler(void)
{
    uint8_t res;

    if ((__HAL_UART_GET_FLAG(&g_usart3_handler, UART_FLAG_RXNE) != RESET)) /* ���յ����� */
    {
        res = g_usart3_handler.Instance->DR;
        ringbuffer_in_check(&uart3RxFifo, (uint8_t *)&res, 1); /*!< �����յ������ݷ���FIFO */
    }
}

/**
 * @brief       USART3��ʼ������
 *   @note      �ú�����Ҫ�ǳ�ʼ������
 * @param       baudrate: ������, �����Լ���Ҫ���ò�����ֵ
 * @retval      ��
 */
void usart3_init(uint32_t baudrate)
{
	
	GPIO_InitTypeDef  gpio_initure;
	
    /* IO �� ʱ������ */
    USART3_TX_GPIO_CLK_ENABLE(); /* ʹ�� ����TX�� ʱ�� */
    USART3_RX_GPIO_CLK_ENABLE(); /* ʹ�� ����RX�� ʱ�� */
    USART3_UX_CLK_ENABLE();      /* ʹ�� ���� ʱ�� */

    gpio_initure.Pin = USART3_TX_GPIO_PIN;
    gpio_initure.Mode = GPIO_MODE_AF_PP;
    gpio_initure.Pull = GPIO_PULLUP;
    gpio_initure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(USART3_TX_GPIO_PORT, &gpio_initure); /* ����TX �� ģʽ���� */

    gpio_initure.Pin = USART3_RX_GPIO_PIN;
    gpio_initure.Mode = GPIO_MODE_AF_INPUT;
    HAL_GPIO_Init(USART3_RX_GPIO_PORT, &gpio_initure); /* ����RX �� �������ó�����ģʽ */

    /* USART ��ʼ������ */
    g_usart3_handler.Instance = USART3_UX;                  /* ѡ��485��Ӧ�Ĵ��� */
    g_usart3_handler.Init.BaudRate = baudrate;             /* ������ */
    g_usart3_handler.Init.WordLength = UART_WORDLENGTH_8B; /* �ֳ�Ϊ8λ���ݸ�ʽ */
    g_usart3_handler.Init.StopBits = UART_STOPBITS_1;      /* һ��ֹͣλ */
    g_usart3_handler.Init.Parity = UART_PARITY_NONE;       /* ����żУ��λ */
    g_usart3_handler.Init.HwFlowCtl = UART_HWCONTROL_NONE; /* ��Ӳ������ */
    g_usart3_handler.Init.Mode = UART_MODE_TX_RX;          /* �շ�ģʽ */
    HAL_UART_Init(&g_usart3_handler);                      /* ʹ�ܶ�Ӧ�Ĵ���, �������MSp */
    __HAL_UART_DISABLE_IT(&g_usart3_handler, UART_IT_TC);

    /* ʹ�ܽ����ж� */
    __HAL_UART_ENABLE_IT(&g_usart3_handler, UART_IT_RXNE); /* ���������ж� */
    HAL_NVIC_EnableIRQ(USART3_UX_IRQn);                    /* ʹ��USART1�ж� */
    HAL_NVIC_SetPriority(USART3_UX_IRQn, 0, 3);            /* ��ռ���ȼ�3�������ȼ�3 */

    RingBuffer_Reset();                                    /* ��ʼ�����ջ��� */
}

/**
 * @brief       USART3����len���ֽ�
 * @param       buf     : �������׵�ַ
 * @param       len     : ���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ���� USART3_REC_LEN ���ֽ�)
 * @retval      ��
 */
void usart3_send_data(uint8_t *buf, uint8_t len)
{
    HAL_UART_Transmit(&g_usart3_handler, buf, len, 1000); /* ����3�������� */
}

/**
  * @brief  ��ȡ����3����fifo������
  * @param  buf: ��ŵĻ�����
  * @param  len: ��Ҫ��ȡ�ĳ���
  * @retval uint16_t: ʵ�ʻ�ȡ���ĳ��� 0��ʾû�����ݿɻ�ȡ
  */
uint16_t usart3_getRxData(uint8_t *buf, uint16_t len)
{
    return ringbuffer_out(&uart3RxFifo, buf, len);
}
