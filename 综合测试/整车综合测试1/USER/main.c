#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "led.h"
#include "motorT.h"
#include "Servos.h"


#define size 1
extern u16 DMA1_MEM_LEN;
extern DMA_InitTypeDef DMA_InitStructure;  
u16 send_buf[size];


int main(void)
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();
	LED_Init();
	uart_init(115200);
	uart2_init(115200);  //串口接收
	KEY_Init();
	time3_initer(0xffff, 35); //步进电机定时器
	Servos_PWM_Init(ARR_5000-1, PSC-1); //舵机定时器初始化
	BaseTimer_Init();
	delay_ms(20);
	TIM_SetCompare1(TIM4,559);
	while(1)
	{
		
		Control(usartRecvData());
		
		if(KEY_Scan(1) == 2){
			//ServosLeftRightClick(1);
			Control(7);
			LED0 = 1;
		}
		
		if(KEY_Scan(1) == 3){
			//ServosLeftRightClick(2);
			Control(7);
			LED0 = 0;
		}
		
	}
}

