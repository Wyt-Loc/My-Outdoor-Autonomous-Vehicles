/*   	��ʱ����ʱ			*/
#include "timer.h"


/***************************************************************
���������� void BASETimer_NVIC_Config(void)
����  �ܡ� ��ʱ���жϳ�ʼ��
������ֵ�� ��
������ֵ�� ��
****************************************************************/
void BASETimer_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	NVIC_InitStruct.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1; //ֻ��һ���жϣ�������
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_Init(&NVIC_InitStruct);
}


/***************************************************************
���������� void BaseTimer_Config(void)
����  �ܡ� ������ʱ����ʼ��
������ֵ�� ��
������ֵ�� ��
****************************************************************/
void BaseTimer_Config(uint16_t arr, uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE );
	
	TIM_TimeBaseInitStruct.TIM_Period = (arr-1);
	TIM_TimeBaseInitStruct.TIM_Prescaler = (psc-1);  //1���Ӽ���1000�� 20ms  Ҳ���Ǽ���20��

	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStruct);
	
	//�����ж�
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
	
}


/***************************************************************
���������� void BaseTimer_Init(void)
����  �ܡ� ��ʼ��������ʱ��
������ֵ�� ��
������ֵ�� ��
****************************************************************/
void BaseTimer_Init(void)
{
	BASETimer_NVIC_Config();
	BaseTimer_Config(100, 720);
}


/***************************************************************
���������� void startTimer(void)
����  �ܡ� ������ʱ������
������ֵ�� ��
������ֵ�� ��
****************************************************************/
void startTimerInterrupt(void)
{

	TIM_ClearFlag(TIM6,TIM_FLAG_Update); //����жϱ�־λ

	TIM_Cmd(TIM6,ENABLE); //ʹ�ܼ���

}


/***************************************************************
���������� void stopTimerInterrupt(void) �رն�ʱ������
����  �ܡ� �رն�ʱ������ 
������ֵ�� ��
������ֵ�� ��
****************************************************************/
void stopTimerInterrupt(void)
{

	TIM_ClearFlag(TIM6,TIM_FLAG_Update); //����жϱ�־λ

	TIM_Cmd(TIM6,DISABLE); //ʹ�ܼ���

}


/***************************************************************
���������� u8 getTimerCountValue(TIM_TypeDef* TIMx)
����  �ܡ� �õ���ʱ���жϼ���ֵ
������ֵ�� TIMx ��ʱ��
������ֵ�� ��ʱ������ֵ
****************************************************************/
u8 getTimerCountValue(TIM_TypeDef* TIMx)
{

	return TIMx->CNT; //�õ���Ӧ�Ĵ����ļ���ֵ

}

