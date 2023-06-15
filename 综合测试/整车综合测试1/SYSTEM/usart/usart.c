#include "sys.h"
#include "usart.h"	  
#include "led.h"
#include "Servos.h"
#include "motorT.h"
#include <stdio.h>
#include <string.h>
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif

#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF1[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u8 USART_RX_BUF2[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.

//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA1=0;       //����״̬���	  
u16 USART_RX_STA2=0;       //����״̬���	   


void uart_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}

void USART1_IRQHandler(void)                	//����1�жϷ������
	{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		
		if((USART_RX_STA1&0x8000)==0)//����δ���
			{
			if(USART_RX_STA1&0x4000)//���յ���0x0d
				{
				if(Res!=0x0a)USART_RX_STA1=0;//���մ���,���¿�ʼ
				else USART_RX_STA1|=0x8000;	//��������� 
				}
			else //��û�յ�0X0D
				{	
				if(Res==0x0d)USART_RX_STA1|=0x4000;
				else
					{
					USART_RX_BUF1[USART_RX_STA1&0X3FFF]=Res ;
					USART_RX_STA1++;
					if(USART_RX_STA1>(USART_REC_LEN-1))USART_RX_STA1=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			}   		 
     } 
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
}



















void uart2_init(u32 bound){
	 //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);	//ʹ��USART2ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	//ʹ��GPIOAʱ��
	USART_DeInit(USART2);  //��λ����2
	 //USART2_TX   PA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA2
 
	 //USART2_RX	  PA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA3
 
 
	//USART ��ʼ������
 
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
 
	 USART_Init(USART2, &USART_InitStructure); //��ʼ������
	#if EN_USART1_RX		  //���ʹ���˽���  
	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
 
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
	#endif
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 
 
}


void USART2_IRQHandler(void)                	//����2�жϷ������
{
	u8 Res;
	#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntEnter();
	#endif
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res = USART_ReceiveData(USART2);//(USART1->DR);	//��ȡ���յ�������
		
		if((USART_RX_STA2&0x8000)==0)//����δ���
		{
			if(USART_RX_STA2&0x4000)//���յ���0x0d
			{
				if(Res!=0x0a)USART_RX_STA2=0;//���մ���,���¿�ʼ
				else USART_RX_STA2|=0x8000;	//��������� 
			}
			else //��û�յ�0X0D
			{	
				if(Res==0x0d)USART_RX_STA2|=0x4000;
				else
				{
					USART_RX_BUF2[USART_RX_STA2&0X3FFF]=Res ;
					USART_RX_STA2++;
					if(USART_RX_STA2>(USART_REC_LEN-1))USART_RX_STA2=0;//�������ݴ���,���¿�ʼ����	  
				}
			}
		}
	 }
	#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
	OSIntExit();  											 
	#endif
}



u8 usartRecvData(void){
	
	u8 len,t;
	
	if(USART_RX_STA2&0x8000)   // �Խ������ݽ����ж�
	{
	
	 len=USART_RX_STA2&0x3fff;//�õ��˴ν��յ������ݳ���
		printf("%d",len);
	
	for(t=0;t<len;t++)
	{
		USART_SendData(USART1, USART_RX_BUF2[t]);//�򴮿�1��������
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	}
		
	switch(len){
			
		case 5:
			if(!strcmp(LEFTS,(const char*)USART_RX_BUF2)){  //��ת�㰴
				memset(USART_RX_BUF2,0,sizeof(USART_RX_BUF2));
				return  1;
			}
			if(!strcmp(LEFTL,(const char*)USART_RX_BUF2)){  // ��ת����
				memset(USART_RX_BUF2,0,sizeof(USART_RX_BUF2));
				return 2;
			}
			if(!strcmp(BACKS,(const char*)USART_RX_BUF2)){   //���˵㰴
				memset(USART_RX_BUF2,0,sizeof(USART_RX_BUF2));
				return 3; 
			}
			if(!strcmp(BACKL,(const char*)USART_RX_BUF2)){   //���˳���
				memset(USART_RX_BUF2,0,sizeof(USART_RX_BUF2));
				return 4; 
			}
			break;
		
		case 6:
			if(!strcmp(RIGHTS,(const char*)USART_RX_BUF2)){  //��ת�㰴
				memset(USART_RX_BUF2,0,sizeof(USART_RX_BUF2));
				return  5;
			}
			if(!strcmp(RIGHTL,(const char*)USART_RX_BUF2)){  //��ת����
				memset(USART_RX_BUF2,0,sizeof(USART_RX_BUF2));
				return  6;
			}
			break;
		
		case 9:
			if(!strcmp(ADVANCESS,(const char*)USART_RX_BUF2)){  //ǰ���㰴
				memset(USART_RX_BUF2,0,sizeof(USART_RX_BUF2));
				return  7;
			}
			if(!strcmp(ADVANCESL,(const char*)USART_RX_BUF2)){  //ǰ������
				memset(USART_RX_BUF2,0,sizeof(USART_RX_BUF2));
				return  8;
			}
			if(!strcmp(STOPMOTOR,(const char*)USART_RX_BUF2)){  //ֹͣ�������
				memset(USART_RX_BUF2,0,sizeof(USART_RX_BUF2));
				return  10;
			}
			break;
			
		case 10:
				if(!strcmp(STOPSERVOS,(const char*)USART_RX_BUF2)){  //ֹͣ�������
				memset(USART_RX_BUF2,0,sizeof(USART_RX_BUF2));
				return  9;
			}
			break;
	}
		USART_RX_STA2=0;
	}
	return 0;
	
}


__IO uint32_t g_set_speed  = 80;//* radtor;    //Ȧ/s    //600 * 5.729578;   /* ����ٶ� ��λΪ 0.1rad/sec */   //0.1����ÿ��  1rad = 5.729578
__IO uint32_t g_step_accel = 20;//* radtor;               /* ���ٶ� ��λΪ 0.1rad/sec^2 */  //0.1����ÿ2�η���
__IO uint32_t g_step_decel = 20;//* radtor;               /* ���ٶ� ��λΪ 0.1rad/sec^2 */  //0.1����ÿ2�η���
__IO uint16_t g_step_angle = 5;             /* ���õĲ���*/   //��ӦȦ�� 8ϸ����


extern __IO uint32_t g_add_pulse_count;     /* ��������ۼ�*/

extern u8 clickorlongservos;
extern u8 clickorlongmotor;

void Control(u8 value){
		
	switch(value){
		case 1:
			ServosLeftRightClick(1);  //��ת�㰴
			printf("��ת�㰴");
			break;
		case 5:
			ServosLeftRightClick(2);  //��ת�㰴
			printf("��ת�㰴");
			break;
		case 6:
			clickorlongservos = 2;
			printf("��ת����");
			break;
		case 2:
			clickorlongservos = 1;
			printf("��ת����");
			break;
		case 7:
			create_t_ctrl_param(SPR*g_step_angle, g_step_accel, g_step_decel, g_set_speed);
			g_add_pulse_count=0;
			printf("ǰ���㰴");
			break;
		case 3:
			create_t_ctrl_param(-SPR*g_step_angle, g_step_accel, g_step_decel, g_set_speed);
			g_add_pulse_count=0;
			printf("���˵㰴");
			break;
		case 4:
			clickorlongmotor = 1;
			create_t_ctrl_param(-SPR*g_step_angle*3, g_step_accel, g_step_decel, g_set_speed);
			g_add_pulse_count=0;
			printf("���˳���");
			break;
		case 8:
			clickorlongmotor = 1;
			create_t_ctrl_param(SPR*g_step_angle*3, g_step_accel, g_step_decel, g_set_speed);
			g_add_pulse_count=0;
			printf("ǰ������");
			break;
		case 9:
			servosStop(&clickorlongservos);
			printf("���ֹͣ");
			break;
		case 10:
			motorStopLong(&clickorlongmotor);
			printf("���ֹͣ");
			break;
	}
	
}







#endif	

