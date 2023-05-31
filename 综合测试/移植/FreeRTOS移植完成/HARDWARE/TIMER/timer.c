#include "timer.h"
#include "led.h"
#include "usart.h"


#ifdef x


void TIM3_PWM_Init(uint16_t arr,uint16_t psc)
{  
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;                                      //����NVIC_InitTypeDef �����ṹ����
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	/* ʹ�ܶ�ʱ��3ʱ�� */
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  /* ʹ��GPIO���� */   
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; /* TIM_CH1*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  /* ����������� */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);/* ��ʼ��GPIO */
	
   /* ��ʼ��TIM3 */
	TIM_TimeBaseStructure.TIM_Period = arr; /* ��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ */
	TIM_TimeBaseStructure.TIM_Prescaler =psc; /* ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ */
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//0; /* ����ʱ�ӷָ�:TDTS = Tck_tim */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  /*TIM���ϼ���ģʽ */
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); /* ����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ */
	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;  //TIM_OCMode_PWM1; /* ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ1 */
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; /* �Ƚ����ʹ�� */
	
	//TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  /* ʹ��TIM3��CCR1�ϵ�Ԥװ�ؼĴ���*/
	TIM_OCInitStructure.TIM_Pulse= 0;

	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; /* �������:TIM����Ƚϼ��Ը� */

	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  /* ����Tָ���Ĳ�����ʼ������TIM3 OC1 */
	
	//�����ж�
	//TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);  //ʹ�ܶ�ʱ���ĸ����жϣ������ж������ж������һ�֣�
	
	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;       	 			//TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;			//�����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; 					//�����ȼ�Ϊ3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;     				//ʹ��IRQͨ�� 				
	NVIC_Init(&NVIC_InitStructure);     			//��ʼ��NVIC�Ĵ���
	
	//TIM_DMACmd(TIM3, TIM_DMA_Update, ENABLE);	/* �����Ҫ����ռ�ձȾͰ�����ȥ��ע�ͣ�Ȼ��ע�͵��������У��ٰ�DMAͨ��6��ΪDMAͨ��3 */
	TIM_DMACmd(TIM3, TIM_DMA_CC1, ENABLE);
	//TIM_Cmd(TIM3, DISABLE);  /* ʹ��TIM3 */
	
}

//DMA_Config(DMA1_Channel6, (u32)&TIM3->PSC, (u32)send_buf, size);

#endif



u16 oc1crr1=4915;
_Bool oc1_sta=0;


void time3_initer(u16 arr,u16 psc)
{
	
	 GPIO_InitTypeDef GPIO_InitTypestu;
	 TIM_TimeBaseInitTypeDef TIM_TimeBaseInitTypestu;
	 NVIC_InitTypeDef NVIC_InitTypeDST;
	 TIM_OCInitTypeDef TIM_OCInitTypeSTU;
	
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
   
	 GPIO_InitTypestu.GPIO_Mode=GPIO_Mode_AF_PP;
	 GPIO_InitTypestu.GPIO_Pin=GPIO_Pin_6;
	 GPIO_InitTypestu.GPIO_Speed=GPIO_Speed_10MHz;
	 GPIO_Init(GPIOA,&GPIO_InitTypestu);
	
	 TIM_TimeBaseInitTypestu.TIM_ClockDivision=TIM_CKD_DIV1;
	 TIM_TimeBaseInitTypestu.TIM_CounterMode=TIM_CounterMode_Up;
	 TIM_TimeBaseInitTypestu.TIM_Period=arr;
	 TIM_TimeBaseInitTypestu.TIM_Prescaler=psc;
	 TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitTypestu);
	
	 NVIC_InitTypeDST.NVIC_IRQChannel=TIM3_IRQn;
	 NVIC_InitTypeDST.NVIC_IRQChannelCmd=ENABLE;
	 NVIC_InitTypeDST.NVIC_IRQChannelPreemptionPriority=1;
	 NVIC_InitTypeDST.NVIC_IRQChannelSubPriority=1;
	 NVIC_Init(&NVIC_InitTypeDST);
	 
	 TIM_OCInitTypeSTU.TIM_OCMode=TIM_OCMode_Toggle;
	 TIM_OCInitTypeSTU.TIM_OCPolarity=TIM_OCPolarity_Low;
	 TIM_OCInitTypeSTU.TIM_OutputState=TIM_OutputState_Enable;
	 TIM_OCInitTypeSTU.TIM_Pulse=oc1crr1;
	 TIM_OC1Init(TIM3,&TIM_OCInitTypeSTU);
	 TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Disable);

	 //TIM_Cmd(TIM3,DISABLE);  ��ʼ��ʹ�ܶ�ʱ��
	 //TIM_ITConfig(TIM3,TIM_IT_CC1,ENABLE);
}







