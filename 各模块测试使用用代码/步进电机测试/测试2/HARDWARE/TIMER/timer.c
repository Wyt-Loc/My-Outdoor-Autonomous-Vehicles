#include "timer.h"
#include "led.h"
#include "usart.h"



void TIM3_PWM_Init(u16 arr,u16 psc)
{  
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;                                      //����NVIC_InitTypeDef �����ṹ����
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	/* ʹ�ܶ�ʱ��3ʱ�� */
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  /* ʹ��GPIO���� */   
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; /* TIM_CH1*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  /* ����������� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);/* ��ʼ��GPIO */
	
   /* ��ʼ��TIM3 */
	TIM_TimeBaseStructure.TIM_Period = arr; /* ��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ */
	TIM_TimeBaseStructure.TIM_Prescaler =psc; /* ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ */
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; /* ����ʱ�ӷָ�:TDTS = Tck_tim */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  /*TIM���ϼ���ģʽ */
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); /* ����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ */
	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;//TIM_OCMode_PWM1; /* ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ1 */
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; /* �Ƚ����ʹ�� */
	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  /* ʹ��TIM3��CCR1�ϵ�Ԥװ�ؼĴ���*/
	TIM_OCInitStructure.TIM_Pulse= 100;

	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; /* �������:TIM����Ƚϼ��Ը� */

	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  /* ����Tָ���Ĳ�����ʼ������TIM3 OC1 */
	
	//�����ж�
	//TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);  //ʹ�ܶ�ʱ���ĸ����жϣ������ж������ж������һ�֣�
	
	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;       	 			//TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;			//�����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; 					//�����ȼ�Ϊ3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;     				//ʹ��IRQͨ�� 				
	NVIC_Init(&NVIC_InitStructure);     			//��ʼ��NVIC�Ĵ���
	
	//TIM_DMACmd(TIM3, TIM_DMA_Update, ENABLE);	/* �����Ҫ����ռ�ձȾͰ�����ȥ��ע�ͣ�Ȼ��ע�͵��������У��ٰ�DMAͨ��6��ΪDMAͨ��3 */
	TIM_DMACmd(TIM3, TIM_DMA_CC1, ENABLE);
	TIM_Cmd(TIM3, DISABLE);  /* ʹ��TIM3 */
	
}

Motor m;

extern u16 arr1; 
extern u16 psc1;

u16 T;
u8 i = 0;









int num  = 0;


// �Ƕ�ת������
u16 step_angletwopulse(int angle){	
	
	int pluseNum = 0;
	m.stepAngle = 1.80f;
	m.subdivision = 16.0f;
	if(angle >= 0) m.dir = 1;
	else m.dir = 0;
	
	pluseNum = angle  / 0.225f;  /// 3200.0f + (angle % 3200); // m.stepAngle / m.subdivision; 
	
	if(pluseNum < 0) pluseNum = -pluseNum;
	
	if(pluseNum == 0){ //���������Ϊ0 ��ر�PWM���
			
		TIM_Cmd(TIM3, DISABLE);  //�ر�TIM3
	
	}
	
	else{
	
		TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3

	}
	
	return pluseNum;
	
}


//  step_angletwopulse 
// ���ƶ�ʱ��������ٸ�������
// ִ�н������� 1
u8 step_pulseNum(int pulsenum){
	
	u8 flag = 0;	//�Ƿ�ִ���� ��־λ
	
	if(pulsenum != 0){  //�����������Ϊ0�����������
		m.pulsesNum = 0;
		flag = 0;
		while(flag == 0){
			if(m.pulsesNum == pulsenum){ //����ۼ��������������������� ��ֹͣ���
				TIM_Cmd(TIM3,DISABLE);
				flag = 1;
				return 1;
			}
		}
	}
	return 0;
}





