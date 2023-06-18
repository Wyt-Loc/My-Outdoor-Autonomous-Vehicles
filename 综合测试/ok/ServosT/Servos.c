#include "Servos.h"
#include "timer.h"
#include "stdio.h"

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




__IO u16 duty = 539;
//中值 489
//右最小 440
//左最大 625




/***************************************************************
【函数名】 void ServosLeftRight(u8 leftorright)
【功  能】 控制舵机向左还是向右打角 点按的函数
【参数值】 左还是右  1左   2右   
【返回值】 无
****************************************************************/
void ServosLeftRightClick(u8 leftorright){
	
	if (leftorright == 1 && duty < ServoAngleMAX){		//舵机执行
			duty += 1;
			TIM_SetCompare1(TIM4,duty);
			printf("%d",duty);
	}
	
	if (leftorright == 2 && duty > ServoAngleMIN){    //舵机执行
			duty -= 1;
			TIM_SetCompare1(TIM4,duty);
			printf("%d",duty);
	}
	
	if (leftorright == 3 && duty > ServoAngleMIN && duty < ServoAngleMAX){    //舵机执行
			duty = 563;
			TIM_SetCompare1(TIM4,duty);
			printf("%d",duty);
	}
	
}



//这个东西放定时器执行 防止卡死主程序
//电机长短按控制
__IO u8 clickorlongservos = 0;

short t = 0;

//改为一次函数，进行一个转向加速
// duty = a x + b
void ServosLeftRightLong(void){  //每秒执行33次
	
	if(clickorlongservos != 0){
	
		if (clickorlongservos == 1 && duty < ServoAngleMAX){		//舵机执行
				t++;
				duty += 1;
				TIM_SetCompare1(TIM4,duty);
		}
	
		if (clickorlongservos == 2 && duty > ServoAngleMIN){    //舵机执行
				t--;
				duty -= 1;
				TIM_SetCompare1(TIM4,duty);
		}
	
	}
}


//控制舵机停止长按动作
void servosStop(u8 *clickorlongservos){
		
		*clickorlongservos = 0;
	
}


extern u8 clickorlongmotor;

//反复执行舵机变向函数 达到长按的效果
//之后引入一个
void TIM6_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM6,TIM_IT_Update))
	{
			 ServosLeftRightLong();
			 //printf("%d\r\n",clickorlongmotor);
	}
	TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
}


