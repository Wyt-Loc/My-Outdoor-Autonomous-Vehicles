#include "stm32f0xx.h"
#include "time.h"
#include "GPIO.h"
#include "systick.h"


void Go_Zero()	//归零，正转到霍尔中断后反转到霍尔中断
{
	uint8_t fre=2;
	Speed=3500;		//脉冲速度，输出频率
	Count=10000;		//脉冲数，总步数=Count+r2
	GPIO_SetBits(GPIOA,GPIO_Pin_6);
	while(fre)
	{
		r1=0;
		r2=100;
		StepCount=0;
		GPIO_WriteBit(GPIOA,GPIO_Pin_6,(BitAction)(1-GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_6)));
		TIM_Config();
  do
  {
		
  }while(r2); 
		fre--;
		TIM_Cmd(TIM3, DISABLE);
		Delay(500);
  }
}



int main(void)
{
	SystemInit();
	IO_Init();
	Systick_Init();
	GPIO_SetBits(GPIOA,GPIO_Pin_7);
	Delay(100);
	
	Go_Zero();

	while(1);
}
