#include "timer.h"
#include "led.h"
#include "usart.h"


#ifdef x


void TIM3_PWM_Init(uint16_t arr,uint16_t psc)
{  
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;                                      //声明NVIC_InitTypeDef 向量结构变量
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	/* 使能定时器3时钟 */
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  /* 使能GPIO外设 */   
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; /* TIM_CH1*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  /* 复用推挽输出 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);/* 初始化GPIO */
	
   /* 初始化TIM3 */
	TIM_TimeBaseStructure.TIM_Period = arr; /* 设置在下一个更新事件装入活动的自动重装载寄存器周期的值 */
	TIM_TimeBaseStructure.TIM_Prescaler =psc; /* 设置用来作为TIMx时钟频率除数的预分频值 */
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//0; /* 设置时钟分割:TDTS = Tck_tim */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  /*TIM向上计数模式 */
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); /* 根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位 */
	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;  //TIM_OCMode_PWM1; /* 选择定时器模式:TIM脉冲宽度调制模式1 */
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; /* 比较输出使能 */
	
	//TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  /* 使能TIM3在CCR1上的预装载寄存器*/
	TIM_OCInitStructure.TIM_Pulse= 0;

	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; /* 输出极性:TIM输出比较极性高 */

	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  /* 根据T指定的参数初始化外设TIM3 OC1 */
	
	//开启中断
	//TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);  //使能定时器的更新中断（更新中断属于中断种类的一种）
	
	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;       	 			//TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;			//主优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; 					//从优先级为3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;     				//使能IRQ通道 				
	NVIC_Init(&NVIC_InitStructure);     			//初始化NVIC寄存器
	
	//TIM_DMACmd(TIM3, TIM_DMA_Update, ENABLE);	/* 如果是要调节占空比就把这行去掉注释，然后注释掉下面那行，再把DMA通道6改为DMA通道3 */
	TIM_DMACmd(TIM3, TIM_DMA_CC1, ENABLE);
	//TIM_Cmd(TIM3, DISABLE);  /* 使能TIM3 */
	
}

//DMA_Config(DMA1_Channel6, (u32)&TIM3->PSC, (u32)send_buf, size);

#endif



u16 oc1crr1=4915;
_Bool oc1_sta=0;


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
	 NVIC_InitTypeDST.NVIC_IRQChannelPreemptionPriority=1;
	 NVIC_InitTypeDST.NVIC_IRQChannelSubPriority=1;
	 NVIC_Init(&NVIC_InitTypeDST);
	 
	 TIM_OCInitTypeSTU.TIM_OCMode=TIM_OCMode_Toggle;
	 TIM_OCInitTypeSTU.TIM_OCPolarity=TIM_OCPolarity_Low;
	 TIM_OCInitTypeSTU.TIM_OutputState=TIM_OutputState_Enable;
	 TIM_OCInitTypeSTU.TIM_Pulse=oc1crr1;
	 TIM_OC1Init(TIM3,&TIM_OCInitTypeSTU);
	 TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Disable);

	 //TIM_Cmd(TIM3,DISABLE);  开始不使能定时器
	 //TIM_ITConfig(TIM3,TIM_IT_CC1,ENABLE);
}







