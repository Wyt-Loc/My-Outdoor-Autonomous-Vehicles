#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"


#define BASIC_TIM6 // 如果使用 TIM7，注释掉这个宏即可

#ifdef BASIC_TIM6 // 使用基本定时器 TIM6
#define BASIC_TIM TIM6
#define BASIC_TIM_APBxClock_FUN RCC_APB1PeriphClockCmd
#define BASIC_TIM_CLK RCC_APB1Periph_TIM6
#define BASIC_TIM_IRQ TIM6_IRQn
#define BASIC_TIM_IRQHandler TIM6_IRQHandler

#else // 使用基本定时器 TIM7
#define BASIC_TIM TIM7
#define BASIC_TIM_APBxClock_FUN RCC_APB1PeriphClockCmd
#define BASIC_TIM_CLK RCC_APB1Periph_TIM7
#define BASIC_TIM_IRQ TIM7_IRQn
#define BASIC_TIM_IRQHandler TIM7_IRQHandler

#endif



void time3_initer(u16 arr,u16 psc);
void BASIC_TIM_Config(void);


#endif
