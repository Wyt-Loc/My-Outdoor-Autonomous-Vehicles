#ifndef __MORTORH_H
#define __MORTORH_H 

#include "sys.h"


u16 step_angletwopulse(int angle, float V);
u8 step_pulseNum(int pulsenum);




typedef struct
{
	int curAngle;				 /* ��ǰ�Ƕ�  */
	float Speed;				 /* �ٶ�      */
	u16 pulsesNum;  		 /* �������  */
	float subdivision;	 /* ϸ����    */
	float	stepAngle;		 /* ��������*/
	uint8_t	en;				   /* ʹ��      */
	uint8_t dir;			   /* ����		  */
}Motor;




/******************************************************************************************/
#define TIM_FREQ            72000000U                      /* ��ʱ����Ƶ */
#define MAX_STEP_ANGLE      0.225                           /* ��С����(1.8/MICRO_STEP) */
#define PAI                 3.1415926                       /* Բ����*/
#define FSPR                200                             /* ���������Ȧ���� */
#define MICRO_STEP          8                               /* �������������ϸ���� */
#define T1_FREQ             (TIM_FREQ/72)                   /* Ƶ��ftֵ */
#define SPR                 (FSPR*MICRO_STEP)               /* ��תһȦ��Ҫ�������� */

/* ��ѧ���� */

#define ALPHA               ((float)(2*PAI/SPR))            /* �� = 2*pi/spr */
#define A_T_x10             ((float)(10*ALPHA*T1_FREQ))
#define T1_FREQ_148         ((float)((T1_FREQ*0.69)/10))    /* 0.69Ϊ�������ֵ */
#define A_SQ                ((float)(2*100000*ALPHA))
#define A_x200              ((float)(200*ALPHA))            /* 2*10*10*a/10 */

typedef struct
{
    __IO uint8_t  run_state;                                /* �����ת״̬ */
    __IO uint8_t  dir;                                      /* �����ת���� */
    __IO int32_t  step_delay;                               /* �¸��������ڣ�ʱ������������ʱΪ���ٶ� */
    __IO uint32_t decel_start;                              /* ��ʼ����λ�� */
    __IO int32_t  decel_val;                                /* ���ٽ׶β��� */
    __IO int32_t  min_delay;                                /* �ٶ���죬����ֵ��С��ֵ(����ٶȣ������ٶ��ٶ�) */
    __IO int32_t  accel_count;                              /* �Ӽ��ٽ׶μ���ֵ */
} speedRampData;

enum STA
{
    STOP = 0,                                               /* �Ӽ�������״̬��ֹͣ*/
    ACCEL,                                                  /* �Ӽ�������״̬�����ٽ׶�*/
    DECEL,                                                  /* �Ӽ�������״̬�����ٽ׶�*/
    RUN                                                     /* �Ӽ�������״̬�����ٽ׶�*/
};

enum DIR
{
 CCW = 0,                                                   /* ��ʱ�� */ 
 CW                                                         /* ˳ʱ�� */
};

enum EN
{
 EN_ON = 0,                                                 /* ʧ���ѻ����� */
 EN_OFF                                                     /* ʹ���ѻ����� ʹ�ܺ���ֹͣ��ת */
};


void create_t_ctrl_param(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed); /* ���μӼ��ٿ��ƺ��� */
void motorStopLong(u8 *clickorlongmotor);






#endif
