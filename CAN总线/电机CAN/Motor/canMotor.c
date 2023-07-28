/* 					CANBUS ���Ƶ�� 				*/

#include "canMotor.h"
#include "MotorT.h"
#include "can.h"
#include "stdio.h"


/***************************************************************
���������� u8* receData(void)
����  �ܡ� �õ�CAN��������
������ֵ�� ��
������ֵ�� ����
****************************************************************/
	static u8 ReceBuf[8]; // ���ν��յ�����
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
			for(i = 0; i < isRece / 2; i++){
				sum += ReceBuf[i] & 0xE0;  // ����
			}
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


void test (){
	
	if(receData() == 1){
		printf("shuju = %s \r\n",ReceBuf);
	}
}

/***************************************************************
���������� void motorExecute(u8* arr)
����  �ܡ� 
������ֵ��
������ֵ��
****************************************************************/
//  create_t_ctrl_param(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
void motorExecute(u8* arr){
	
	u8 dir = 0;
	float dis = 0.0f;
	float speed = 0.0f;
	int32_t step = 0;
	
	dis = arr[1]+arr[2]*0.1;
	
	speed = arr[3] + arr[4] * 0.1; // �ٶ� m/s
	
	//ת��Ϊ RPM   �����ֵ��ܳ�Ϊ0.5m
	speed = speed / 0.5f;
	
	// ת��Ϊ ����    1 Rad/sec = 9.55 RPM
	speed = speed / 9.55f;
	
	step = dis / 0.5f;
	
	step = step / 9.55f;
	
	if (dir == 1){
			dis = dis;
	}else if (dir == 0){
		dis = -dis;
	}
	
	//�������ת�� ������ ���ٶȣ����ٶȣ�����ٶ�
	create_t_ctrl_param( step,  30,  30,  speed);
	printf("step = %d, speed = %f\r\n", step, speed);
	
}




