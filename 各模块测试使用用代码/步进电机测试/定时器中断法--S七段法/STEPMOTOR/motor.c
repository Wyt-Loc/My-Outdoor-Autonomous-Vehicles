#include "motor.h"
#include "s_curve.h"



void Motor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_PUL1_CLK | RCC_DIR1_CLK | RCC_ENA1_CLK, ENABLE);
    // 脉冲口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_PUL1_Pin;
    GPIO_Init(GPIO_PUL1, &GPIO_InitStructure);
    GPIO_ResetBits(GPIO_PUL1, GPIO_PUL1_Pin);

    // 方向
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_DIR1_Pin;
    GPIO_Init(GPIO_DIR1, &GPIO_InitStructure);
    GPIO_ResetBits(GPIO_DIR1, GPIO_DIR1_Pin);

    // 使能
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_ENA1_Pin;
    GPIO_Init(GPIO_ENA1, &GPIO_InitStructure);
    GPIO_ResetBits(GPIO_ENA1, GPIO_ENA1_Pin);
}

void TIM5_Int_Init(u16 arr, u16 psc)
{

    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    TIM_DeInit(TIM5);                                    /*复位TIM1定时器*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); /*开时钟*/
    TIM_TimeBaseStructure.TIM_Period = arr;              /*时钟滴答的次数，够数中断这里是1ms中断一次*/
    TIM_TimeBaseStructure.TIM_Prescaler = psc;           /* 分频720*/
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; /*计数方向向上计数*/
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

    /* Clear TIM1 update pending flag  清除TIM1溢出中断标志]  */
    TIM_ClearFlag(TIM5, TIM_FLAG_Update);
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn; /*溢出中断*/
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE); /* Enable TIM1 Update interrupt TIM1溢出中断允许*/
		
}

void TIM5_IRQHandler(void)
{
    TIM_ClearFlag(TIM5, TIM_FLAG_Update);
}
