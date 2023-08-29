#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "timer.h"
#include "canMotor.h"
#include "can.h"

int main(void)
{
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_Normal); //CAN工作模式;CAN_Mode_Normal(0)：普通模式，CAN_Mode_LoopBack(1)：环回模式
	TIM3_NVIC_Configuration();
	TIM3_PWM_Init(ARR_5000-1, PSC-1);    //72000000/(7199+1)/(199+1)=50Hz
	printf("舵机控制器开启");
	delay_ms(100);
	while(1)
	{

		execute();
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

