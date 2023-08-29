#include "motorT.h"
#include "math.h"
#include "stdio.h"
#include "dma.h"


Motor m;

u16 T;
u8 i = 0;
extern u16 send_buf[1];

int num  = 0;


// 角度转脉冲数
u16 step_angletwopulse(int angle, float V){
	
	int pluseNum = 0;
	
	m.stepAngle = 1.80f;
	m.subdivision = 16.0f;
	if(angle >= 0) m.dir = 1;
	else m.dir = 0;
	
	m.Speed = V;  
	
	pluseNum = angle  / 0.225f;  
	
	if(pluseNum < 0) pluseNum = -pluseNum;
	
	if(pluseNum == 0){ //如果脉冲数为0 则关闭PWM输出
			
		TIM_Cmd(TIM3, DISABLE);  //关闭TIM3
	
	}
	
	else{
	
		TIM_Cmd(TIM3, ENABLE);  //使能TIM3

	}
	printf("pluseNum = %d",pluseNum);
	return pluseNum;
	
}


#define gx   625.0f


//设置速度
float Set_V(Motor m){
	
	float psc = 1;
	psc = gx / m.Speed / 2; // 单位转为 转/秒
	send_buf[0] = (u16)psc;
	printf("psc = %d",(u16)psc);
	
	DMA_Enable(DMA1_Channel6);  //psc = 6250
	
	return psc;
	
}


//  step_angletwopulse 
// 控制定时器输出多少个脉冲数, 按设置的速度进行
// 执行结束返回 1
u8 step_pulseNum(int pulsenum){

	u8 flag = 0;	//是否执行完 标志位

	Set_V(m);

	if(pulsenum == 0)
		TIM_Cmd(TIM3, DISABLE);  //使能TIM3
	else{  //如果脉冲数不为0，则输出脉冲
		m.pulsesNum = 0;
		flag = 0;
		while(flag == 0){
			if(m.pulsesNum == pulsenum){ //如果累计脉冲数等于所需脉冲数 则停止输出
				TIM_Cmd(TIM3,DISABLE);
				return 1;
			}
		}
	}
	return 0;
}



/*
void TIM3_IRQHandler(void)
{
	    if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
    {
       
    }
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清楚中断标志位		
	
}*/















