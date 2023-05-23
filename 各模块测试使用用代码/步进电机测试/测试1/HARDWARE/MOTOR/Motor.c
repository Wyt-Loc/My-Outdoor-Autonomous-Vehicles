#include "Motor.h"
#include "timer.h"


/***************************************************************
【函数名】 void CLK_Init(void)
【功  能】 初始化步进电机脉冲输出引脚
【参数值】 无
【返回值】 无
****************************************************************/
void CLK_Init(void){
	
		GPIO_InitTypeDef  GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	  
		//TIM1_4
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;                   //TIM1_CH4 PA11
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;             //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		//TIM4_2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 									//TIM4_CH2和T
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;             //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}

/***************************************************************
【函数名】 void EN_Init(void)
【功  能】 初始化步进电机使能引脚
【参数值】 无
【返回值】 无
****************************************************************/
void EN_Init(void){
	
		GPIO_InitTypeDef  GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB,PE端口时钟
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_0;  //
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
		GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
		GPIO_SetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1);
}

/***************************************************************
【函数名】 void DIR_Init(void)
【功  能】 初始化控制步进电机方向引脚
【参数值】 无
【返回值】 无
****************************************************************/
void DIR_Init(void){
	
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;				 //LED0-->PB.5 端口配置
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
		GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
	
}

/***************************************************************
【函数名】 void main_Init(void)
【功  能】 初始化步进电机初始化参数
【参数值】 无
【返回值】 无
****************************************************************/
void main_Init(void){
	
	CLK_Init();//脉冲输出使能
	
	EN_Init();//使能引脚初始化
	
	DIR_Init();//方向初始化
	
}







