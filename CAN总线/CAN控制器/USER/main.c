#include "usart.h"	 
#include "can.h" 
#include "delay.h"
#include "key.h"
#include "timer.h"
#include "stdio.h"
#include "crcLib.h"
#include "canControl.h"
 

int main(void)
{

	u8 key;
	u8 buf[8]={1,2,3,4,5,6,7,8};

	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
	KEY_Init();				//按键初始化
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_Normal);//CAN初始化环回模式,波特率500Kbps
	//BaseTimer_Init();
	
 	while(1)
	{

		key = KEY_Scan(0);
		if(key==3)
		{
			printf("AAA");
			ControlEntryFunction(1, 13.14f, 9.99f);
			//Can_Send_Msg(buf,8);
		}

	}
}

