#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "dma.h"
#include "timer.h"
#include "led.h"
#include "motorT.h"

#define size 1
extern u16 DMA1_MEM_LEN;
extern DMA_InitTypeDef DMA_InitStructure;  
u16 send_buf[size];

extern u16 arr1;
extern u16 psc1;

int main(void)
{

	int duty = 20;
	u16 duty1 = 20;
	Motor m1;
	delay_init();
	LED_Init();
	uart_init(115200);
	KEY_Init();
	
	DMA_Config(DMA1_Channel6, (u32)&TIM3->PSC, (u32)send_buf, size);
	
	TIM3_PWM_Init(1999,719);   // 开启时默认 320 赫兹   不使能定时器TIM3  100KHZ
	
	delay_ms(20);
	TIM_Cmd(TIM3, ENABLE);  /* 使能TIM3 */
	TIM_SetCompare1(TIM3,1000);		
	
	//DMA_Enable(DMA1_Channel6);
	
	while(1)
	{
		if(KEY_Scan(0) == 1){
			//myStepMotorT(1600,25,20,1000);
			send_buf[0] = duty;
			DMA_Enable(DMA1_Channel6);
			duty += 100;
			//TIM_SetCompare1(TIM3,duty);		
			//duty = step_angletwopulse(180);
			//duty1 = step_pulseNum(duty);
		 if(duty1 == 1){
			 LED0 = 0;
			 delay_ms(200);
			 LED0 = 1;
		  }
		}
	}
}
