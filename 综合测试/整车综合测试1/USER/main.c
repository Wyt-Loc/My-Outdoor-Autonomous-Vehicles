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


#define radtor  0.01591549



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
	BASIC_TIM_Config();
	delay_ms(20);
	
	while(1)
	{
		
		Control(usartRecvData());
		
		if(KEY_Scan(1) == 2){
			LED0 = 1;
		}
		if(KEY_Scan(1) == 3){
				LED0 = 0;
		}
		
	}
}

