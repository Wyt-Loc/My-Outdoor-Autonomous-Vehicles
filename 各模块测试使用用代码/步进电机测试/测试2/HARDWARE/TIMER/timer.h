#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/4
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
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
	
	int curAngle;				 /* ��ǰ�Ƕ�  */
	int curSpeed;				 /* ��ǰ�ٶ�  */
	u16 pulsesNum;  		 /* �������  */
	float subdivision;	 /* ϸ����    */
	float	stepAngle;		 /* ��������*/
	uint8_t	en;				   /* ʹ��      */
	uint8_t dir;			   /* ����		  */
	
	
}Motor;



#endif
