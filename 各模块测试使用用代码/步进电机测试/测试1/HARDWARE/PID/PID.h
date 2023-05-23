#ifndef __PID_H
#define __PID_H	 
#include "sys.h"


typedef struct
{
    //PID运算模式
    uint8_t mode;
    //PID 三个基本参数
    float Kp;
    float Ki;
    float Kd;

    float max_out;  //PID最大输出
    float max_iout; //PID最大积分输出

    float set;	  //PID目标值
    float fdb;	  //PID当前值

    float out;		//三项叠加输出
    float Pout;		//比例项输出
    float Iout;		//积分项输出
    float Dout;		//微分项输出
    //微分项最近三个值 0最新 1上一次 2上上次
    float Dbuf[3];  
    //误差项最近三个值 0最新 1上一次 2上上次
    float error[3];  

} pid_type_def;





enum PID_MODE
{
    PID_POSITION = 0,
    PID_DELTA
};



void PID_init(pid_type_def *pid, uint8_t mode, const float PID[3], float max_out, float max_iout);
float PID_calc(pid_type_def *pid, float ref, float set);


#endif


