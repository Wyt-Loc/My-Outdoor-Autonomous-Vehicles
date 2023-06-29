/* 	CANBUS����������ָ����ⷢ��			*/
 
 #include <math.h>
 #include <stdio.h>
 #include <string.h>
 #include "timer.h"
 #include "canControl.h"
 #include "can.h" 


/***************************************************************
���������� char motorValueVerify(u8 dir, float distance, float speed)
����  �ܡ� У������Ƿ���ȷ  �����Ƿ���ڸ�����������
������ֵ�� �Ƿ���ȷ
������ֵ�� �Է��� 1������ ���� 0
****************************************************************/
char motorValueVerify(u8 dir, float distance, float speed){
	
	u8 flag = 0;
	
    if (dir > 255) {
      printf("Error: Invalid dir parameter! \n");
			flag = 1;
    }
    if (isnan(distance) || !isfinite(distance) || distance < 0 || distance > 255) {
      printf("Error: Invalid distance parameter! \n");
			flag = 1;
    }
    if (isnan(speed) || !isfinite(speed) || speed < 0) {
			printf("Error: Invalid speed parameter! \n");
			flag = 1;  
    }
		if (speed > 10) {
			printf("The speed parameter exceeds the maximum value! \n");
			flag = 1;
		}
		if (distance > 50.0f){
			printf("Error: The distance parameter exceeds the maximum value \n");
			flag = 1;
		}
		if(flag == 0) return 1; else return 0;

}


/***************************************************************
���������� u8 *motorCommandGenerate(u8 dir, float distance, float speed)
����  �ܡ� ���������������
������ֵ�� dir������  distance���˶����룻 speed���ٶȣ�
������ֵ�� ��������, ���ʹ�÷�������bitλ���˷ѡ�
****************************************************************/
u8* motorCommandGenerate(u8 dir, float distance, float speed){

	static u8 buf[8] = {0};
		if(dir == 1)
			buf[0] = 1;
		else
			buf[0] = 0;

		//get disance
		buf[1] = distance; //�õ��������� 		���ֵλ255
		buf[2] = (distance - buf[1]) * 100;  //С�����2λ  ���99

		//get speed
		buf[3] = speed; //����   ���ֵλ255
		buf[4] = (speed - buf[3]) * 100;		//С�����2λ  ���99  ����7λ�����������λ����

		//ʣ��3λ��Ϊ����λ�ݲ�ʹ��

		return buf;
}


// �Ƿ���յ������յ��ˣ����շ�����һ��Ӧ�������0.1����û��Ӧ���źţ���ô�ٴη��ͣ��ۼƴ����ϣ�
// ֱ�ӱ�����ʼ��¼error����log���Ҵ���ʱ����Ϣ��
/*
   500KB ��can ���ÿ��500000bits �� ��ô1��Ҳ���� 3900 ֡�� ����ֱ�Ӳ���һ����ôҲ�н�2000֡��
		1ms �ܹ���3֡�� ��ô���ǲ�ȡ0.01���У��ʱ�䣬Ҳ�ܹ�֤ϵͳ�㹻���ȶ���
*/
/***************************************************************
���������� u8 isReceived(void)
����  �ܡ� �жϹ��ؽڵ��Ƿ���յ�������֡
������ֵ�� ��
������ֵ�� �Ƿ�
****************************************************************/
u8 isReceived(u8* buff){
	
	u8 receBuf[8];
	u8 cnt = 0;
	u8 sum = 0;
	u8 i = 0;
	
	cnt = Can_Receive_Msg(receBuf); // ���������յ��󷵻������ֽڵĸ���λ֮�͡� ���Ҳ����9 * 8 = 72  ��ʱû�뵽�õ�У���㷨
	if(cnt) { //���յ�������
		for(i = 0; i < cnt / 2; i++){
			sum += buff[i] & 0xE0;
		}
		 if( sum == receBuf[0]){
			 	printf("The returned data was successfully received!! \r\n");
			 return 1;  //�ɹ�
		 }
	}

	return 0; //ʧ��

}


/***************************************************************
���������� u8 sendMotorCommand(u8 dir, float distance, float speed)
����  �ܡ� ���Ϳ�����Ϣ�����Ƶ���˶�
������ֵ�� dir������  distance���˶����룻 speed���ٶȣ�
������ֵ�� �Ƿ��ͳɹ�����ʱ����,    �������ʧ�ܣ���һֱ�����ط���ֱ���ɹ�Ϊֹ��
					 ������һ���׶ˣ�����һ���ǳ�����Ҫ�����ݷ���ʧ�ܣ�����ϵͳֱ��̱����
					 ����һֱ���ղ�����Ҳ��������·��ĳ������ˣ����������������ȥ�������ʱ�������ˡ�
****************************************************************/
u8 sendMotorCommand(u8 dir, float distance, float speed){
	u8 i;
	u8 errorFlag = 0;
	u8 errorCnt = 0;
	u8 issucess = 0;
	u8 timecnt = 0;
	u8 *commandBuf = motorCommandGenerate(dir,distance,speed);
	issucess = Can_Send_Msg(commandBuf,8);//����8���ֽ�
	
	_q1:  //goto���������ִ��
	startTimerInterrupt();		//��ʱ����ʼ����
	
	printf("This time the data is��");
	for(i = 0;  i < 8; i++){
		 printf(" %d",commandBuf[i]);
	}
	printf("\r\n");
	
	if (!issucess)
		printf("The control command was sent successfully ! \r\n"); 	//�������е�printf������װλ��̫����Log
	else
		printf("The control command was sent fail ! \r\n"); //�����Ƿ�ɹ�
	
	while(1){
		// �ɹ���
		if( isReceived(commandBuf) && getTimerCountValue(TIM6) < 10)  // ����õ���������ʱ��С��10ms
		{
			stopTimerInterrupt();
			timecnt = getTimerCountValue(TIM6);   // �õ�ʱ���ʱ����������������
			printf("This time, the data is sent \r\n");
			errorFlag = 0; // ���ͳɹ��ˣ���ô�⿪������ơ�
			break;
		}
		// ʧ����
		else
		{
			// ��û������ĵȼ�
				stopTimerInterrupt();
				errorCnt++;
				if(errorCnt < ERRORRANGE){  //�������
					printf("The time is greater than 2ms or the data received is incorrect %dth time\r\n", errorCnt); //һֱ��log֮���������Ǳ�дһ���ű����log��־�Ƿ�������
					issucess = Can_Send_Msg(commandBuf,8);  //���·��ʹ˴ε�����
					goto _q1;   //ʹ��goto�򻯴��룬 ����ʱע����������������½��м����ε��¼���Ҳ���������˿�ʼ��ʱ����һ�д��롣
				}
				else{
					errorFlag = 1;
				}
			// ������10�λ���ʧ���ˣ�
				// ��Ȼ���ϳ������·��ͣ�����logҲ��һֱ��ӡ
				while(errorFlag == 1){
					//����һֱ�ظ�������̣� ����log�ȼ������ ERROR �ȼ�
					printf("ERROR The time is greater than 2ms or the data received is incorrect %dth time\r\n", errorCnt); //һֱ��log֮���������Ǳ�дһ���ű����log��־�Ƿ�������
					issucess = Can_Send_Msg(commandBuf,8);  //���·��ʹ˴ε�����
					goto _q1;
				}
			break;
		}
	}
	return timecnt;
}


/***************************************************************
����������
����  �ܡ�
������ֵ��
������ֵ��
****************************************************************/
void ControlEntryFunction(u8 dir, float distance, float speed)
{
	
	if (motorValueVerify(dir, distance, speed)) { //����ֵУ��

		sendMotorCommand(dir, distance, speed);

	}

}


/***************************************************************
����������void test()
����  �ܡ����ܵ�Ԫ���� ��ͱ�׼Ҫ��
������ֵ����
������ֵ����
****************************************************************/
void test(void){

	u8 dir = 1;
	float dis = 3.125f;
	float speed = 4.043f;

	if (motorValueVerify(dir, dis, speed)) { //����ֵУ��

		sendMotorCommand(dir, dis, speed);

	}
}

