/* 					CANBUS 控制电机 				*/

#include "canMotor.h"
#include "MotorT.h"
#include "can.h"
#include "stdio.h"
#include "timer.h"


/***************************************************************
【函数名】 u8* receData(void)
【功  能】 得到CAN总线数据
【参数值】 无
【返回值】 数据
****************************************************************/
static u8 ReceBuf[8]; // 本次接收的数据
static u8 old = 0;
u8 receData(void){

	u8 i = 0;
	u8 sum = 0;
	static u8 isRece = 0;

		isRece = Can_Receive_Msg(ReceBuf); //接收主节点数据
		if(isRece)
		{
			printf("接收到了");
			for(i=0;i<isRece;i++)
			{
						printf(" %d ",ReceBuf[i]);
			}
			for(i = 0; i < isRece / 2; i++)
			{
				sum += ReceBuf[i] & 0xE0;  // 解密
			}
			old = ReceBuf[0];
			ReceBuf[0] = sum;
			while( isRece && !Can_Send_Msg(ReceBuf,8))
			{
				isRece = 0;
				printf("\r\n");
			}
			return 1;
		}
		return 0;
}

short  duty = 563;
u8 js1 = 100;
u8 js2 = 100;

u8 servo(u8 *arr){

	u8  angle  = 0;
	angle = arr[1];  // 得到舵机的角度信息
	printf("angle = %d\r\n", angle);
	
	// 舵机 stop
	if (old == 115 && arr[1] == 116)
	{
		printf("舵机stop");
		TIM_SetCompare2(TIM3,duty); // 舵机的stop 就是 让舵机直接执行 到当前的位置不动就好了。
	}
	// 舵机执行
	else
	{
		if(angle > 0 && angle <= 20)
		{
			//中
			if(angle == 100)
			{
				TIM_SetCompare2(TIM3,ServoMedian); //占空比=i/(199+1)  563
				printf("duty = %d",ServoMedian);
			}
			//左偏
			if (angle < 10)
			{
				// 暂时假设是左
				duty = (ServoMedian - ServoAngleMIN) / 10.0f * angle + ServoAngleMIN;
				if(duty < 468) duty = 468;
				if(duty > 647) duty = 647;
				TIM_SetCompare2(TIM3,duty);//占空比=i/(199+1)
				printf("duty = %d",duty);
			}
			//右偏
			if (angle > 10)
			{
				duty = (ServoAngleMAX - ServoMedian) / 10.0f * angle + ServoAngleMIN;
				if(duty < 468) duty = 468;
				if(duty > 647) duty = 647;
				TIM_SetCompare2(TIM3,duty);//占空比=i/(199+1)
				printf("duty = %d",duty);
			}
		}
		else
		{
			printf("angle 范围错误");
			return 0;
		}
	}
	return 1;
}


void execute(void){
	
	u8 a = 0;
	//CAN消息接收
	a = receData();
	// 舵机执行
	if (a == 1)
	servo(ReceBuf);
	
}



