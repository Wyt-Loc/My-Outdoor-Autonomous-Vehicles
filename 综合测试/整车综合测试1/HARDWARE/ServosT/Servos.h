#ifndef __Servos_H
#define __Servos_H

#include "sys.h"


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

#define ServoAngleMAX 660   	//���������ֵ   ---����
#define ServoAngleMIN 410			//��������Сֵ
#define ServoMedian  563			//�����ֵ



#endif




void Servos_PWM_Init(u16 arr,u16 psc);
void ServosLeftRightClick(u8 leftorright);
void ServosLeftRightLong(void);
void servosStop(u8 *clickorlong);

#endif

