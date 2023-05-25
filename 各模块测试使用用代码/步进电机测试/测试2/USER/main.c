#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "dma.h"
#include "timer.h"
#define size 1

extern u16 DMA1_MEM_LEN;
extern DMA_InitTypeDef DMA_InitStructure;  
u16 send_buf[size];
 
int main(void)
{	
	int i;
	int duty;
	int feedback;
	delay_init();	
	uart_init(115200);
	KEY_Init();
	DMA_Config(DMA1_Channel6, (u32)&TIM3->PSC, (u32)send_buf, size);
	TIM3_PWM_Init(1999,719);

	DMA_Enable(DMA1_Channel6);
	
	while(1)
	{
		
			feedback = DMA_send_feedback(DMA1_Channel6);
			if(feedback != 0)
			{
				printf("-> ");
				printf("%d\r\n", DMA_send_feedback(DMA1_Channel6));
			}
			
			if(KEY_Scan(0) == 1)
			{
				send_buf[0] = duty;
				duty += 100;
				DMA_Enable(DMA1_Channel6);
			}
			
	}
	
}
