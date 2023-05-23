#include "timer.h"
#include "led.h"
#include "usart.h"


//���1����
/***��ʱ��1��ģʽ***/
void TIM1_config(u32 Cycle) //����
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE); 

    TIM_TimeBaseStructure.TIM_Period = Cycle-1; //����ʱ��                                           
    TIM_TimeBaseStructure.TIM_Prescaler =71;                    //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ                                                     
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //����ʱ�ӷָTDTS= Tck_tim            
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���ģʽ
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;            //�ظ�������һ��Ҫ=0������
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);                                       

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;          //ѡ��ʱ��ģʽ��TIM�����ȵ���ģʽ1       
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_Pulse = Cycle/2-1;                    //���ô�װ�벶��Ĵ���������ֵ                                   
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      //�������       
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);                                                         

	
    TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable);
    TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);

    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);                              
    TIM_ARRPreloadConfig(TIM1, ENABLE);                                                          
}


/***��ʱ��2��ģʽ***/
void TIM2_config(u32 PulseNum)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure; 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = PulseNum-1;   
    TIM_TimeBaseStructure.TIM_Prescaler =0;    
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);  

    TIM_SelectInputTrigger(TIM2, TIM_TS_ITR0);
    TIM2->SMCR|=0x07;                                  //���ô�ģʽ�Ĵ���   
    TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;        
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;     
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure);
}


void Pulse_output_1(u32 Cycle,u32 PulseNum)
{
    TIM2_config(PulseNum); 
    TIM_Cmd(TIM2, ENABLE);
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM1_config(Cycle); //��������
    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);   //�߼���ʱ��һ��Ҫ���ϣ������ʹ��
}


void TIM2_IRQHandler(void) 
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)     // TIM_IT_CC1
    { 
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // ����жϱ�־λ 
        TIM_CtrlPWMOutputs(TIM1, DISABLE);  //�����ʹ��
        TIM_Cmd(TIM1, DISABLE); // �رն�ʱ��
        TIM_Cmd(TIM2, DISABLE); // �رն�ʱ�� 
        TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE); 
    }
}


//���2����
//��ʱ��4��ģʽ,��ʱ��3��ģʽ
//PWM���
void TIM4_config(u32 Cycle)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//��ʱ�����ýṹ��
    TIM_OCInitTypeDef  TIM_OCInitStructure; //pwm����Ӧ���ýṹ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //����ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE); //����ʱ��
 
    TIM_TimeBaseStructure.TIM_Period = Cycle-1;                                                   
    TIM_TimeBaseStructure.TIM_Prescaler =71;                    //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ                                                     
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //����ʱ�ӷָTDTS= Tck_tim            
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);                                       
 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;          //ѡ��ʱ��ģʽ��TIM�����ȵ���ģʽ1       
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_Pulse = Cycle/2-1;                    //���ô�װ�벶��Ĵ���������ֵ                                   
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      //�������       
    TIM_OC2Init(TIM4, &TIM_OCInitStructure);                                                         
 
    TIM_SelectMasterSlaveMode(TIM4, TIM_MasterSlaveMode_Enable);
    TIM_SelectOutputTrigger(TIM4, TIM_TRGOSource_Update);
 
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);                              
    TIM_ARRPreloadConfig(TIM4, ENABLE);                                                          
}


/***��ʱ��3��ģʽ***/
void TIM3_config(u32 PulseNum)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure; 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
 
    TIM_TimeBaseStructure.TIM_Period = PulseNum-1; //�����Զ���װ������ֵ
    TIM_TimeBaseStructure.TIM_Prescaler =0;    
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;     
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);  
 
    TIM_SelectInputTrigger(TIM3, TIM_TS_ITR3);
    TIM_SelectSlaveMode(TIM3,TIM_SlaveMode_External1 );// ��ͬ TIM2->SMCR|=0x07 //���ô�ģʽ�Ĵ��� 
    //   TIM2->SMCR|=0x07;            
    TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE);
 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void Pulse_output_2(u32 Cycle,u32 PulseNum)
{
    TIM3_config(PulseNum);
    TIM_Cmd(TIM3, ENABLE);
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
    TIM4_config(Cycle);
    TIM_Cmd(TIM4, ENABLE);
}

void TIM3_IRQHandler(void) 
{ 
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)     // TIM_IT_CC1
    { 
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // ����жϱ�־λ 
        TIM_CtrlPWMOutputs(TIM3, DISABLE);  //�����ʹ��
        TIM_Cmd(TIM4, DISABLE); // �رն�ʱ�� 
        TIM_Cmd(TIM3, DISABLE); // �رն�ʱ�� 
        TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);         
    }
}


/***************************************************************
���������� u8 Speed_And_Angle_Motor1(int v, int dis)
����  �ܡ� ���1 -----  ���Ʋ������ת�����ٶȺ��ܹ�Ҫת���ĽǶ�
������ֵ�� v  �ٶ�      dis �Ƕ�    �ο� Pulse_output_1(1000,8000);   1000hz   8000������
������ֵ�� 1 ת����,  0 ûת����.
�����㷽����˵���� ���Ǽ��轫������õ��ϸ��Ϊ x��  
****************************************************************/
u8 Speed_And_Angle_Motor1(int v, int dis){
	
	
	
	Pulse_output_1(1000,8000);
	
	
	return 1;
}
























