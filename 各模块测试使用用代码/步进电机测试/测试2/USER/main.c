#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"


/* 
  �����������Ƶ�ʷ�Χ�� 20-100000Hz   ��PWM�����Ƶ�ʷ�Χ
	
	
	
*/
int main(void)
{
	u8 key = 0;
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(115200);
	KEY_Init();
	
	TIM3_PWM_Init(1999, 71);		 		 //  3200����һȦ ��Ƶ�ʼ��������
	
	TIM_SetCompare2(TIM3, 1000);
	
	while (1)
	{
		
		key = KEY_Scan(0);
		
		if(key == 1){
			
		}
		
		
	}
}
