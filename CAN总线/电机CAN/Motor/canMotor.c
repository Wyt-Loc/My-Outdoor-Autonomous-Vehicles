/* 					CANBUS 控制电机 				*/

#include "canMotor.h"
#include "MotorT.h"
#include "can.h"
#include "stdio.h"


/***************************************************************
【函数名】 u8* receData(void)
【功  能】 得到CAN总线数据
【参数值】 无
【返回值】 数据
****************************************************************/
	static u8 ReceBuf[8]; // 本次接收的数据
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
			for(i = 0; i < isRece / 2; i++){
				sum += ReceBuf[i] & 0xE0;  // 解密
			}
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


void test (){
	
	if(receData() == 1){
		printf("shuju = %s \r\n",ReceBuf);
	}
}

/***************************************************************
【函数名】 void motorExecute(u8* arr)
【功  能】 
【参数值】
【返回值】
****************************************************************/
//  create_t_ctrl_param(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)
void motorExecute(u8* arr){
	
	u8 dir = 0;
	float dis = 0.0f;
	float speed = 0.0f;
	int32_t step = 0;
	
	dis = arr[1]+arr[2]*0.1;
	
	speed = arr[3] + arr[4] * 0.1; // 速度 m/s
	
	//转换为 RPM   假设轮的周长为0.5m
	speed = speed / 0.5f;
	
	// 转换为 弧度    1 Rad/sec = 9.55 RPM
	speed = speed / 9.55f;
	
	step = dis / 0.5f;
	
	step = step / 9.55f;
	
	if (dir == 1){
			dis = dis;
	}else if (dir == 0){
		dis = -dis;
	}
	
	//步进电机转动 步数， 加速度，减速度，最大速度
	create_t_ctrl_param( step,  30,  30,  speed);
	printf("step = %d, speed = %f\r\n", step, speed);
	
}




