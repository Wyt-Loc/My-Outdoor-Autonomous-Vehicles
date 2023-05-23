#include "timer.h"
#include "led.h"
#include "usart.h"


//电机1配置
/***定时器1主模式***/
void TIM1_config(u32 Cycle) //周期
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE); 

    TIM_TimeBaseStructure.TIM_Period = Cycle-1; //设置时间                                           
    TIM_TimeBaseStructure.TIM_Prescaler =71;                    //设置用来作为TIMx时钟频率除数的预分频值                                                     
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //设置时钟分割：TDTS= Tck_tim            
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;            //重复计数，一定要=0！！！
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);                                       

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;          //选择定时器模式：TIM脉冲宽度调制模式1       
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_Pulse = Cycle/2-1;                    //设置待装入捕获寄存器的脉冲值                                   
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      //输出极性       
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);                                                         

	
    TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable);
    TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);

    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);                              
    TIM_ARRPreloadConfig(TIM1, ENABLE);                                                          
}


/***定时器2从模式***/
void TIM2_config(u32 PulseNum)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure; 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = PulseNum-1;   
    TIM_TimeBaseStructure.TIM_Prescaler =0;    
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);  

    TIM_SelectInputTrigger(TIM2, TIM_TS_ITR0);
    TIM2->SMCR|=0x07;                                  //设置从模式寄存器   
    TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;        
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;     
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure);
}


void Pulse_output_1(u32 Cycle,u32 PulseNum)
{
    TIM2_config(PulseNum); 
    TIM_Cmd(TIM2, ENABLE);
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM1_config(Cycle); //周期设置
    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);   //高级定时器一定要加上，主输出使能
}


void TIM2_IRQHandler(void) 
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)     // TIM_IT_CC1
    { 
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // 清除中断标志位 
        TIM_CtrlPWMOutputs(TIM1, DISABLE);  //主输出使能
        TIM_Cmd(TIM1, DISABLE); // 关闭定时器
        TIM_Cmd(TIM2, DISABLE); // 关闭定时器 
        TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE); 
    }
}


//电机2配置
//定时器4主模式,定时器3从模式
//PWM输出
void TIM4_config(u32 Cycle)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//定时器设置结构体
    TIM_OCInitTypeDef  TIM_OCInitStructure; //pwm波对应设置结构体
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE); //开启时钟
 
    TIM_TimeBaseStructure.TIM_Period = Cycle-1;                                                   
    TIM_TimeBaseStructure.TIM_Prescaler =71;                    //设置用来作为TIMx时钟频率除数的预分频值                                                     
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //设置时钟分割：TDTS= Tck_tim            
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);                                       
 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;          //选择定时器模式：TIM脉冲宽度调制模式1       
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_Pulse = Cycle/2-1;                    //设置待装入捕获寄存器的脉冲值                                   
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      //输出极性       
    TIM_OC2Init(TIM4, &TIM_OCInitStructure);                                                         
 
    TIM_SelectMasterSlaveMode(TIM4, TIM_MasterSlaveMode_Enable);
    TIM_SelectOutputTrigger(TIM4, TIM_TRGOSource_Update);
 
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);                              
    TIM_ARRPreloadConfig(TIM4, ENABLE);                                                          
}


/***定时器3从模式***/
void TIM3_config(u32 PulseNum)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure; 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
 
    TIM_TimeBaseStructure.TIM_Period = PulseNum-1; //设置自动重装载周期值
    TIM_TimeBaseStructure.TIM_Prescaler =0;    
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;     
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);  
 
    TIM_SelectInputTrigger(TIM3, TIM_TS_ITR3);
    TIM_SelectSlaveMode(TIM3,TIM_SlaveMode_External1 );// 等同 TIM2->SMCR|=0x07 //设置从模式寄存器 
    //   TIM2->SMCR|=0x07;            
    TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE);
 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void Pulse_output_2(u32 Cycle,u32 PulseNum)
{
    TIM3_config(PulseNum);
    TIM_Cmd(TIM3, ENABLE);
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
    TIM4_config(Cycle);
    TIM_Cmd(TIM4, ENABLE);
}

void TIM3_IRQHandler(void) 
{ 
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)     // TIM_IT_CC1
    { 
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // 清除中断标志位 
        TIM_CtrlPWMOutputs(TIM3, DISABLE);  //主输出使能
        TIM_Cmd(TIM4, DISABLE); // 关闭定时器 
        TIM_Cmd(TIM3, DISABLE); // 关闭定时器 
        TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);         
    }
}


/***************************************************************
【函数名】 u8 Speed_And_Angle_Motor1(int v, int dis)
【功  能】 电机1 -----  控制步进电机转动的速度和总共要转动的角度
【参数值】 v  速度      dis 角度    参考 Pulse_output_1(1000,8000);   1000hz   8000个脉冲
【返回值】 1 转完了,  0 没转完呢.
【计算方法及说明】 我们假设将电机设置电机细分为 x，  
****************************************************************/
u8 Speed_And_Angle_Motor1(int v, int dis){
	
	
	
	Pulse_output_1(1000,8000);
	
	
	return 1;
}
























