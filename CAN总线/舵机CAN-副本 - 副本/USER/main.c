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
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
	//time3_initer(0xffff, 35); //���������ʱ��
	//CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_Normal); //CAN����ģʽ;CAN_Mode_Normal(0)����ͨģʽ��CAN_Mode_LoopBack(1)������ģʽ
	Servos_PWM_Init(ARR_5000-1, PSC-1); //�����ʱ����ʼ��
	delay_ms(1000);
 	printf("�������������");
	
	while(1)
	{
		
		printf("2");
		for (a = 50; a < 250; a++){
			TIM_SetCompare1(TIM4,a);
			delay_ms(100);
		}
		
		//execute();
		//receData();

//		isRece = Can_Receive_Msg(ReceBuf); //�������ڵ�����
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

