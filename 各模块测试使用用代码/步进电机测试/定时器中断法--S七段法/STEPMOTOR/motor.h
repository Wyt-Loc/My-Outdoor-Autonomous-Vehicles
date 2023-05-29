
#ifndef __MOTOR_H
#define __MOTOR_H	 

#include "sys.h"



#define  RCC_PUL1_CLK    RCC_APB2Periph_GPIOB
#define  RCC_DIR1_CLK    RCC_APB2Periph_GPIOD
#define  RCC_ENA1_CLK    RCC_APB2Periph_GPIOD
#define  GPIO_PUL1_Pin       GPIO_Pin_9
#define  GPIO_DIR1_Pin       GPIO_Pin_15
#define  GPIO_ENA1_Pin       GPIO_Pin_13
#define  GPIO_PUL1           GPIOB
#define  GPIO_DIR1           GPIOD
#define  GPIO_ENA1           GPIOD

//��������
#define PULSE1 PBout(9)

//��������
#define DIR1   PDout(15)
#define ENA1   PDout(13)
#define CW 0//˳ʱ��
#define CCW 1//��ʱ��


void Motor_Init(void);
void TIM5_Int_Init(u16 arr, u16 psc);


#endif
