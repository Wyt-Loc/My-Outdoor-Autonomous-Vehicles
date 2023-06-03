#include "sys.h"
#include "usart.h"	  
#include "led.h"
#include "motorT.h"
#include "Servos.h"
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
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
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


#define radtor  0.01591549


__IO uint32_t g_set_speed  = 400;//* radtor;    //Ȧ/s    //600 * 5.729578;   /* ����ٶ� ��λΪ 0.1rad/sec */   //0.1����ÿ��  1rad = 5.729578
__IO uint32_t g_step_accel = 20;//* radtor;               /* ���ٶ� ��λΪ 0.1rad/sec^2 */  //0.1����ÿ2�η���
__IO uint32_t g_step_decel = 20;//* radtor;               /* ���ٶ� ��λΪ 0.1rad/sec^2 */  //0.1����ÿ2�η���
__IO uint16_t g_step_angle = 50;             /* ���õĲ���*/   //��ӦȦ�� 8ϸ����


extern __IO uint32_t g_add_pulse_count;     /* ��������ۼ�*/



void usartReceData(void){
	u8 len=0; int t;
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










#endif	

