#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "motor.h"
#include "s_curve.h"

int main(void)
{

	delay_init();								
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	Motor_Init();
	TIM5_Int_Init(499, 119);

	while (1)
	{
		
	}
	
}
