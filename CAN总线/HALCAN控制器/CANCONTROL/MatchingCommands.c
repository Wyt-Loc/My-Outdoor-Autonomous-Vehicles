/*		�������				*/
#include "MatchingCommands.h"
#include "canControl.h"
#include "string.h"
#include "atk_u2e.h"
#include "usart3.h"
#include "usart.h"





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

void sendata(){
	
	uint16_t rx_len = 0;
	static uint8_t g_data_rxbuf[256];       /* ���ڽ������ݵĻ��� */
	
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
	
}


