#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"

/************************************************
 ALIENTEK精英STM32开发板实验4
 串口 实验   
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/
 int main(void)
 {		
 	u16 t;  
	u16 len;	
	u16 times=0;
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 //串口初始化为115200
	uart2_init(115200);
 	LED_Init();			     //LED端口初始化
	KEY_Init();          //初始化与按键连接的硬件接口
 	
	while(1)
	{
			if(USART_RX_STA2&0x8000)
			{
				len=USART_RX_STA2&0x3fff;//得到此次接收到的数据长度
				for(t=0;t<len;t++)
				{
					USART_SendData(USART1, USART_RX_BUF2[t]);//向串口1发送数据
					while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
				}
				
				if(USART_RX_BUF2[0] == 'u' && USART_RX_BUF2[1] == 'p'){
					printf("向上");
				}
				if(USART_RX_BUF2[0] == 'd' && USART_RX_BUF2[1] == 'o' && USART_RX_BUF2[2] == 'w' && USART_RX_BUF2[3] == 'n'){
						printf("向下");
				}
				if(USART_RX_BUF2[0] == 's' && USART_RX_BUF2[1] == 't' && USART_RX_BUF2[2] == 'o' && USART_RX_BUF2[3] == 'p'){
					printf("停");
				}
				USART_RX_STA2=0;
			}
	}
}
 

