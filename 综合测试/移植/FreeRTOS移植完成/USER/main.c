#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "dma.h"
#include "timer.h"
#include "led.h"
#include "motorT.h"


#ifdef x
#define size 1
extern u16 DMA1_MEM_LEN;
extern DMA_InitTypeDef DMA_InitStructure;  
u16 send_buf[size];
#endif


__IO uint32_t g_set_speed  = 5000;          /* ����ٶ� ��λΪ0.1rad/sec */
__IO uint32_t g_step_accel = 80;            /* ���ٶ� ��λΪ0.1rad/sec^2 */
__IO uint32_t g_step_decel = 60;            /* ���ٶ� ��λΪ0.1rad/sec^2 */
__IO uint16_t g_step_angle = 30;             /* ���õĲ�����Ҳ���Ƕ���Ȧ*/

extern __IO uint32_t g_add_pulse_count;     /* ��������ۼ�*/

int main(void)
{

	delay_init();
	LED_Init();
	uart_init(115200);
	KEY_Init();
	time3_initer(0xffff, 71);
	delay_ms(20);
	
	while(1)
	{
		
		if(KEY_Scan(0) == 3){
			
			create_t_ctrl_param(SPR*g_step_angle, g_step_accel, g_step_decel, g_set_speed); //Ȧ��  ���ٶ�  ���ٶ�  ����ٶ�
			g_add_pulse_count=0;

		}
	}
}
