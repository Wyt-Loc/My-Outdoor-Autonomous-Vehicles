#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"


void time3_initer(u16 arr,u16 psc);
void BASIC_TIM_Config(void);
void BaseTimer_Init(void);
void BaseTimer_Config(uint16_t arr, uint16_t psc);
void BaseTimer_Init(void);
void startTimerInterrupt(void);
void stopTimerInterrupt(void);
u8 getTimerCountValue(TIM_TypeDef* TIMx);

#endif
