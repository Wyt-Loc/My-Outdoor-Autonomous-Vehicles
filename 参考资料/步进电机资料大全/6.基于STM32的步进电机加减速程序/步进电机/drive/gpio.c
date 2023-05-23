#include "stm32f0xx.h"
#include "gpio.h"

void IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7 ;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);		//GPIOA7，ENA，使能
	GPIO_SetBits(GPIOA,GPIO_Pin_6);			//GPIOA6，DIR，方向
	// | GPIO_Pin_5
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4; 
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP; // 上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStruct);	

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);	//GPIOB1脉冲输出
  GPIO_InitStruct.GPIO_Pin =GPIO_Pin_1 ;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_ResetBits(GPIOB, GPIO_Pin_1);

  
	NVIC_InitStruct.NVIC_IRQChannel = EXTI4_15_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);	


	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource4);
	EXTI_InitStruct.EXTI_Line = EXTI_Line4;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger =EXTI_Trigger_Falling;//下降沿中断
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct); 
		
//		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource5);
//		EXTI_InitStruct.EXTI_Line = EXTI_Line5;
//		EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
//		EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿中断
//		EXTI_InitStruct.EXTI_LineCmd = ENABLE;
//		EXTI_Init(&EXTI_InitStruct); 

}


