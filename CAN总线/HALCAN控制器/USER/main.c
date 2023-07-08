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
#include "MatchingCommands.h"





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


void Nextval(char T[],int *next)
{
	int lenT=strlen(T);
	int k=-1;//ǰ׺ 
	int j=0;//��׺ 
	next[0]=-1; 
	
	while(j<lenT)
	{
		if(k==-1||T[j]==T[k])
		{
			j++;
			k++;
			//��֮ǰnext�����󷨣��Ķ�������4��
			if(T[j]!=T[k])
			{
				next[j]=k;//֮ǰֻ����һ��
			}
			else
			{
				next[j]=next[k];/*Ϊ���ܳ���T[j] = T[ next[j ]]��
				                  ���Ե�����ʱ�����������λ��ݣ�k = next[next[k]]  */ 
			 } 
		}
		else 
		{
			k=next[k];
		 } 
	}
}

int KMP(uint8_t S[],uint8_t T[])
{
	int i=0,j=0,lenS,lenT;
	int next[100];
	
	lenS=strlen((const char *)S);
	lenT=strlen((const char *)T);
	
	Nextval((char*)T,next);
	
	while(i<lenS&&j<lenT)
	{
		if(j==-1||S[i]==T[j])
		{
			i++;
			j++;
		}
		else
		{ 
			j=next[j];
		}
	}
	 
	if(j==lenT)
	{
	    printf("Match succeed!\n");
	 	return i-j;
	} 
	else 
	{
		printf("Match failed!\n");
		return -1;
    }
}

//��������
u8 parsingDirData(uint8_t arr[]){
	
	u8 data = 2;
	
	if((arr[0] - '0') == 1)
		return 1;
	else if ((arr[0] - '0') == 0)
		return 0;
	
	return data;
}

//��������
float parsingDisData(uint8_t arr[]){
	
	float data;
	float shiwei = 0; // ʮλ
	float gewei = 0;  // ��λ
	float hou1 = 0; // С�����һλ
	float hou2 = 0;  // С�������λ
	
	// ��������Ϊ
	if((arr[1] - '0') == 0){
		gewei = arr[2] - '0';
		hou1 = arr[3] - '0';
		hou2 = arr[4] - '0';
	}
	
	//���һ�β�����10.00
	else if((arr[1] - '0') != 0){
		shiwei = arr[1] - '0';
		gewei = arr[2] - '0';
		hou1 = arr[3] - '0';
		hou2 = arr[4] - '0';
	}
	printf("shiwei = %f, gewei = %f, hou1 = %f,hou2 = %f", shiwei, gewei, hou1, hou2);
	// ����
	data = shiwei*10 + gewei + (hou1 / 10.0f) + (hou2 / 100.0f);

	return data;
}

//�����ٶ�
float parsingVData(uint8_t arr[]){
	
	float data;
	float shiwei = 0; // ʮλ
	float gewei = 0;  // ��λ
	float hou1 = 0; // С�����һλ
	float hou2 = 0;  // С�������λ
	//0 < speed < 10   0 < distance < 50
	
	// �ٶ�����Ϊ
	if((arr[5] - '0') == 0){
		gewei = arr[6] - '0';
		hou1 = arr[7] - '0';
		hou2 = arr[8] - '0';
	}
	
	//���һ�β�����10.00
	else if((arr[5] - '0') != 0){
		shiwei = arr[5] - '0';
		gewei = arr[6] - '0';
		hou1 = arr[7] - '0';
		hou2 = arr[8] - '0';
	}
	
	// �ٶ�
	data = shiwei*10 + gewei + (hou1 / 10.0f) + (hou2 / 100.0f);
	
	return data;
	
}


int main(void)
{
	
  u8 key;
	short target = 0;
	short target1 = 0;
	u8 i = 0;
	int rx_len = 0;
	uint8_t ok[2]={"ok"};
	uint8_t one[8] = {0}; // ֡ͷ ȷ��֡
	uint8_t onecp[8] = "0xAAAAx0";
	uint8_t fun1[1]={"m"};  
	uint8_t fun2[1]={"s"};
	uint8_t two[9] = {0};   // �������
	uint8_t three[4] = {0};  // �������
	
  HAL_Init();                    	 	//��ʼ��HAL��
  Stm32_Clock_Init(RCC_PLL_MUL9);   	//����ʱ��,72M
	delay_init(72);               		//��ʼ����ʱ����
	uart_init(115200);					//��ʼ������
	LED_Init();							//��ʼ��LED
	KEY_Init();							//��ʼ������
 	CAN1_Mode_Init(CAN_SJW_1TQ,CAN_BS2_8TQ,CAN_BS1_9TQ,4,CAN_MODE_NORMAL); //CAN��ʼ��,������500Kbps      
	CAN_Config();
	TIM3_Init(100-1,72-1);    // ÿ�����10000�� can��Ϣ������
	u2e_config_init(U2E_TCP_CLIENT); /* ѡ��ģʽ */

	while (1)
	{
			u8 js=0;
			key = KEY_Scan(0);      /* ��֧������ */
			switch (key)
			{
					case KEY0_PRES:     /* KEY0���� */
						//sendMotorCommand(1, 13.14f, 9.99f);
							usart3_send_data((uint8_t*)test_buf[U2E_TCP_CLIENT],strlen((const char*)test_buf[U2E_TCP_CLIENT]));      /* �������ݵ������� */
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
					target=KMP(g_data_rxbuf,fun1);
					target1=KMP(g_data_rxbuf,fun2);
				
				for(i=0;i<8;i++){
					one[i]=g_data_rxbuf[i];
				}
				
				for(i=0;i<8;i++){
					if(one[i]==onecp[i])
						js++;
				}
				if(js==8)
				{
					target=KMP(g_data_rxbuf,fun1);
					target1=KMP(g_data_rxbuf,fun2);
					
					printf("target = %d   %d ", target,target1);
					
					if(target == 8 && target1 == 18)
					{
						for(i=target+1;i<target1;i++)
						{
							two[i-9] = g_data_rxbuf[i];
							
						}
					}
					printf("������� = %s\r\n", two);
					for(i=target1+1;i<target1+5;i++){
						three[i-target1-1] = g_data_rxbuf[i];
					}
					for(i =0;i<4;i++)
						three[i] = three[i];
					
					usart3_send_data((uint8_t*)ok,2);      /* �������ݵ������� */
					
					//ControlEntryFunction(1, 13.14f, 9.99f);
					printf("dir = %d, dis = %f, V = %f", parsingDirData(two), parsingDisData(two), parsingVData(two) );
					
					ControlEntryFunction((u8)parsingDirData(two), parsingDisData(two), parsingVData(two));
				}
				
				//usart1_send_data(g_data_rxbuf,rx_len);       /* ת��������1 */
				//	printf("\r\n");
					rx_len = 0;
			}
	}
	
}
