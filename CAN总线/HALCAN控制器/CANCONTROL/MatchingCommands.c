/*		命令解析				*/
#include "MatchingCommands.h"
#include "canControl.h"
#include "string.h"
#include "atk_u2e.h"
#include "usart3.h"
#include "usart.h"





/**
 * @brief       串口1发送函数
 * @param       *buf : 发送缓冲区指针
 * @param       len  : 源地址
 * @param       n    : 需要复制的内存长度(字节为单位)
 * @retval      无
 */
void usart1_send_data(uint8_t* buf,uint32_t len)
{
	int32_t i = 0;
    for(i=0;i<len;i++)
    {
        while ((USART1->SR & 0X40) == 0);     /* 等待上一个字符发送完成 */
        USART1->DR = buf[i];             /* 将要发送的字符 ch 写入到DR寄存器 */
    }
}

void sendata(){
	
	uint16_t rx_len = 0;
	static uint8_t g_data_rxbuf[256];       /* 用于接收数据的缓存 */
	
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
	
}


