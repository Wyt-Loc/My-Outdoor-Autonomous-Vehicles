#include "Speed.h"

//https://item.taobao.com/item.htm?ft=t&id=591410892913

//函数名称： CalculateSpeedTab
//函数功能：计算加减速表
//参数参数：Speed 速度结构体
//             V0;                //初速度，单位：step/s
//             Vt;                //末速度，单位：step/s
//             S;                 //路程，单位：step
//             t;                 //加速时间，单位：s
//             time;              //加速次数    单位：次
//             *SpeedTab;         //加速速度表，速度单位：step/s
//             SecSpeedPoint;     //减速点 单位：step
//返回输出：速度结构体
//https://item.taobao.com/item.htm?ft=t&id=591410892913

static Speed_t CalculateSpeedTab(Speed_t Speed)
{
    int i;                        
    double aa;                    //加加速
    int DeltaV;                   //速度变化量
    int tempVt;
    //如果速度记录表的指针为空，说明没有分配内存
    RT_ASSERT(Speed->SpeedTab != RT_NULL);
    //各参数是否超范围
    RT_ASSERT(IS_SPEED(Speed->V0, Speed->Vt));
    RT_ASSERT(IS_SPACE(Speed->S));
    RT_ASSERT(IS_ADDTIMING(Speed->t));
    RT_ASSERT(IS_ADDTIME(Speed->time));
    //根据加速法则计算末速度
    tempVt = LASTVELOCITY(Speed->S / SPEEDPRINCIPLE, Speed->V0, Speed->t);
    Speed->Vt = (tempVt > Speed->Vt) ? Speed->Vt : tempVt;                                                   
    Speed->SecSpeedPoint = ACCELERATESPACE(Speed->V0, Speed->Vt, Speed->t);        //计算减速点位置，剩余路程为该值时开始减速
    aa = (double)((Speed->Vt - Speed->V0) / 2)      //加速度变化拐点时的速度值                           a|   /|\             
        * 2                                         //知道直角三角形面积，逆推三角型高度时将面积*2        |  / | \面积为DeltaV
        / (Speed->time / 2)                         //除以底边                                            | /  |  \            
        / (Speed->time / 2);                        //再除以底边，得到斜率，即加加速                      |/___|___\__time   
    //开始速度计算
    for(i = 0; i < ((Speed->time / 2) + 1); i++)
    {
        DeltaV = (aa * i * i) / 2;                                     //V = V0 + a * t / 2;  a = aa * t;
        *(Speed->SpeedTab + i) = Speed->V0 + DeltaV;                   //当前点的速度
        *(Speed->SpeedTab + Speed->time - i) = Speed->Vt - DeltaV;     //对称点的速度
    }
    return Speed;
}
//函数名称：TestSpeed
//函数功能：测试速度表计算函数
//函数参数：V0      初速度，单位：转/min
//          Vt      末速度，单位：转/min
//          S       路程，单位：mm
//          time    加速次数
//函数返回：无
//https://item.taobao.com/item.htm?ft=t&id=591410892913
void TestSpeed(int V0, int Vt, int S, int time)
{
    int i;
    Speed_t Speed = (Speed_t)rt_malloc(sizeof(struct SPEED));
    Speed->V0 = STEPSPEED(V0);    //起速
    Speed->Vt = STEPSPEED(Vt);    //末速
    Speed->S = TOTALSTEP(S);      //路程
    Speed->t = 0.2;               //加速时间设为0.2秒
    Speed->time = time;           //加速次数
    Speed->SpeedTab = (int *)rt_malloc(sizeof(int) * (Speed->time + 1)); //根据加速次数申请表格内存，加1，以防用的时候超界
    CalculateSpeedTab(Speed);       //开始计算
    for(i = 0; i < Speed->time; i++)
    {
        rt_kprintf("SpeedTab[%d] = %d, Acceleration = %d\n"
                  , i
                  , *(Speed->SpeedTab + i)
                  , *(Speed->SpeedTab + i + 1) - *(Speed->SpeedTab + i));
    }
    rt_kprintf("SpeedTab[%d] = %d\n", i, *(Speed->SpeedTab + i));
    rt_free(Speed->SpeedTab);              
    rt_free(Speed);                    
}

FINSH_FUNCTION_EXPORT(TestSpeed, Test Speed table calculate);

