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
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_Normal); //CAN����ģʽ;CAN_Mode_Normal(0)����ͨģʽ��CAN_Mode_LoopBack(1)������ģʽ 
 	
	while(1)
	{

		isRece = Can_Receive_Msg(ReceBuf); //�������ڵ�����
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


