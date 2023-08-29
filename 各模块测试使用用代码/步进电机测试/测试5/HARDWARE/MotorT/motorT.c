#include "motorT.h"
#include "math.h"
#include "stdio.h"
#include "dma.h"


Motor m;

u16 T;
u8 i = 0;
extern u16 send_buf[1];

int num  = 0;


// �Ƕ�ת������
u16 step_angletwopulse(int angle, float V){
	
	int pluseNum = 0;
	
	m.stepAngle = 1.80f;
	m.subdivision = 16.0f;
	if(angle >= 0) m.dir = 1;
	else m.dir = 0;
	
	m.Speed = V;  
	
	pluseNum = angle  / 0.225f;  
	
	if(pluseNum < 0) pluseNum = -pluseNum;
	
	if(pluseNum == 0){ //���������Ϊ0 ��ر�PWM���
			
		TIM_Cmd(TIM3, DISABLE);  //�ر�TIM3
	
	}
	
	else{
	
		TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3

	}
	printf("pluseNum = %d",pluseNum);
	return pluseNum;
	
}


#define gx   625.0f


//�����ٶ�
float Set_V(Motor m){
	
	float psc = 1;
	psc = gx / m.Speed / 2; // ��λתΪ ת/��
	send_buf[0] = (u16)psc;
	printf("psc = %d",(u16)psc);
	
	DMA_Enable(DMA1_Channel6);  //psc = 6250
	
	return psc;
	
}


//  step_angletwopulse 
// ���ƶ�ʱ��������ٸ�������, �����õ��ٶȽ���
// ִ�н������� 1
u8 step_pulseNum(int pulsenum){

	u8 flag = 0;	//�Ƿ�ִ���� ��־λ

	Set_V(m);

	if(pulsenum == 0)
		TIM_Cmd(TIM3, DISABLE);  //ʹ��TIM3
	else{  //�����������Ϊ0�����������
		m.pulsesNum = 0;
		flag = 0;
		while(flag == 0){
			if(m.pulsesNum == pulsenum){ //����ۼ��������������������� ��ֹͣ���
				TIM_Cmd(TIM3,DISABLE);
				return 1;
			}
		}
	}
	return 0;
}



/*
void TIM3_IRQHandler(void)
{
	    if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
    {
       
    }
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ		
	
}*/















