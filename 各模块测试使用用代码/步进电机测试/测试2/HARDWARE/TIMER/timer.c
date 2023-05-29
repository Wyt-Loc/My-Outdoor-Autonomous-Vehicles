#include "timer.h"
#include "led.h"
#include "usart.h"



void TIM3_PWM_Init(u16 arr,u16 psc)
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
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; /* 设置时钟分割:TDTS = Tck_tim */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  /*TIM向上计数模式 */
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); /* 根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位 */
	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;//TIM_OCMode_PWM1; /* 选择定时器模式:TIM脉冲宽度调制模式1 */
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; /* 比较输出使能 */
	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  /* 使能TIM3在CCR1上的预装载寄存器*/
	TIM_OCInitStructure.TIM_Pulse= 100;

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
	TIM_Cmd(TIM3, DISABLE);  /* 使能TIM3 */
	
}

Motor m;

extern u16 arr1; 
extern u16 psc1;

u16 T;
u8 i = 0;









int num  = 0;


// 角度转脉冲数
u16 step_angletwopulse(int angle){	
	
	int pluseNum = 0;
	m.stepAngle = 1.80f;
	m.subdivision = 16.0f;
	if(angle >= 0) m.dir = 1;
	else m.dir = 0;
	
	pluseNum = angle  / 0.225f;  /// 3200.0f + (angle % 3200); // m.stepAngle / m.subdivision; 
	
	if(pluseNum < 0) pluseNum = -pluseNum;
	
	if(pluseNum == 0){ //如果脉冲数为0 则关闭PWM输出
			
		TIM_Cmd(TIM3, DISABLE);  //关闭TIM3
	
	}
	
	else{
	
		TIM_Cmd(TIM3, ENABLE);  //使能TIM3

	}
	
	return pluseNum;
	
}


//  step_angletwopulse 
// 控制定时器输出多少个脉冲数
// 执行结束返回 1
u8 step_pulseNum(int pulsenum){
	
	u8 flag = 0;	//是否执行完 标志位
	
	if(pulsenum != 0){  //如果脉冲数不为0，则输出脉冲
		m.pulsesNum = 0;
		flag = 0;
		while(flag == 0){
			if(m.pulsesNum == pulsenum){ //如果累计脉冲数等于所需脉冲数 则停止输出
				TIM_Cmd(TIM3,DISABLE);
				flag = 1;
				return 1;
			}
		}
	}
	return 0;
}





