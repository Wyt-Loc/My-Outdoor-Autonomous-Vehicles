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
void MYDMA_Init();
void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx);



typedef struct
{
	int angle;				   /* Ŀ��Ƕ�  */
	uint32_t pulsesNum;  /* �������  */
	float subdivision;	 /* ϸ����    */
	float	stepAngle;		 /* ��������*/
	uint8_t	en;				   /* ʹ��      */
	uint8_t dir;			   /* ����		  */
	
	
}Motor;



#endif
