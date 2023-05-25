#include "timer.h"
#include "led.h"
#include "usart.h"

void TIM3_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	 TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	/* ʹ�ܶ�ʱ��3ʱ�� */
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  /* ʹ��GPIO���� */   
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; /* TIM_CH1*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  /* ����������� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);/* ��ʼ��GPIO */
 
   /* ��ʼ��TIM3 */
	TIM_TimeBaseStructure.TIM_Period = arr; /* ��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ */
	TIM_TimeBaseStructure.TIM_Prescaler =psc; /* ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ */
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; /* ����ʱ�ӷָ�:TDTS = Tck_tim */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  /*TIM���ϼ���ģʽ */
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); /* ����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ */
	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; /* ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ1 */
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; /* �Ƚ����ʹ�� */
	//TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  /* ʹ��TIM3��CCR1�ϵ�Ԥװ�ؼĴ���*/
	TIM_OCInitStructure.TIM_Pulse= 100;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; /* �������:TIM����Ƚϼ��Ը� */
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  /* ����Tָ���Ĳ�����ʼ������TIM3 OC1 */
	//TIM_DMACmd(TIM3, TIM_DMA_Update, ENABLE);	/* �����Ҫ����ռ�ձȾͰ�����ȥ��ע�ͣ�Ȼ��ע�͵��������У��ٰ�DMAͨ��6��ΪDMAͨ��3 */
	TIM_DMACmd(TIM3, TIM_DMA_CC1, ENABLE);
	TIM_Cmd(TIM3, ENABLE);  /* ʹ��TIM3 */
	
}



/*
#define SEND_BUF_SIZE 8200	//�������ݳ���,��õ���sizeof(TEXT_TO_SEND)+2��������.
u8 SendBuff[SEND_BUF_SIZE];	//�������ݻ�����

void MYDMA_Init(){
	
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //ʹ�� DMA ʱ��
	DMA_DeInit(DMA1_Channel6);   // ��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ 
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = TIM3->PSC; 	//TIM3CH1 CRR�Ĵ���
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)SendBuff; //DMA �ڴ����ַ Ҫ�������ݵĵ�ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  //���ݴ��䷽�򣬴��ڴ��ȡ���͵�����
	DMA_InitStructure.DMA_BufferSize = SEND_BUF_SIZE;  //DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //��������������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMAͨ�� xӵ�������ȼ� 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	
	
	
	DMA_Init(DMA1_Channel6, &DMA_InitStructure);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���
	
}

u16 DMA1_MEM_LEN;

//����һ��DMA����
void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx)
{
	//SendBuff[0] = 100;
	int i;
	for (i = 0; i < sizeof(SEND_BUF_SIZE); i++){
		SendBuff[i] = i;
	}
	
	DMA_Cmd(DMA_CHx, ENABLE);  //�ر�USART1 TX DMA1 ��ָʾ��ͨ��      
 	DMA_SetCurrDataCounter(DMA_CHx,DMA1_MEM_LEN);//DMAͨ����DMA����Ĵ�С
 	DMA_Cmd(DMA_CHx, DISABLE);  //ʹ��USART1 TX DMA1 ��ָʾ��ͨ�� 

}

*/


Motor m;

// �Ƕ�ת����  ͨ���Ƕ��������÷���
void step_angletwopulse(int angle){	
	
	if(angle >= 0) m.dir = 1;
	else m.dir = 0;
	
	m.pulsesNum = angle / m.stepAngle / m.subdivision;
	
	if(m.pulsesNum == 0){ //���������Ϊ0 ��ر�PWM���
			
		TIM_Cmd(TIM3, DISABLE);  //�ر�TIM3
	
	}
	else{
	
		TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3

	}
	
}



// ���ƶ�ʱ��������ٸ�������
void step_pulseNum(int pulsenum){
	
	if(pulsenum != 0){  //�����������Ϊ0�����������
			
	}
	
}





