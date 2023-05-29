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

void TIM3_Int_Init(u16 arr,u16 psc);
void TIM3_PWM_Init(u16 arr,u16 psc);
u16 step_angletwopulse(int angle);
u8 step_pulseNum(int pulsenum);


extern u16 arr1;
extern u16 psc1;

typedef struct
{
	
	int curAngle;				 /* 当前角度  */
	int curSpeed;				 /* 当前速度  */
	u16 pulsesNum;  		 /* 脉冲个数  */
	float subdivision;	 /* 细分数    */
	float	stepAngle;		 /* 电机步距角*/
	uint8_t	en;				   /* 使能      */
	uint8_t dir;			   /* 方向		  */
	
	
}Motor;



#endif
