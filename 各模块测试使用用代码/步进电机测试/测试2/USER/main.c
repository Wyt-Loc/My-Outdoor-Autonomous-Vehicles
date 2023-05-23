#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"


/* 
  步进电机驱动频率范围： 20-100000Hz   即PWM输出的频率范围
	
	
	
*/
int main(void)
{
	u8 key = 0;
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(115200);
	KEY_Init();
	
	TIM3_PWM_Init(1999, 71);		 		 //  3200脉冲一圈 即频率即脉冲个数
	
	TIM_SetCompare2(TIM3, 1000);
	
	while (1)
	{
		
		key = KEY_Scan(0);
		
		if(key == 1){
			
		}
		
		
	}
}
