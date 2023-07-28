#include "timer.h"
#include "led.h"

TIM_HandleTypeDef TIM3_Handler;      //��ʱ����� 

void TIM3_Init(u16 arr,u16 psc)
{
    TIM3_Handler.Instance=TIM3;                          //ͨ�ö�ʱ��3
    TIM3_Handler.Init.Prescaler=psc;                     //��Ƶϵ��
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM3_Handler.Init.Period=arr;                        //�Զ�װ��ֵ
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
		HAL_TIM_Base_Init(&TIM3_Handler);
}

//��ʱ���ײ�����������ʱ�ӣ������ж����ȼ�
//�˺����ᱻHAL_TIM_Base_Init()��������
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
  if(htim->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();            //ʹ��TIM3ʱ��
		HAL_NVIC_SetPriority(TIM3_IRQn,1,3);    //�����ж����ȼ�����ռ���ȼ�1�������ȼ�3
		HAL_NVIC_EnableIRQ(TIM3_IRQn);          //����ITM3�ж�
	}
}

/***************************************************************
���������� void startTimer(void)
����  �ܡ� ������ʱ������
������ֵ�� ��
������ֵ�� ��
****************************************************************/
void startTimerInterrupt(void)
{

	//TIM_ClearFlag(TIMx,TIM_FLAG_Update); //����жϱ�־λ
	//TIM_Cmd(TIM6,ENABLE); //ʹ�ܼ���
	TIM3->SR = 0;
	HAL_TIM_Base_Start(&TIM3_Handler);  //������ʱ��

}


/***************************************************************
���������� void stopTimerInterrupt(void) �رն�ʱ������
����  �ܡ� �رն�ʱ������ 
������ֵ�� ��
������ֵ�� ��
****************************************************************/
void stopTimerInterrupt(void)
{
	
	//TIM_ClearFlag(TIM6,TIM_FLAG_Update); //����жϱ�־λ
	//TIM_Cmd(TIM6,DISABLE); //ʧ�ܼ���
	TIM3->SR = 0;
	HAL_TIM_Base_Stop(&TIM3_Handler);  //������ʱ��
 
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

//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM3_Handler);
}


