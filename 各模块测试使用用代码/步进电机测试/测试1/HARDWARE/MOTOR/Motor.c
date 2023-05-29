#include "Motor.h"
#include "timer.h"


/***************************************************************
���������� void CLK_Init(void)
����  �ܡ� ��ʼ��������������������
������ֵ�� ��
������ֵ�� ��
****************************************************************/
void CLK_Init(void){
	
		GPIO_InitTypeDef  GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	  
		//TIM1_4
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;                   //TIM1_CH4 PA11
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;             //�����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		//TIM4_2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 									//TIM4_CH2��T
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;             //�����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}

/***************************************************************
���������� void EN_Init(void)
����  �ܡ� ��ʼ���������ʹ������
������ֵ�� ��
������ֵ�� ��
****************************************************************/
void EN_Init(void){
	
		GPIO_InitTypeDef  GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB,PE�˿�ʱ��
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_0;  //
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
		GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
		GPIO_SetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1);
}

/***************************************************************
���������� void DIR_Init(void)
����  �ܡ� ��ʼ�����Ʋ��������������
������ֵ�� ��
������ֵ�� ��
****************************************************************/
void DIR_Init(void){
	
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;				 //LED0-->PB.5 �˿�����
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
		GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
	
}

/***************************************************************
���������� void main_Init(void)
����  �ܡ� ��ʼ�����������ʼ������
������ֵ�� ��
������ֵ�� ��
****************************************************************/
void main_Init(void){
	
	CLK_Init();//�������ʹ��
	
	EN_Init();//ʹ�����ų�ʼ��
	
	DIR_Init();//�����ʼ��
	
}







