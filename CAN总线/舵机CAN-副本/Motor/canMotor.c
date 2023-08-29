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
static u8 old = 0;
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
u8 js1 = 100;
u8 js2 = 100;

u8 servo(u8 *arr){

	u8  angle  = 0;
	angle = arr[1];  // �õ�����ĽǶ���Ϣ
	printf("angle = %d\r\n", angle);
	
	// ��� stop
	if (old == 115 && arr[1] == 116)
	{
		printf("���stop");
		TIM_SetCompare2(TIM3,duty); // �����stop ���� �ö��ֱ��ִ�� ����ǰ��λ�ò����ͺ��ˡ�
	}
	// ���ִ��
	else
	{
		if(angle > 0 && angle <= 20)
		{
			//��
			if(angle == 100)
			{
				TIM_SetCompare2(TIM3,ServoMedian); //ռ�ձ�=i/(199+1)  563
				printf("duty = %d",ServoMedian);
			}
			//��ƫ
			if (angle < 10)
			{
				// ��ʱ��������
				duty = (ServoMedian - ServoAngleMIN) / 10.0f * angle + ServoAngleMIN;
				if(duty < 468) duty = 468;
				if(duty > 647) duty = 647;
				TIM_SetCompare2(TIM3,duty);//ռ�ձ�=i/(199+1)
				printf("duty = %d",duty);
			}
			//��ƫ
			if (angle > 10)
			{
				duty = (ServoAngleMAX - ServoMedian) / 10.0f * angle + ServoAngleMIN;
				if(duty < 468) duty = 468;
				if(duty > 647) duty = 647;
				TIM_SetCompare2(TIM3,duty);//ռ�ձ�=i/(199+1)
				printf("duty = %d",duty);
			}
		}
		else
		{
			printf("angle ��Χ����");
			return 0;
		}
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



