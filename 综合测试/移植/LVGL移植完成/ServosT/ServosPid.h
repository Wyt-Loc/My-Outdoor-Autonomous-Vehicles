#ifndef __ServosPID_H
#define __ServosPID_H


/***************************************************************
【函数名】typedef struct PID
【功  能】舵机PID参数声明
【参数值】无
【返回值】无
****************************************************************/
typedef struct PID
{
    float                kp;         //P
    float                ki;         //I
    float                kd;         //D
    float                imax;       //积分限幅

    float                out_p;  //KP输出
    float                out_i;  //KI输出
    float                out_d;  //KD输出
    float                out;    //pid输出

    float                integrator; //< 积分值
    float                last_error; //< 上次误差
    float                last_derivative;//< 上次误差与上上次误差之差
    unsigned long        last_t;     //< 上次时间

}pid_param_t;


// 舵机控制相关函数声明
void PidInit_wz(pid_param_t * pid);
float PidLocCtrl_wz(pid_param_t * pid, float error);



#endif

