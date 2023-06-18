#include "lcd.h"
#include "usart.h"	 
#include "can.h" 
#include "delay.h"
#include "key.h"
#include "timer.h"
#include "stdio.h"


int main(void)
{

	u8 key;
	u8 i=0,t=0;
	u8 cnt=0;
	u8 canbuf[8];
	u8 res;

	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
	LCD_Init();			   	//初始化LCD	
	KEY_Init();				//按键初始化
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_Normal);//CAN初始化环回模式,波特率500Kbps

 	while(1)
	{

		key = KEY_Scan(0);
		if(key==KEY0_PRES)//KEY0按下,发送一次数据
		{
			for(i = 0; i < 8; i++){
				canbuf[i] = i+100;
				if(res)LCD_ShowString(60+80,190,200,16,16,(u8*)"Failed");		//提示发送失败
				else LCD_ShowString(60+80,190,200,16,16,(u8*)"OK  ");	 		//提示发送成功				
			}
			res=Can_Send_Msg(canbuf,8);//发送8个字节
							   
		}
	}
}


