/**
 ****************************************************************************************************
 * @file        usart3.c
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

#include "usart3.h"
#include "delay.h"
#include "ringbuffer.h"

UART_HandleTypeDef g_usart3_handler;        /* USART3控制句柄(串口) */

uint8_t g_usart3_rx_buf[USART3_REC_LEN];    /* 接收缓冲, 最大 USART3_REC_LEN 个字节. */
ringbuffer_t uart3RxFifo;

/**
 * @brief       重置接收缓存
 * @note        用于初始化串口接收内存，或者调用此接口丢弃串口已缓存数据
 * @param       无
 * @retval      无
 */
void RingBuffer_Reset(void)
{
    ringbuffer_init(&uart3RxFifo, g_usart3_rx_buf, USART3_REC_LEN);
}

/**
 * @brief       检查串口是否有数据收到
 * @param       无
 * @retval uint8_t: 0 为空 1 不为空（有数据）
 */
uint8_t res_usart3_revdata()
{
   return !(ringbuffer_isEmpty(&uart3RxFifo));
}


/**
 * @brief       串口接收中断处理函数
 * @param       无
 * @retval      无
 */
void USART3_UX_IRQHandler(void)
{
    uint8_t res;

    if ((__HAL_UART_GET_FLAG(&g_usart3_handler, UART_FLAG_RXNE) != RESET)) /* 接收到数据 */
    {
        res = g_usart3_handler.Instance->DR;
        ringbuffer_in_check(&uart3RxFifo, (uint8_t *)&res, 1); /*!< 将接收到的数据放入FIFO */
    }
}

/**
 * @brief       USART3初始化函数
 *   @note      该函数主要是初始化串口
 * @param       baudrate: 波特率, 根据自己需要设置波特率值
 * @retval      无
 */
void usart3_init(uint32_t baudrate)
{
	
	GPIO_InitTypeDef  gpio_initure;
	
    /* IO 及 时钟配置 */
    USART3_TX_GPIO_CLK_ENABLE(); /* 使能 串口TX脚 时钟 */
    USART3_RX_GPIO_CLK_ENABLE(); /* 使能 串口RX脚 时钟 */
    USART3_UX_CLK_ENABLE();      /* 使能 串口 时钟 */

    gpio_initure.Pin = USART3_TX_GPIO_PIN;
    gpio_initure.Mode = GPIO_MODE_AF_PP;
    gpio_initure.Pull = GPIO_PULLUP;
    gpio_initure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(USART3_TX_GPIO_PORT, &gpio_initure); /* 串口TX 脚 模式设置 */

    gpio_initure.Pin = USART3_RX_GPIO_PIN;
    gpio_initure.Mode = GPIO_MODE_AF_INPUT;
    HAL_GPIO_Init(USART3_RX_GPIO_PORT, &gpio_initure); /* 串口RX 脚 必须设置成输入模式 */

    /* USART 初始化设置 */
    g_usart3_handler.Instance = USART3_UX;                  /* 选择485对应的串口 */
    g_usart3_handler.Init.BaudRate = baudrate;             /* 波特率 */
    g_usart3_handler.Init.WordLength = UART_WORDLENGTH_8B; /* 字长为8位数据格式 */
    g_usart3_handler.Init.StopBits = UART_STOPBITS_1;      /* 一个停止位 */
    g_usart3_handler.Init.Parity = UART_PARITY_NONE;       /* 无奇偶校验位 */
    g_usart3_handler.Init.HwFlowCtl = UART_HWCONTROL_NONE; /* 无硬件流控 */
    g_usart3_handler.Init.Mode = UART_MODE_TX_RX;          /* 收发模式 */
    HAL_UART_Init(&g_usart3_handler);                      /* 使能对应的串口, 但会调用MSp */
    __HAL_UART_DISABLE_IT(&g_usart3_handler, UART_IT_TC);

    /* 使能接收中断 */
    __HAL_UART_ENABLE_IT(&g_usart3_handler, UART_IT_RXNE); /* 开启接收中断 */
    HAL_NVIC_EnableIRQ(USART3_UX_IRQn);                    /* 使能USART1中断 */
    HAL_NVIC_SetPriority(USART3_UX_IRQn, 0, 3);            /* 抢占优先级3，子优先级3 */

    RingBuffer_Reset();                                    /* 初始化接收缓存 */
}

/**
 * @brief       USART3发送len个字节
 * @param       buf     : 发送区首地址
 * @param       len     : 发送的字节数(为了和本代码的接收匹配,这里建议不要超过 USART3_REC_LEN 个字节)
 * @retval      无
 */
void usart3_send_data(uint8_t *buf, uint8_t len)
{
    HAL_UART_Transmit(&g_usart3_handler, buf, len, 1000); /* 串口3发送数据 */
}

/**
  * @brief  获取串口3接收fifo的数据
  * @param  buf: 存放的缓冲区
  * @param  len: 需要获取的长度
  * @retval uint16_t: 实际获取到的长度 0表示没有数据可获取
  */
uint16_t usart3_getRxData(uint8_t *buf, uint16_t len)
{
    return ringbuffer_out(&uart3RxFifo, buf, len);
}
