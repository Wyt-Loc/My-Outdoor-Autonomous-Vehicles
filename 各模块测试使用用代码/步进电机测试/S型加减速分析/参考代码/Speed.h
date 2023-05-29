#ifndef __SPEED_H__
#define __SPEED_H__
#include "rtthread.h"
#include "finsh.h"
/*  三分之一加减速法则：
加速路程不超过三分之一总路程，
这样做的目的是给减速留有足够的时间去检测减速点，
进而实现平稳的将速度减到初速度，
如果在处理速度够快的情况下，
可以选择二分之一加减速
*/
//https://item.taobao.com/item.htm?ft=t&id=591410892913
//https://item.taobao.com/item.htm?ft=t&id=591410892913
//https://item.taobao.com/item.htm?ft=t&id=591410892913
//加减速法则，2分之一法则或3分之一法则
#define SPEEDPRINCIPLE              3
//电机最大转速，单位转/min
#define MOTOMAXSPEED                1500    
//电机每转对应的传动距离,单位：mm
#define MOTOROUNDLENGTH             36
//电机最大初速度，初速度超过该值会对电机造成伤害，单位转/min
#define MOTOMAXSTART                200
//驱动器细分数
#define DRIVERSFRACTION             1
//电机细分数
#define MOTOFRACTION                200
//根据初速度，末速度，以及时间，计算加速路程
#define ACCELERATESPACE(V0,Vt,t)    (((V0) + (Vt)) * (t) / 2)
//根据加速路程，初速度，以及时间，计算末速度
#define LASTVELOCITY(S,V0,t)        (2 * (S) / (t) - (V0))
//根据电机转速（转/min），计算电机步速度（step/s）
#define STEPSPEED(RV)               ((RV) * MOTOFRACTION * DRIVERSFRACTION / 60)
//根据长度计算电机所需走的步数，S的单位为mm
#define TOTALSTEP(S)                S * MOTOFRACTION * DRIVERSFRACTION / MOTOROUNDLENGTH 
struct SPEED
{
    int         V0;                 //初速度，单位：step/s
    int         Vt;                 //末速度，单位：step/s
    long long   S;                  //路程，单位：step
    double      t;                  //加速时间，单位：s
    int         time;               //加速次数    单位：次
    int         *SpeedTab;          //加速速度表，速度单位：step/s
    int         SecSpeedPoint;      //减速点 单位：step(在电机运动过程中，如果剩余路程小于等于该值，那么电机开始减速)
};
typedef struct SPEED * Speed_t;


//各参数范围值，可以在此设置参数范围
//初速度大于0小于电机最高起速，末速度大于初速度，小于电机最高转速
#define IS_SPEED(V0,Vt)     (((V0) >= 0) && ((V0) <= STEPSPEED(MOTOMAXSTART))  \
                            && ((Vt) >= (V0)) && ((Vt) <= STEPSPEED(MOTOMAXSPEED)))                            
#define IS_SPACE(S)         ((S) > 0)
#define IS_ADDTIMING(t)     ((t) > 0)            
#define IS_ADDTIME(time)     ((time) >= 32)        //让加速次数必须大于等于32，否则计算表格就没什么意义

#endif
