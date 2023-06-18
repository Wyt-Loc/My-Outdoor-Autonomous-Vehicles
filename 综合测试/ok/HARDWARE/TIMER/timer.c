#include "timer.h"
#include "led.h"
#include "lvgl.h"
#include "timer.h"




void TIM5_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

	TIM_Cmd(TIM5, ENABLE);  //使能TIMx					 
}
//定时器3中断服务程序
void TIM5_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
		{
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update  );  //清除TIMx更新中断标志 
			lv_tick_inc(1);
		}
}



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
	 
	 NVIC_InitTypeDST.NVIC_IRQChannelPreemptionPriority=3;
	 NVIC_InitTypeDST.NVIC_IRQChannelSubPriority=1;
	  
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











