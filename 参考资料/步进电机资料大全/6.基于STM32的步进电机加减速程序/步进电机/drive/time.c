#include "time.h"


#define TimerPeriod  ((SystemCoreClock / 48 / 1000  ) - 1)//Ƶ��=SystemCoreClock/(��Ƶ+1)-1
#define StartSpeed	500		//��250hz��ʼ����
	int StepCount;
	int r1;
	int r2;
	int Speed=500;					//Ĭ���ٶ�500hz
	int Count;
	
void TIM_Config(void)
{


	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	
  /*  TIM3 �ж�Ƕ�����*/
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);	
	
	
	/* TIM3 ������ ---------------------------------------------------
   TIM3 ����ʱ��(TIM3CLK) ����Ϊ APB1 ʱ�� (PCLK1)    
    => TIM3CLK = PCLK1 = SystemCoreClock
   TIM3CLK = SystemCoreClock, Prescaler = 0, TIM3 counter clock = SystemCoreClock
   SystemCoreClock Ϊ48 MHz */

	
  /* TIM3 ʱ��ʹ�� */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
  
  /* Time ��ʱ��������*/
  TIM_TimeBaseStructure.TIM_Prescaler = 47;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  /* Time ��ʱ����Ϊ�����ؼ���ģʽ*/
  TIM_TimeBaseStructure.TIM_Period =TimerPeriod;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* TIM3 ������ʹ��*/
  TIM_Cmd(TIM3, ENABLE);

  /* TIM �ж�ʹ�� */
  TIM_ITConfig(TIM3,TIM_IT_Update, ENABLE);

	}


void f(int Vt,int a,int d,int S)		//	��������=S+r2
{ 
	int pulse;
	int t1;
	int t2=100;
  t1=(Vt*10-500)/a;								//����Ҫ�ߵĲ���
 if(StepCount<t1)                 //���ٽ׶Σ���t1������
  {
  
  pulse=1000000/(500+r1);
  TIM_SetAutoreload(TIM3,pulse);
	r1+=a;
  }                        
 else if(StepCount<=2*S)          //���ٽ׶�����Ҫ��Ĳ������߾���
 {
  pulse=100000/Vt;
  TIM_SetAutoreload(TIM3,pulse);
 }
 else if(StepCount>2*S)                            //���ٽ׶Σ�����S����ʼ���٣���t2-1������
  {
    r2--;
   
  if(t2>=1)
  {
  pulse=(100000*t2)/(r2*Vt);
  TIM_SetAutoreload(TIM3,pulse);
  }
 
  }
}


void TIM3_IRQHandler(void)			//TIM3�ж�
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		TIM3->SR=0;
		GPIO_WriteBit(GPIOB,GPIO_Pin_1,(BitAction)(1-GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_1)));//��ת
    StepCount=StepCount+1;
    f((Speed/5),10,10,Count);
	}
	
}





