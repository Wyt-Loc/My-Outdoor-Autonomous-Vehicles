#include "ServosPid.h"


/***************************************************************
【函数名】void PidInit_wz(pid_param_t * pid)
【功  能】舵机位置式PID控制
【参数值】pid_param_t * pid     舵机控制结构体
【返回值】无
****************************************************************/
void PidInit_wz(pid_param_t * pid)
{
    pid->kp        = 0.08f;  //比例调节
    pid->ki        = 0;     
    pid->kd        = 0.08f;     //越大反应越快
    pid->imax      = 0;     
    pid->out_p     = 0;			
    pid->out_i     = 0;  		
    pid->out_d     = 0;
    pid->out       = 0;
    pid->integrator= 0;
    pid->last_error= 0;
    pid->last_derivative = 0;
    pid->last_t    = 0;
}


/***************************************************************
【函数名】float PidLocCtrl_wz(pid_param_t * pid, float error)
【功  能】舵机控制
【参数值】pid_param_t * pid  舵机控制结构体
					float error				 当前误差  也就是 测量到的ADC的误差
【返回值】舵机控制变化参数   也就是 相对于舵机中值 应该打多大角
****************************************************************/
float PidLocCtrl_wz(pid_param_t * pid, float error)
{
	
	pid->out_p = pid->kp * error; 			// Kp * 误差 + KD * 本次误差 与 上次的差
	pid->out_d = pid->kd * (error - pid->last_error);
	pid->last_error = error;

	pid->out = pid->out_p + pid->out_d;

	return pid->out;
	
}



