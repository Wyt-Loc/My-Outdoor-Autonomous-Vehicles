#include "time.h"


#define TimerPeriod  ((SystemCoreClock / 48 / 1000  ) - 1)//频率=SystemCoreClock/(分频+1)-1
#define StartSpeed	500		//从250hz开始加速
	int StepCount;
	int r1;
	int r2;
	int Speed=500;					//默认速度500hz
	int Count;
	
void TIM_Config(void)
{


	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	
  /*  TIM3 中断嵌套设计*/
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);	
	
	
	/* TIM3 的配置 ---------------------------------------------------
   TIM3 输入时钟(TIM3CLK) 设置为 APB1 时钟 (PCLK1)    
    => TIM3CLK = PCLK1 = SystemCoreClock
   TIM3CLK = SystemCoreClock, Prescaler = 0, TIM3 counter clock = SystemCoreClock
   SystemCoreClock 为48 MHz */

	
  /* TIM3 时钟使能 */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
  
  /* Time 定时基础设置*/
  TIM_TimeBaseStructure.TIM_Prescaler = 47;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  /* Time 定时设置为上升沿计算模式*/
  TIM_TimeBaseStructure.TIM_Period =TimerPeriod;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* TIM3 计算器使能*/
  TIM_Cmd(TIM3, ENABLE);

  /* TIM 中断使能 */
  TIM_ITConfig(TIM3,TIM_IT_Update, ENABLE);

	}


void f(int Vt,int a,int d,int S)		//	总脉冲数=S+r2
{ 
	int pulse;
	int t1;
	int t2=100;
  t1=(Vt*10-500)/a;								//加速要走的步数
 if(StepCount<t1)                 //加速阶段，分t1级加速
  {
  
  pulse=1000000/(500+r1);
  TIM_SetAutoreload(TIM3,pulse);
	r1+=a;
  }                        
 else if(StepCount<=2*S)          //匀速阶段运行要求的步数或者距离
 {
  pulse=100000/Vt;
  TIM_SetAutoreload(TIM3,pulse);
 }
 else if(StepCount>2*S)                            //减速阶段，走完S步后开始减速，分t2-1级减速
  {
    r2--;
   
  if(t2>=1)
  {
  pulse=(100000*t2)/(r2*Vt);
  TIM_SetAutoreload(TIM3,pulse);
  }
 
  }
}


void TIM3_IRQHandler(void)			//TIM3中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		TIM3->SR=0;
		GPIO_WriteBit(GPIOB,GPIO_Pin_1,(BitAction)(1-GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_1)));//翻转
    StepCount=StepCount+1;
    f((Speed/5),10,10,Count);
	}
	
}





