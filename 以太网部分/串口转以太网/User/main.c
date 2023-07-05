#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/ATK-UART2ETH/atk_u2e.h"
#include "./BSP/USART3/usart3.h"
#include "string.h"

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
    for(int32_t i=0;i<len;i++)
    {
        while ((USART_UX->SR & 0X40) == 0);     /* �ȴ���һ���ַ�������� */
        USART_UX->DR = buf[i];             /* ��Ҫ���͵��ַ� ch д�뵽DR�Ĵ��� */
    }
}







int key = 42;
void decrypt_data(char *data, int length, int key)
{
    for (int i = 0; i < length; i++)
    {
        data[i] = data[i] ^ key;
    }
}


int main(void)
{
    uint8_t key;
    uint8_t i = 0;
    uint16_t rx_len = 0;

    sys_stm32_clock_init(9);        /* ����ʱ��, 72Mhz */
    delay_init(72);                 /* ��ʱ��ʼ�� */
    usart_init(115200);             /* ���ڳ�ʼ��Ϊ115200 */
    led_init();                     /* ��ʼ��LED */
    key_init();                     /* ��ʼ������ */

    u2e_config_init(U2E_TCP_CLIENT); /* ѡ��ģʽ */

    while (1)
    {
        key = key_scan(0);      /* ��֧������ */
			
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
            LED0_TOGGLE();  /* LED0��˸ */
        }
    }
}




