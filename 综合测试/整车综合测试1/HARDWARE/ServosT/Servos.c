#include "Servos.h"


/***************************************************************
���������� void Servos_PWM_Init(u16 arr,u16 psc)
����  �ܡ� ���PWM��ʼ��
������ֵ�� arr  �Զ���װ��ֵ
					 psc  ��Ƶϵ��
������ֵ�� ��
****************************************************************/
void Servos_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��
 
   //���ø�����Ϊ�����������,���TIM4 CH1 CH2��PWM���岨��	GPIOB.6��GPIOB.7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO
 
   //��ʼ��TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//��ʼ��TIM3 Channel2 PWMģʽ
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //�������:TIM����Ƚϼ��Ը� TIM_OCPolarity_Low
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC2
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
	
	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIM3
}


u16 duty = 130;



/***************************************************************
���������� void ServosControl(int angle)
����  �ܡ� �������    ��һ��תһ��
������ֵ�� �Ƕ� Ĭ��Ϊ0   ������ת��  ������ת   ServoMedianLeft�����ֵ
������ֵ�� ��		 
****************************************************************/
void ServosControl(int angle){
	
	u16 Servoserror = ServoAngleMAXLeft - ServoMedianLeft;
	
	duty = ServoMedianLeft + angle * (Servoserror / 150);
	
	if (angle == 0){
		TIM_SetCompare1(TIM4,ServoMedianLeft);
		TIM_SetCompare2(TIM4,ServoMedianLeft);
	}
	
	if(angle > 0 && angle < 150){ //������
		TIM_SetCompare1(TIM4,(u16)duty);	
	}
	
	if(angle < 0 && angle > -150){ //���Ҵ��
		TIM_SetCompare1(TIM4,(u16)duty);	
	}
	
}


/***************************************************************
���������� void ServosLeftRight(u8 leftorright)
����  �ܡ� ���ƶ�����������Ҵ��
������ֵ�� ������  1��   2��   
������ֵ�� ��
****************************************************************/
void ServosLeftRight(u8 leftorright){
	
	if (leftorright == 1 && duty < ServoAngleMAXLeft){		//���ִ��
			duty += 1;
			TIM_SetCompare1(TIM4,duty);
	}
	
	if (leftorright == 2 && duty > ServoAngleMINLeft){    //���ִ��
			duty -= 1;
			TIM_SetCompare1(TIM4,duty);
	}
	
}







