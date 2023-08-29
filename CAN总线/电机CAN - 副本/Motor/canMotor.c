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
static volatile u8 old = 0;
u8 receData(void){

	u8 i = 0;
	u8 sum = 0;
	static u8 isRece = 0;

		isRece = Can_Receive_Msg(ReceBuf); //�������ڵ�����
		if(isRece) //���յ���CAN��Ϣ �� ��ǰִ��״̬Ϊ����
		{
			printf("���յ���");
			for(i=0;i<isRece;i++)
			{
				printf(" Recedata=%d ",ReceBuf[i]);
			}
			for(i = 0; i < isRece / 2; i++)
			{
				sum += ReceBuf[i] & 0xE0;  // ����
			}
			old = ReceBuf[0]; // �����ݱ���
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

void test(){

	if(receData() == 1)
	{
		printf("shuju = %s \r\n",ReceBuf);
	}

}

/***************************************************************
���������� void motorExecute(u8* arr)
����  �ܡ� ���ִ��
������ֵ�� �������
������ֵ�� ��
****************************************************************/
//  create_t_ctrl_param(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)

float R = 0.25; //ֱ��
float cir = 1.57075f; //�ܳ�

extern speedRampData g_srd;
void motorExecute(u8* arr){
	
	short dir = 0;
	float dis = 0.0f;
	float speed = 0.0f;
	float num = 0.0f;
	int step = 0;
	
	dir = old;
	
	if(dir == 115 && arr[1] == 116){
		 printf("��� stop\r\n");
		 g_srd.run_state = STOP;
	 }
	 
	else{
		for (dir =0; dir <8;dir++)
		printf("arr = %d \r\n", arr[dir]);
	
		dis = arr[1] + arr[2]*0.1 + arr[3]*0.01; // m
		printf("dis = %f", dis);
	
		speed = arr[4] + arr[5] * 0.1 + arr[6] *0.01; // �ٶ� m/s
		printf("speed = %f", speed); // ����ٶ�
	
		num = dis / cir; //Ҫ�ߵľ�������Ӧ��Ȧ��
	
		step = 1600 * num; // ���� = 
	
		//ת��Ϊ RPM   �����ֵ��ܳ�Ϊ0.5m
		//speed = speed / 0.5f;
	
		// ת��Ϊ ����    1 Rad/sec = 9.55 RPM
		//speed = speed / 9.55f; // Ȧ/s
	
		//step = dis / 0.5f;
		
		//step = step / 9.55f;
	
		if (old == 1){
			printf("ǰ��");
				step = -step;
		}else if (old == 0){
			printf("����");
			step = step;
		}
	
		//�������ת�� ������ ���ٶȣ����ٶȣ�����ٶ�
		create_t_ctrl_param( step*3,  10,  10,  50);
		printf("step = %d, speed = %f\r\n", step, speed );
		
	}
	
}


void motor(void)
{
	u8 a = 0;
	//��������
	a = receData();
	if (a == 1){
	//���ִ��
		motorExecute(ReceBuf);
	}
}
