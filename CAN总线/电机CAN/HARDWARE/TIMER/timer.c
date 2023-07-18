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


void BASETimer_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	
	NVIC_InitStruct.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1; //只有一个中断，随意配
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_Init(&NVIC_InitStruct);
}


void BaseTimer_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE );
	
	TIM_TimeBaseInitStruct.TIM_Period = (3000-1);
	TIM_TimeBaseInitStruct.TIM_Prescaler = (720-1);  //中断一次 1ms
//	不配置也可
//	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
//	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;  //默认向上
//	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0x01;
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStruct);
	
	//配置中断
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
	TIM_ClearFlag(TIM6,TIM_FLAG_Update);
	//使能计数
	TIM_Cmd(TIM6,ENABLE);
}
void BaseTimer_Init(void)
{
	
	BASETimer_NVIC_Config();
	BaseTimer_Config();
}








