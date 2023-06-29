#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "usart3.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "usmart.h"
#include "can.h"
#include "timer.h"
#include "canControl.h"
#include "string.h"
#include "atk_u2e.h"


/*******************************************/
char *Modbuf[]={
    "UDP_SERVER",
    "UDP_CLIENT",
    "TCP_Server",
    "TCP_Client",
    "TCP_CLOUD ",
    "UDP_MULTICAST",
};

char *test_buf[]={
    "Data from UDP_Server mode",
    "Data from UDP_Client mode",
    "Data from TCP_Server mode",
    "Data from TCP_Client mode",
    "CLOUD.ALIENTEK.COM",
    "Data from 233.0.0.6",
};

uint8_t g_data_rxbuf[256];       /* ���ڽ������ݵĻ��� */


/**
 * @brief       ����1���ͺ���
 * @param       *buf : ���ͻ�����ָ��
 * @param       len  : Դ��ַ
 * @param       n    : ��Ҫ���Ƶ��ڴ泤��(�ֽ�Ϊ��λ)
 * @retval      ��
 */
void usart1_send_data(uint8_t* buf,uint32_t len)
{
	int32_t i = 0;
    for(i=0;i<len;i++)
    {
        while ((USART1->SR & 0X40) == 0);     /* �ȴ���һ���ַ�������� */
        USART1->DR = buf[i];             /* ��Ҫ���͵��ַ� ch д�뵽DR�Ĵ��� */
    }
}


int main(void)
{
	
  u8 key;
	uint8_t i = 0;
  uint16_t rx_len = 0;
	
  HAL_Init();                    	 	//��ʼ��HAL��    
  Stm32_Clock_Init(RCC_PLL_MUL9);   	//����ʱ��,72M
	delay_init(72);               		//��ʼ����ʱ����
	uart_init(115200);					//��ʼ������
	usmart_dev.init(84); 		   		//��ʼ��USMART	
	LED_Init();							//��ʼ��LED	
	KEY_Init();							//��ʼ������
 	CAN1_Mode_Init(CAN_SJW_1TQ,CAN_BS2_8TQ,CAN_BS1_9TQ,4,CAN_MODE_NORMAL); //CAN��ʼ��,������500Kbps      
	CAN_Config();
	TIM3_Init(100-1,72-1);    // ÿ�����10000��
	u2e_config_init(U2E_TCP_CLIENT); /* ѡ��ģʽ */
	
	while (1)
	{
			key = KEY_Scan(0);      /* ��֧������ */
			switch (key)
			{
					case KEY0_PRES:     /* KEY0���� */
							usart3_send_data((uint8_t*)test_buf[U2E_TCP_CLIENT],strlen((const char*)test_buf[U2E_TCP_CLIENT]));      /* �������ݵ�ԭ���� */
							break;
					default:break;
			}

			/* ͸�������£��Ӷ����а�����������ȡ��������ת��������1 */
			while (res_usart3_revdata())
			{
					usart3_getRxData(&g_data_rxbuf[rx_len++], 1); /* �Ӵ��ڻ����ж�һ���ֽ� */

					if (rx_len >= U2E_RX_CMD_BUF_SIZE)
					{
							rx_len = 0; /*����Ӧ�����ݳ�����������������*/
					}
			}
			if(rx_len>0)
			{
					usart1_send_data(g_data_rxbuf,rx_len);       /* ת��������1 */
					printf("\r\n");
					rx_len = 0;
			}
			
			delay_ms(10);
			i++;

			if ((i % 20) == 0)
			{
					LED1 = ~LED1;  /* LED0��˸ */
			}
	}
	
	/*
	while(1){
		key = KEY_Scan(0);
		if(key==KEY0_PRES)
		{
			ControlEntryFunction(1, 13.14f, 9.99f);
			key = 0;
		}
	}
	*/

}

