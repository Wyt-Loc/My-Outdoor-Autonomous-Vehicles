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

int main(void)
{
	u16 pluse;
	Motor m1;
	delay_init();
	LED_Init();
	uart_init(115200);
	KEY_Init();
	
	//DMA_Config(DMA1_Channel6, (u32)&TIM3->PSC, (u32)send_buf, size);
	
	//TIM3_PWM_Init(1999, 72-1);   // 开启时默认 320 赫兹   不使能定时器TIM3  10KHZ
	time3_initer(0xffff,71);
	delay_ms(20);
	
	//TIM_SetCompare1(TIM3,1000);		
	
	while(1)
	{
		
		if(KEY_Scan(0) == 3){
	
			//pluse = step_angletwopulse(360,0.1); // 角度转为 脉冲数, 速度 单位转每秒
			//step_pulseNum(pluse);		//执行
			//send_buf[0] = pluse;
			//DMA_Enable(DMA1_Channel6);
			//duty1 = step_pulseNum(duty);
		}
		
		if(KEY_Scan(0) == 2){
			//pluse = step_angletwopulse(360,1); // 角度转为 脉冲数, 速度 单位转每秒
			//step_pulseNum(pluse);		//执行
		}
		
	}
}
