/* 					CANBUS ���Ƶ�� 				*/

#include "canMotor.h"
#include "MotorT.h"
#include "can.h"
#include "stdio.h"
#include "timer.h"


/***************************************************************
���������� u8* receData(void)
����  �ܡ� �õ�CAN��������
������ֵ�� ��
������ֵ�� ����
****************************************************************/
static u8 ReceBuf[8]; // ���ν��յ�����
u8 old = 0;
u8 receData(void){

	u8 i = 0;
	u8 sum = 0;
	static u8 isRece = 0;

		isRece = Can_Receive_Msg(ReceBuf); //�������ڵ�����
		if(isRece)
		{
			printf("���յ���");
			for(i=0;i<isRece;i++)
			{
						printf(" %d ",ReceBuf[i]);
			}
			for(i = 0; i < isRece / 2; i++)
			{
				sum += ReceBuf[i] & 0xE0;  // ����
			}
			old = ReceBuf[0];
			ReceBuf[0] = sum;
			while( isRece && !Can_Send_Msg(ReceBuf,8))
			{
				isRece = 0;
				printf("\r\n");
			}
			return 1;
		}
		return 0;
}


short  duty = 563;

u8 servo(u8 *arr){
	
	u8  angle  = 0;
	
	angle = arr[1];  // �õ�����ĽǶ���Ϣ
	
	if(angle > 0 && angle <= 200){
		
		//��
		if(angle == 100){
			
			TIM_SetCompare1(TIM4,ServoMedian);
			printf("duty = %d",ServoMedian);
			
		}
		//��ƫ
		if (angle < 100){ // ��ʱ��������
			
			//duty = (ServoMedian - ServoAngleMIN) / 100.0f * angle + ServoAngleMIN;
			duty = duty - 5;
			TIM_SetCompare1(TIM4,duty);
			printf("duty = %d",duty);
		
		}
		//��ƫ
		if (angle > 100){
			duty = duty + 5;
			//duty = (ServoAngleMAX - ServoMedian) / 100.0f * angle + ServoMedian;
			TIM_SetCompare1(TIM4,duty);
			printf("duty = %d",duty);
			
		}
		
	}
	else{
		printf("angle ��Χ����");
		return 0;
		
	}
		
	return 1;
}



void execute(void){
	
	u8 a = 0;
	//CAN��Ϣ����
	a = receData();
	// ���ִ��
	if (a == 1)
	servo(ReceBuf);
	
}



