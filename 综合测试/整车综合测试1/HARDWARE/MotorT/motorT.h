#ifndef __MORTORH_H
#define __MORTORH_H 

#include "sys.h"


u16 step_angletwopulse(int angle, float V);
u8 step_pulseNum(int pulsenum);




typedef struct
{
	int curAngle;				 /* 当前角度  */
	float Speed;				 /* 速度      */
	u16 pulsesNum;  		 /* 脉冲个数  */
	float subdivision;	 /* 细分数    */
	float	stepAngle;		 /* 电机步距角*/
	uint8_t	en;				   /* 使能      */
	uint8_t dir;			   /* 方向		  */
}Motor;




/******************************************************************************************/
#define TIM_FREQ            72000000U                      /* 定时器主频 */
#define MAX_STEP_ANGLE      0.225                           /* 最小步距(1.8/MICRO_STEP) */
#define PAI                 3.1415926                       /* 圆周率*/
#define FSPR                200                             /* 步进电机单圈步数 */
#define MICRO_STEP          8                               /* 步进电机驱动器细分数 */
#define T1_FREQ             (TIM_FREQ/72)                   /* 频率ft值 */
#define SPR                 (FSPR*MICRO_STEP)               /* 旋转一圈需要的脉冲数 */

/* 数学常数 */

#define ALPHA               ((float)(2*PAI/SPR))            /* α = 2*pi/spr */
#define A_T_x10             ((float)(10*ALPHA*T1_FREQ))
#define T1_FREQ_148         ((float)((T1_FREQ*0.69)/10))    /* 0.69为误差修正值 */
#define A_SQ                ((float)(2*100000*ALPHA))
#define A_x200              ((float)(200*ALPHA))            /* 2*10*10*a/10 */

typedef struct
{
    __IO uint8_t  run_state;                                /* 电机旋转状态 */
    __IO uint8_t  dir;                                      /* 电机旋转方向 */
    __IO int32_t  step_delay;                               /* 下个脉冲周期（时间间隔），启动时为加速度 */
    __IO uint32_t decel_start;                              /* 开始减速位置 */
    __IO int32_t  decel_val;                                /* 减速阶段步数 */
    __IO int32_t  min_delay;                                /* 速度最快，计数值最小的值(最大速度，即匀速段速度) */
    __IO int32_t  accel_count;                              /* 加减速阶段计数值 */
} speedRampData;

enum STA
{
    STOP = 0,                                               /* 加减速曲线状态：停止*/
    ACCEL,                                                  /* 加减速曲线状态：加速阶段*/
    DECEL,                                                  /* 加减速曲线状态：减速阶段*/
    RUN                                                     /* 加减速曲线状态：匀速阶段*/
};

enum DIR
{
 CCW = 0,                                                   /* 逆时针 */ 
 CW                                                         /* 顺时针 */
};

enum EN
{
 EN_ON = 0,                                                 /* 失能脱机引脚 */
 EN_OFF                                                     /* 使能脱机引脚 使能后电机停止旋转 */
};


void create_t_ctrl_param(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed); /* 梯形加减速控制函数 */
void motorStopLong(u8 *clickorlongmotor);






#endif
