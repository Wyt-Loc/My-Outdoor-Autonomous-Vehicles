#include "Servos.h"


/***************************************************************
【函数名】 void Servos_PWM_Init(u16 arr,u16 psc)
【功  能】 舵机PWM初始化
【参数值】 arr  自动重装载值
					 psc  分频系数
【返回值】 无
****************************************************************/
void Servos_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//使能定时器3时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
 
   //设置该引脚为复用输出功能,输出TIM4 CH1 CH2的PWM脉冲波形	GPIOB.6，GPIOB.7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
 
   //初始化TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM3 Channel2 PWM模式
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性高 TIM_OCPolarity_Low
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC2
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
	
	TIM_Cmd(TIM4, ENABLE);  //使能TIM3
}


u16 duty = 130;



/***************************************************************
【函数名】 void ServosControl(int angle)
【功  能】 舵机控制    按一次转一次
【参数值】 角度 默认为0   负数左转，  正数右转   ServoMedianLeft舵机中值
【返回值】 无		 
****************************************************************/
void ServosControl(int angle){
	
	u16 Servoserror = ServoAngleMAXLeft - ServoMedianLeft;
	
	duty = ServoMedianLeft + angle * (Servoserror / 150);
	
	if (angle == 0){
		TIM_SetCompare1(TIM4,ServoMedianLeft);
		TIM_SetCompare2(TIM4,ServoMedianLeft);
	}
	
	if(angle > 0 && angle < 150){ //向左打角
		TIM_SetCompare1(TIM4,(u16)duty);	
	}
	
	if(angle < 0 && angle > -150){ //向右打角
		TIM_SetCompare1(TIM4,(u16)duty);	
	}
	
}


/***************************************************************
【函数名】 void ServosLeftRight(u8 leftorright)
【功  能】 控制舵机向左还是向右打角
【参数值】 左还是右  1左   2右   
【返回值】 无
****************************************************************/
void ServosLeftRight(u8 leftorright){
	
	if (leftorright == 1 && duty < ServoAngleMAXLeft){		//舵机执行
			duty += 1;
			TIM_SetCompare1(TIM4,duty);
	}
	
	if (leftorright == 2 && duty > ServoAngleMINLeft){    //舵机执行
			duty -= 1;
			TIM_SetCompare1(TIM4,duty);
	}
	
}







