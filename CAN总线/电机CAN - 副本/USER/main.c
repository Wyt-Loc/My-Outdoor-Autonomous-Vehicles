#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "timer.h"
#include "canMotor.h"
#include "can.h"


int main(void)
{

	u8 *p = NULL;
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
	time3_initer(0xffff, 35); //���������ʱ��
	CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_Normal); //CAN����ģʽ;CAN_Mode_Normal(0)����ͨģʽ��CAN_Mode_LoopBack(1)������ģʽ
	
 	printf("����");
	
	while(1)
	{
		receData();
		
		//receData();
		
		//motorExecute(receData());

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

