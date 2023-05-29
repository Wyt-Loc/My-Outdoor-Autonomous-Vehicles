#ifndef __LED_H
#define __LED_H	 
#include "sys.h"



void myStepMotorT(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed);


//�������T�ͼӼ����㷨�������
#define TIM_FREQ            168000000U                      /* ��ʱ����Ƶ */
#define MAX_STEP_ANGLE      0.225                           /* ��С����(1.8/MICRO_STEP) */
#define PAI                 3.1415926                       /* Բ����*/
#define FSPR                200                             /* ���������Ȧ���� */
#define MICRO_STEP          8                               /* �������������ϸ���� */
#define T1_FREQ             (TIM_FREQ/84)                   /* Ƶ��ftֵ */
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

//����������   �ڴ�����ֻ������ ���Ϊ ��1  ��2
#define  	STEPPER_MOTOR_1  1
#define 	STEPPER_MOTOR_2	 2

//������������������
#define  STEPPER_DIR1_GPIO_PIN		GPIO_Pin_6 			/*���1----��*/
#define  STEPPER_DIR1_GPIO_PORT		GPIOA

//#define  STEPPER_DIR1_GPIO_PIN		GPIO_Pin_6 
//#define  STEPPER_DIR1_GPIO_PORT		GPIOA

//��������ѻ����Ŷ���
#define  STEPPER_EN1_GPIO_PIN				GPIO_Pin_1
#define  STEPPER_EN1_GPIO_PORT			GPIOA

















#endif
