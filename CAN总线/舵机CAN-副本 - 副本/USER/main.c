#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"	 
#include "timer.h"
#include "canMotor.h"
#include "can.h"


int main(void)
{
	uint16_t a = 0;
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
	//time3_initer(0xffff, 35); //步进电机定时器
	//CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_Normal); //CAN工作模式;CAN_Mode_Normal(0)：普通模式，CAN_Mode_LoopBack(1)：环回模式
	Servos_PWM_Init(ARR_5000-1, PSC-1); //舵机定时器初始化
	delay_ms(1000);
 	printf("舵机控制器开启");
	
	while(1)
	{
		
		printf("2");
		for (a = 50; a < 250; a++){
			TIM_SetCompare1(TIM4,a);
			delay_ms(100);
		}
		
		//execute();
		//receData();

//		isRece = Can_Receive_Msg(ReceBuf); //接收主节点数据
//		if(isRece)
//		{
//			for(i = 0; i < isRece / 2; i++){
//				sum += ReceBuf[i] & 0xE0;
//			}
//			ReceBuf[0] = sum;
//			while( isRece && !Can_Send_Msg(ReceBuf,8))
//			{
//				for(i=0;i<isRece;i++)
//				{
//						printf(" %d ",ReceBuf[i]);
//				}
//				isRece = 0;
//				printf("\r\n");
//			}
//		}
	}
}

