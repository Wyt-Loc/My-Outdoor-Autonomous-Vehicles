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
u8 old = 0;
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

u8 servo(u8 *arr){
	
	u8  angle  = 0;
	
	angle = arr[1];  // 得到舵机的角度信息
	
	if(angle > 0 && angle <= 200){
		
		//中
		if(angle == 100){
			
			TIM_SetCompare1(TIM4,ServoMedian);
			printf("duty = %d",ServoMedian);
			
		}
		//左偏
		if (angle < 100){ // 暂时假设是左
			
			//duty = (ServoMedian - ServoAngleMIN) / 100.0f * angle + ServoAngleMIN;
			duty = duty - 5;
			TIM_SetCompare1(TIM4,duty);
			printf("duty = %d",duty);
		
		}
		//右偏
		if (angle > 100){
			duty = duty + 5;
			//duty = (ServoAngleMAX - ServoMedian) / 100.0f * angle + ServoMedian;
			TIM_SetCompare1(TIM4,duty);
			printf("duty = %d",duty);
			
		}
		
	}
	else{
		printf("angle 范围错误");
		return 0;
		
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



