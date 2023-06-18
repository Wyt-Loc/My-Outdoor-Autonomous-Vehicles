#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "can.h" 


int main(void)
{

	u8 i;
	u8 ReceBuf[8];
	u8 isRece = 0;
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_Normal); //CAN工作模式;CAN_Mode_Normal(0)：普通模式，CAN_Mode_LoopBack(1)：环回模式 
 	
	while(1)
	{

		isRece = Can_Receive_Msg(ReceBuf); //接收主节点数据
		if(isRece)
		{
			for(i=0;i<isRece;i++)
			{
					printf(" %d ",ReceBuf[i]);
			}
			printf("\r\n");
		}
	}
}


