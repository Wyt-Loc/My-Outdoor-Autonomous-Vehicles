#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#include "stm32f10x.h"
 
 
#define START_TIME  time=0;RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);TIM_Cmd(TIM2, ENABLE)
#define STOP_TIME  TIM_Cmd(TIM2, DISABLE);RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE)

void TIM3_NVIC_Configuration(void);
//void TIM3_Configuration(void);
void TIM3_PWM_Init(u16 arr,u16 psc);


/*һ�����еĲ��Ի��ڶ�������Ⱥ�ת���Ƕ��������*/

//#define ARR_2000 2000							//ѡ��ARR��ֵ
#define ARR_5000 5000


#ifdef ARR_2000

#define PSC 720

#define ServoAngleMAXLeft 250   	//���������ֵ   ---����
#define ServoAngleMINLeft 50			//��������Сֵ
#define ServoMedianLeft  100			//�����ֵ

#define ServoAngleMAXRight 250   	//���������ֵ   ---�Ҷ��
#define ServoAngleMINRight 50			//��������Сֵ
#define ServoMedianRight  100			//�����ֵ

#elif ARR_5000

#define PSC 288

#define ServoAngleMAX 647   	//���������ֵ   ---����
#define ServoAngleMIN 468			//��������Сֵ
#define ServoMedian  535			//�����ֵ



#endif


void Servos_PWM_Init(u16 arr,u16 psc);
void ServosLeftRightClick(u8 leftorright);
void ServosLeftRightLong(void);
void servosStop(u8 *clickorlong);
void TIM4_PWM_Init(u16 arr,u16 psc);


#endif
