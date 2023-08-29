#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "timer.h"
#include "canMotor.h"
#include "can.h"

int main(void)
{
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_Normal); //CAN����ģʽ;CAN_Mode_Normal(0)����ͨģʽ��CAN_Mode_LoopBack(1)������ģʽ
	TIM3_NVIC_Configuration();
	TIM3_PWM_Init(ARR_5000-1, PSC-1);    //72000000/(7199+1)/(199+1)=50Hz
	printf("�������������");
	delay_ms(100);
	while(1)
	{

		execute();
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

