#include "timer.h"


void time3_initer(u16 arr,u16 psc)
{
	 GPIO_InitTypeDef GPIO_InitTypestu;
	 TIM_TimeBaseInitTypeDef TIM_TimeBaseInitTypestu;
	 NVIC_InitTypeDef NVIC_InitTypeDST;
	 TIM_OCInitTypeDef TIM_OCInitTypeSTU;
	
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
   
	 GPIO_InitTypestu.GPIO_Mode=GPIO_Mode_AF_PP;
	 GPIO_InitTypestu.GPIO_Pin=GPIO_Pin_6;
	 GPIO_InitTypestu.GPIO_Speed=GPIO_Speed_10MHz;
	 GPIO_Init(GPIOA,&GPIO_InitTypestu);
	
	 TIM_TimeBaseInitTypestu.TIM_ClockDivision=TIM_CKD_DIV1;
	 TIM_TimeBaseInitTypestu.TIM_CounterMode=TIM_CounterMode_Up;
	 TIM_TimeBaseInitTypestu.TIM_Period=arr;
	 TIM_TimeBaseInitTypestu.TIM_Prescaler=psc;
	 TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitTypestu);
	
	 NVIC_InitTypeDST.NVIC_IRQChannel=TIM3_IRQn;
	 NVIC_InitTypeDST.NVIC_IRQChannelCmd=ENABLE;
	 
	 NVIC_InitTypeDST.NVIC_IRQChannelPreemptionPriority=2;
	 NVIC_InitTypeDST.NVIC_IRQChannelSubPriority=0;
	  
	 NVIC_Init(&NVIC_InitTypeDST);
	 
	 TIM_OCInitTypeSTU.TIM_OCMode=TIM_OCMode_Toggle;
	 TIM_OCInitTypeSTU.TIM_OCPolarity=TIM_OCPolarity_Low;
	 TIM_OCInitTypeSTU.TIM_OutputState=TIM_OutputState_Enable;
	 TIM_OCInitTypeSTU.TIM_Pulse=0;
	 TIM_OC1Init(TIM3,&TIM_OCInitTypeSTU);
	 TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Disable);

	 TIM_Cmd(TIM3,ENABLE);
	 
	 TIM_ITConfig(TIM3,TIM_IT_CC1,ENABLE);
}





void BASIC_TIM_Config(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

// ������ʱ��ʱ��,���ڲ�ʱ�� CK_INT=72M
    BASIC_TIM_APBxClock_FUN(BASIC_TIM_CLK, ENABLE);

// �Զ���װ�ؼĴ����ܵ�ֵ(����ֵ)
    TIM_TimeBaseStructure.TIM_Period=3000;

// ʱ��Ԥ��Ƶ��Ϊ 71����������������ʱ�� CK_CNT = CK_INT / (719+1)= 33�� ÿ��ִ��33��
    TIM_TimeBaseStructure.TIM_Prescaler= 719;

// ��ʼ����ʱ��
    TIM_TimeBaseInit(BASIC_TIM, &TIM_TimeBaseStructure);

// ����������жϱ�־λ
    TIM_ClearFlag(BASIC_TIM, TIM_FLAG_Update);

// �����������ж�
    TIM_ITConfig(BASIC_TIM,TIM_IT_Update,ENABLE);

// ʹ�ܼ�����
    TIM_Cmd(BASIC_TIM, ENABLE);

// ��ʱ�رն�ʱ����ʱ�ӣ��ȴ�ʹ��
    BASIC_TIM_APBxClock_FUN(BASIC_TIM_CLK, DISABLE);
}








