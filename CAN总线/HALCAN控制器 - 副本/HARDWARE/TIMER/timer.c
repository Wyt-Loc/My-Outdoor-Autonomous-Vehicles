#include "timer.h"
#include "led.h"

TIM_HandleTypeDef TIM3_Handler;      //定时器句柄 

void TIM3_Init(u16 arr,u16 psc)
{
    TIM3_Handler.Instance=TIM3;                          //通用定时器3
    TIM3_Handler.Init.Prescaler=psc;                     //分频系数
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //向上计数器
    TIM3_Handler.Init.Period=arr;                        //自动装载值
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频因子
		HAL_TIM_Base_Init(&TIM3_Handler);
}

//定时器底册驱动，开启时钟，设置中断优先级
//此函数会被HAL_TIM_Base_Init()函数调用
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
  if(htim->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();            //使能TIM3时钟
		HAL_NVIC_SetPriority(TIM3_IRQn,1,3);    //设置中断优先级，抢占优先级1，子优先级3
		HAL_NVIC_EnableIRQ(TIM3_IRQn);          //开启ITM3中断
	}
}

/***************************************************************
【函数名】 void startTimer(void)
【功  能】 开启定时器计数
【参数值】 无
【返回值】 无
****************************************************************/
void startTimerInterrupt(void)
{

	//TIM_ClearFlag(TIMx,TIM_FLAG_Update); //清除中断标志位
	//TIM_Cmd(TIM6,ENABLE); //使能计数
	TIM3->SR = 0;
	HAL_TIM_Base_Start(&TIM3_Handler);  //启动定时器

}


/***************************************************************
【函数名】 void stopTimerInterrupt(void) 关闭定时器计数
【功  能】 关闭定时器计数 
【参数值】 无
【返回值】 无
****************************************************************/
void stopTimerInterrupt(void)
{
	
	//TIM_ClearFlag(TIM6,TIM_FLAG_Update); //清除中断标志位
	//TIM_Cmd(TIM6,DISABLE); //失能计数
	TIM3->SR = 0;
	HAL_TIM_Base_Stop(&TIM3_Handler);  //启动定时器
 
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

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM3_Handler);
}


