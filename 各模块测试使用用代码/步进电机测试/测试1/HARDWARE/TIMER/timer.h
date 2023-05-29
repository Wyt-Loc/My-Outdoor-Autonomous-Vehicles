#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/4
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//********************************************************************************

//电机1函数声明
void TIM2_IRQHandler(void);
void TIM2_config(u32 PulseNum);
void TIM1_config(u32 Cycle);
void Pulse_output_1(u32 Cycle,u32 PulseNum);//频率 = 72M / 72 / 目标频率     距离


//电机2函数声明
void TIM4_IRQHandler(void);
void TIM3_config(u32 Cycle);
void TIM4_config(u32 PulseNum);
void Pulse_output_2(u32 Cycle,u32 PulseNum);//频率 = 72M / 72 / 目标频率     距离


void TIM3_Int_Init(u16 arr,u16 psc);


#endif
