#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/ATK-UART2ETH/atk_u2e.h"
#include "./BSP/USART3/usart3.h"
#include "string.h"

/*******************************************/
char *Modbuf[]={
    "UDP_SERVER",
    "UDP_CLIENT",
    "TCP_Server",
    "TCP_Client",
    "TCP_CLOUD ",
    "UDP_MULTICAST",
};

char *test_buf[]={
    "Data from UDP_Server mode",
    "Data from UDP_Client mode",
    "Data from TCP_Server mode",
    "Data from TCP_Client mode",
    "CLOUD.ALIENTEK.COM",
    "Data from 233.0.0.6",
};

uint8_t g_data_rxbuf[256];       /* 用于接收数据的缓存 */

/**
 * @brief       串口1发送函数
 * @param       *buf : 发送缓冲区指针
 * @param       len  : 源地址
 * @param       n    : 需要复制的内存长度(字节为单位)
 * @retval      无
 */
void usart1_send_data(uint8_t* buf,uint32_t len)
{
    for(int32_t i=0;i<len;i++)
    {
        while ((USART_UX->SR & 0X40) == 0);     /* 等待上一个字符发送完成 */
        USART_UX->DR = buf[i];             /* 将要发送的字符 ch 写入到DR寄存器 */
    }
}







int key = 42;
void decrypt_data(char *data, int length, int key)
{
    for (int i = 0; i < length; i++)
    {
        data[i] = data[i] ^ key;
    }
}


int main(void)
{
    uint8_t key;
    uint8_t i = 0;
    uint16_t rx_len = 0;

    sys_stm32_clock_init(9);        /* 设置时钟, 72Mhz */
    delay_init(72);                 /* 延时初始化 */
    usart_init(115200);             /* 串口初始化为115200 */
    led_init();                     /* 初始化LED */
    key_init();                     /* 初始化按键 */

    u2e_config_init(U2E_TCP_CLIENT); /* 选择模式 */

    while (1)
    {
        key = key_scan(0);      /* 不支持连按 */
			
        switch (key)
        {
            case KEY0_PRES:     /* KEY0按下 */
                usart3_send_data((uint8_t*)test_buf[U2E_TCP_CLIENT],strlen((const char*)test_buf[U2E_TCP_CLIENT]));      /* 发送数据到原子云 */
                break;
            default:break;
        }

        /* 透传功能下，从队列中把所有数据提取出来，并转发到串口1 */
        while (res_usart3_revdata())
        {
            usart3_getRxData(&g_data_rxbuf[rx_len++], 1); /* 从串口缓存中读一个字节 */

            if (rx_len >= U2E_RX_CMD_BUF_SIZE)
            {
                rx_len = 0; /*接收应答数据超长，丢弃部分数据*/
            }
        }
        if(rx_len>0)
        {
            usart1_send_data(g_data_rxbuf,rx_len);       /* 转发到串口1 */
						printf("\r\n");
            rx_len = 0;
        }
				
        delay_ms(10);
        i++;

        if ((i % 20) == 0)
        {
            LED0_TOGGLE();  /* LED0闪烁 */
        }
    }
}




