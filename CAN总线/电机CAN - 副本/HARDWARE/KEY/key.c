#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"
					    
//步进电机驱动器
void StepperMotorDriver_IO(void) //IO初始化
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能PORTA,PORTE时钟
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2;				 //PA.1 端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.1

}
