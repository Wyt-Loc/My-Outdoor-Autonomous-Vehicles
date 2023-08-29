#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#include "stm32f10x.h"
 
 
#define START_TIME  time=0;RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);TIM_Cmd(TIM2, ENABLE)
#define STOP_TIME  TIM_Cmd(TIM2, DISABLE);RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE)

void TIM3_NVIC_Configuration(void);
//void TIM3_Configuration(void);
void TIM3_PWM_Init(u16 arr,u16 psc);


/*一下所有的测试基于舵机灵敏度和转动角度最大的情况*/

//#define ARR_2000 2000							//选择ARR的值
#define ARR_5000 5000


#ifdef ARR_2000

#define PSC 720

#define ServoAngleMAXLeft 250   	//舵机打角最大值   ---左舵机
#define ServoAngleMINLeft 50			//舵机打角最小值
#define ServoMedianLeft  100			//舵机中值

#define ServoAngleMAXRight 250   	//舵机打角最大值   ---右舵机
#define ServoAngleMINRight 50			//舵机打角最小值
#define ServoMedianRight  100			//舵机中值

#elif ARR_5000

#define PSC 288

#define ServoAngleMAX 647   	//舵机打角最大值   ---左舵机
#define ServoAngleMIN 468			//舵机打角最小值
#define ServoMedian  535			//舵机中值



#endif


void Servos_PWM_Init(u16 arr,u16 psc);
void ServosLeftRightClick(u8 leftorright);
void ServosLeftRightLong(void);
void servosStop(u8 *clickorlong);
void TIM4_PWM_Init(u16 arr,u16 psc);


#endif
