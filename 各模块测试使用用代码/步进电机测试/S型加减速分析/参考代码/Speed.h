#ifndef __SPEED_H__
#define __SPEED_H__
#include "rtthread.h"
#include "finsh.h"
/*  ����֮һ�Ӽ��ٷ���
����·�̲���������֮һ��·�̣�
��������Ŀ���Ǹ����������㹻��ʱ��ȥ�����ٵ㣬
����ʵ��ƽ�ȵĽ��ٶȼ������ٶȣ�
����ڴ����ٶȹ��������£�
����ѡ�����֮һ�Ӽ���
*/
//https://item.taobao.com/item.htm?ft=t&id=591410892913
//https://item.taobao.com/item.htm?ft=t&id=591410892913
//https://item.taobao.com/item.htm?ft=t&id=591410892913
//�Ӽ��ٷ���2��֮һ�����3��֮һ����
#define SPEEDPRINCIPLE              3
//������ת�٣���λת/min
#define MOTOMAXSPEED                1500    
//���ÿת��Ӧ�Ĵ�������,��λ��mm
#define MOTOROUNDLENGTH             36
//��������ٶȣ����ٶȳ�����ֵ��Ե������˺�����λת/min
#define MOTOMAXSTART                200
//������ϸ����
#define DRIVERSFRACTION             1
//���ϸ����
#define MOTOFRACTION                200
//���ݳ��ٶȣ�ĩ�ٶȣ��Լ�ʱ�䣬�������·��
#define ACCELERATESPACE(V0,Vt,t)    (((V0) + (Vt)) * (t) / 2)
//���ݼ���·�̣����ٶȣ��Լ�ʱ�䣬����ĩ�ٶ�
#define LASTVELOCITY(S,V0,t)        (2 * (S) / (t) - (V0))
//���ݵ��ת�٣�ת/min�������������ٶȣ�step/s��
#define STEPSPEED(RV)               ((RV) * MOTOFRACTION * DRIVERSFRACTION / 60)
//���ݳ��ȼ����������ߵĲ�����S�ĵ�λΪmm
#define TOTALSTEP(S)                S * MOTOFRACTION * DRIVERSFRACTION / MOTOROUNDLENGTH 
struct SPEED
{
    int         V0;                 //���ٶȣ���λ��step/s
    int         Vt;                 //ĩ�ٶȣ���λ��step/s
    long long   S;                  //·�̣���λ��step
    double      t;                  //����ʱ�䣬��λ��s
    int         time;               //���ٴ���    ��λ����
    int         *SpeedTab;          //�����ٶȱ��ٶȵ�λ��step/s
    int         SecSpeedPoint;      //���ٵ� ��λ��step(�ڵ���˶������У����ʣ��·��С�ڵ��ڸ�ֵ����ô�����ʼ����)
};
typedef struct SPEED * Speed_t;


//��������Χֵ�������ڴ����ò�����Χ
//���ٶȴ���0С�ڵ��������٣�ĩ�ٶȴ��ڳ��ٶȣ�С�ڵ�����ת��
#define IS_SPEED(V0,Vt)     (((V0) >= 0) && ((V0) <= STEPSPEED(MOTOMAXSTART))  \
                            && ((Vt) >= (V0)) && ((Vt) <= STEPSPEED(MOTOMAXSPEED)))                            
#define IS_SPACE(S)         ((S) > 0)
#define IS_ADDTIMING(t)     ((t) > 0)            
#define IS_ADDTIME(time)     ((time) >= 32)        //�ü��ٴ���������ڵ���32������������ûʲô����

#endif
