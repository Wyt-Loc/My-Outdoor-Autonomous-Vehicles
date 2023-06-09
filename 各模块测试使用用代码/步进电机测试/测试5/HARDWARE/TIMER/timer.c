#include "timer.h"


u16 oc1crr1=4915;
u16 oc1crr2=3271;
_Bool oc1_sta=0;

u16 oc2crr1=3277;
u16 oc2crr2=819;
_Bool oc2_sta=0;

void time3_initer(u16 arr,u16 psc)
{
	 GPIO_InitTypeDef GPIO_InitTypestu;
	 TIM_TimeBaseInitTypeDef TIM_TimeBaseInitTypestu;
	 NVIC_InitTypeDef NVIC_InitTypeDST;
	 TIM_OCInitTypeDef TIM_OCInitTypeSTU;
	 TIM_OCInitTypeDef TIM_OCInitTypeSSS;
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

	 TIM_Cmd(TIM3,ENABLE);
	 
	 TIM_ITConfig(TIM3,TIM_IT_CC1,ENABLE);
}


void TIM3_IRQHandler(void)
{
	u16 capture=0;
	u16 capture2=0;
	
  if(TIM_GetITStatus(TIM3,TIM_IT_CC1)!=RESET)
  {
    TIM_ClearITPendingBit(TIM3,TIM_IT_CC1);
		capture=TIM_GetCapture1(TIM3);
		if(oc1_sta==0)
		{
      TIM_SetCompare1(TIM3,capture+oc1crr2);
			oc1_sta=1;
    }
		else
		{
       TIM_SetCompare1(TIM3,capture+oc1crr1);
			 oc1_sta=0;
    }
	}
  
}
