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

	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
	LCD_Init();			   	//��ʼ��LCD	
	KEY_Init();				//������ʼ��
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_Normal);//CAN��ʼ������ģʽ,������500Kbps

 	while(1)
	{

		key = KEY_Scan(0);
		if(key==KEY0_PRES)//KEY0����,����һ������
		{
			for(i = 0; i < 8; i++){
				canbuf[i] = i+100;
				if(res)LCD_ShowString(60+80,190,200,16,16,(u8*)"Failed");		//��ʾ����ʧ��
				else LCD_ShowString(60+80,190,200,16,16,(u8*)"OK  ");	 		//��ʾ���ͳɹ�				
			}
			res=Can_Send_Msg(canbuf,8);//����8���ֽ�
							   
		}
	}
}


