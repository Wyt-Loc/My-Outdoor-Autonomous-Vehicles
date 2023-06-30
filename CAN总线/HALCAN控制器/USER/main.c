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


int main(void)
{
	
  u8 key;

	
  HAL_Init();                    	 	//初始化HAL库
  Stm32_Clock_Init(RCC_PLL_MUL9);   	//设置时钟,72M
	delay_init(72);               		//初始化延时函数
	uart_init(115200);					//初始化串口
	LED_Init();							//初始化LED
	KEY_Init();							//初始化按键
 	CAN1_Mode_Init(CAN_SJW_1TQ,CAN_BS2_8TQ,CAN_BS1_9TQ,4,CAN_MODE_NORMAL); //CAN初始化,波特率500Kbps      
	CAN_Config();
	TIM3_Init(100-1,72-1);    // 每秒计数10000次 can消息计数用
	u2e_config_init(U2E_TCP_CLIENT); /* 选择模式 */
	
	while (1)
	{
			key = KEY_Scan(0);      /* 不支持连按 */
			switch (key)
			{
					case KEY0_PRES:     /* KEY0按下 */
							usart3_send_data((uint8_t*)test_buf[U2E_TCP_CLIENT],strlen((const char*)test_buf[U2E_TCP_CLIENT]));      /* 发送数据到服务器 */
							break;
					default:break;
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

