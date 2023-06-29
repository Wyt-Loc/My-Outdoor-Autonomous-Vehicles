/*   	定时器计时			*/
#include "timer.h"


/***************************************************************
【函数名】 void BASETimer_NVIC_Config(void)
【功  能】 定时器中断初始化
【参数值】 无
【返回值】 无
****************************************************************/
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


/***************************************************************
【函数名】 void BaseTimer_Config(void)
【功  能】 基本定时器初始化
【参数值】 无
【返回值】 无
****************************************************************/
void BaseTimer_Config(uint16_t arr, uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE );
	
	TIM_TimeBaseInitStruct.TIM_Period = (arr-1);
	TIM_TimeBaseInitStruct.TIM_Prescaler = (psc-1);  //1秒钟计数1000次 20ms  也就是计数20次

	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStruct);
	
	//配置中断
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
	
}


/***************************************************************
【函数名】 void BaseTimer_Init(void)
【功  能】 初始化开启定时器
【参数值】 无
【返回值】 无
****************************************************************/
void BaseTimer_Init(void)
{
	BASETimer_NVIC_Config();
	BaseTimer_Config(100, 720);
}


/***************************************************************
【函数名】 void startTimer(void)
【功  能】 开启定时器计数
【参数值】 无
【返回值】 无
****************************************************************/
void startTimerInterrupt(void)
{

	TIM_ClearFlag(TIM6,TIM_FLAG_Update); //清除中断标志位

	TIM_Cmd(TIM6,ENABLE); //使能计数

}


/***************************************************************
【函数名】 void stopTimerInterrupt(void) 关闭定时器计数
【功  能】 关闭定时器计数 
【参数值】 无
【返回值】 无
****************************************************************/
void stopTimerInterrupt(void)
{

	TIM_ClearFlag(TIM6,TIM_FLAG_Update); //清除中断标志位

	TIM_Cmd(TIM6,DISABLE); //使能计数

}


/***************************************************************
【函数名】 u8 getTimerCountValue(TIM_TypeDef* TIMx)
【功  能】 得到定时器中断计数值
【参数值】 TIMx 定时器
【返回值】 定时器计数值
****************************************************************/
u8 getTimerCountValue(TIM_TypeDef* TIMx)
{

	return TIMx->CNT; //得到对应寄存器的计数值

}

