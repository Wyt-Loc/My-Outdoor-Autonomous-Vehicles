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

// 开启定时器时钟,即内部时钟 CK_INT=72M
    BASIC_TIM_APBxClock_FUN(BASIC_TIM_CLK, ENABLE);

// 自动重装载寄存器周的值(计数值)
    TIM_TimeBaseStructure.TIM_Period=3000;

// 时钟预分频数为 71，则驱动计数器的时钟 CK_CNT = CK_INT / (719+1)= 33次 每秒执行33次
    TIM_TimeBaseStructure.TIM_Prescaler= 719;

// 初始化定时器
    TIM_TimeBaseInit(BASIC_TIM, &TIM_TimeBaseStructure);

// 清除计数器中断标志位
    TIM_ClearFlag(BASIC_TIM, TIM_FLAG_Update);

// 开启计数器中断
    TIM_ITConfig(BASIC_TIM,TIM_IT_Update,ENABLE);

// 使能计数器
    TIM_Cmd(BASIC_TIM, ENABLE);

// 暂时关闭定时器的时钟，等待使用
    BASIC_TIM_APBxClock_FUN(BASIC_TIM_CLK, DISABLE);
}








