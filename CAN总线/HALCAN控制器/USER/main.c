#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "usart3.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "usmart.h"
#include "can.h"
#include "timer.h"
#include "canControl.h"
#include "string.h"
#include "atk_u2e.h"


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
	int32_t i = 0;
    for(i=0;i<len;i++)
    {
        while ((USART1->SR & 0X40) == 0);     /* 等待上一个字符发送完成 */
        USART1->DR = buf[i];             /* 将要发送的字符 ch 写入到DR寄存器 */
    }
}


int main(void)
{
	
  u8 key;
	uint8_t i = 0;
  uint16_t rx_len = 0;
	
  HAL_Init();                    	 	//初始化HAL库    
  Stm32_Clock_Init(RCC_PLL_MUL9);   	//设置时钟,72M
	delay_init(72);               		//初始化延时函数
	uart_init(115200);					//初始化串口
	usmart_dev.init(84); 		   		//初始化USMART	
	LED_Init();							//初始化LED	
	KEY_Init();							//初始化按键
 	CAN1_Mode_Init(CAN_SJW_1TQ,CAN_BS2_8TQ,CAN_BS1_9TQ,4,CAN_MODE_NORMAL); //CAN初始化,波特率500Kbps      
	CAN_Config();
	TIM3_Init(100-1,72-1);    // 每秒计数10000次
	u2e_config_init(U2E_TCP_CLIENT); /* 选择模式 */
	
	while (1)
	{
			key = KEY_Scan(0);      /* 不支持连按 */
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
					LED1 = ~LED1;  /* LED0闪烁 */
			}
	}
	
	/*
	while(1){
		key = KEY_Scan(0);
		if(key==KEY0_PRES)
		{
			ControlEntryFunction(1, 13.14f, 9.99f);
			key = 0;
		}
	}
	*/

}

