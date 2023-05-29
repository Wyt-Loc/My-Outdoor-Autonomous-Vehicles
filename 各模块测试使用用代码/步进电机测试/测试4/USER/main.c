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

#define radtor  0.01591549


__IO uint32_t g_set_speed  = 600;//* radtor;    //Ȧ/s    //600 * 5.729578;   /* ����ٶ� ��λΪ 0.1rad/sec */   //0.1����ÿ��  1rad = 5.729578
__IO uint32_t g_step_accel = 150;//* radtor;               /* ���ٶ� ��λΪ 0.1rad/sec^2 */  //0.1����ÿ2�η���
__IO uint32_t g_step_decel = 150;//* radtor;               /* ���ٶ� ��λΪ 0.1rad/sec^2 */  //0.1����ÿ2�η���
__IO uint16_t g_step_angle = 50;             /* ���õĲ���*/   //��ӦȦ�� 8ϸ����


extern __IO uint32_t g_add_pulse_count;     /* ��������ۼ�*/


int main(void)
{
	u8 len,t;
	delay_init();
	LED_Init();
	uart_init(115200);
	uart2_init(115200);
	KEY_Init();
	time3_initer(0xffff, 71);
	delay_ms(20);

	while(1)
	{
		//Control_rece();
		
		if(USART_RX_STA2&0x8000)
			{
				len=USART_RX_STA2&0x3fff;//�õ��˴ν��յ������ݳ���
				for(t=0;t<len;t++)
				{
					USART_SendData(USART1, USART_RX_BUF2[t]);//�򴮿�1��������
					while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
				}
				if(USART_RX_BUF2[0] == 'g' && USART_RX_BUF2[1] == 'o'){
					LED1 =~ LED1;
					create_t_ctrl_param(SPR*g_step_angle, g_step_accel, g_step_decel, g_set_speed);
					g_add_pulse_count=0;
					printf("ǰ��");
				}
				if(USART_RX_BUF2[0] == 'b' && USART_RX_BUF2[1] == 'a' && USART_RX_BUF2[2] == 'c' && USART_RX_BUF2[3] == 'k'){
					LED1 =~ LED1;
					create_t_ctrl_param(SPR*g_step_angle*2, g_step_accel*1.2, g_step_decel*1.2, g_set_speed*1.3);
					g_add_pulse_count=0;
					printf("����");
				}
				if(USART_RX_BUF2[0] == 'l' && USART_RX_BUF2[1] == 'e' && USART_RX_BUF2[2] == 'f' && USART_RX_BUF2[3] == 't'){
					LED0 =~ LED0;
					create_t_ctrl_param(SPR*g_step_angle*3, g_step_accel*1.3, g_step_decel*1.3, g_set_speed*1.6);
					g_add_pulse_count=0;
					printf("����");
				}
				if(USART_RX_BUF2[0] == 'r' && USART_RX_BUF2[1] == 'i' && USART_RX_BUF2[2] == 'g' && USART_RX_BUF2[3] == 'h' && USART_RX_BUF2[4] == 't'){
					LED0 =~ LED0;
					create_t_ctrl_param(SPR*g_step_angle*4, g_step_accel*1.4, g_step_decel*1.4, g_set_speed*1.9);
					g_add_pulse_count=0;
					printf("����");
				}
				USART_RX_STA2=0;
			}
	}
}

