#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"

/************************************************
 ALIENTEK��ӢSTM32������ʵ��4
 ���� ʵ��   
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/
 int main(void)
 {		
 	u16 t;  
	u16 len;	
	u16 times=0;
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
	uart2_init(115200);
 	LED_Init();			     //LED�˿ڳ�ʼ��
	KEY_Init();          //��ʼ���밴�����ӵ�Ӳ���ӿ�
 	
	while(1)
	{
			if(USART_RX_STA2&0x8000)
			{
				len=USART_RX_STA2&0x3fff;//�õ��˴ν��յ������ݳ���
				for(t=0;t<len;t++)
				{
					USART_SendData(USART1, USART_RX_BUF2[t]);//�򴮿�1��������
					while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
				}
				
				if(USART_RX_BUF2[0] == 'u' && USART_RX_BUF2[1] == 'p'){
					printf("����");
				}
				if(USART_RX_BUF2[0] == 'd' && USART_RX_BUF2[1] == 'o' && USART_RX_BUF2[2] == 'w' && USART_RX_BUF2[3] == 'n'){
						printf("����");
				}
				if(USART_RX_BUF2[0] == 's' && USART_RX_BUF2[1] == 't' && USART_RX_BUF2[2] == 'o' && USART_RX_BUF2[3] == 'p'){
					printf("ͣ");
				}
				USART_RX_STA2=0;
			}
	}
}
 

