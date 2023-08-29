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
static volatile u8 old = 0;
u8 receData(void){

	u8 i = 0;
	u8 sum = 0;
	static u8 isRece = 0;

		isRece = Can_Receive_Msg(ReceBuf); //接收主节点数据
		if(isRece) //接收到了CAN消息 且 当前执行状态为空闲
		{
			printf("接收到了");
			for(i=0;i<isRece;i++)
			{
				printf(" Recedata=%d ",ReceBuf[i]);
			}
			for(i = 0; i < isRece / 2; i++)
			{
				sum += ReceBuf[i] & 0xE0;  // 解密
			}
			old = ReceBuf[0]; // 将数据保存
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

void test(){

	if(receData() == 1)
	{
		printf("shuju = %s \r\n",ReceBuf);
	}

}

/***************************************************************
【函数名】 void motorExecute(u8* arr)
【功  能】 电机执行
【参数值】 电机参数
【返回值】 无
****************************************************************/
//  create_t_ctrl_param(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed)

float R = 0.25; //直径
float cir = 1.57075f; //周长

extern speedRampData g_srd;
void motorExecute(u8* arr){
	
	short dir = 0;
	float dis = 0.0f;
	float speed = 0.0f;
	float num = 0.0f;
	int step = 0;
	
	dir = old;
	
	if(dir == 115 && arr[1] == 116){
		 printf("电机 stop\r\n");
		 g_srd.run_state = STOP;
	 }
	 
	else{
		for (dir =0; dir <8;dir++)
		printf("arr = %d \r\n", arr[dir]);
	
		dis = arr[1] + arr[2]*0.1 + arr[3]*0.01; // m
		printf("dis = %f", dis);
	
		speed = arr[4] + arr[5] * 0.1 + arr[6] *0.01; // 速度 m/s
		printf("speed = %f", speed); // 最大速度
	
		num = dis / cir; //要走的距离所对应的圈数
	
		step = 1600 * num; // 步数 = 
	
		//转换为 RPM   假设轮的周长为0.5m
		//speed = speed / 0.5f;
	
		// 转换为 弧度    1 Rad/sec = 9.55 RPM
		//speed = speed / 9.55f; // 圈/s
	
		//step = dis / 0.5f;
		
		//step = step / 9.55f;
	
		if (old == 1){
			printf("前进");
				step = -step;
		}else if (old == 0){
			printf("后退");
			step = step;
		}
	
		//步进电机转动 步数， 加速度，减速度，最大速度
		create_t_ctrl_param( step*3,  10,  10,  50);
		printf("step = %d, speed = %f\r\n", step, speed );
		
	}
	
}


void motor(void)
{
	u8 a = 0;
	//接收数据
	a = receData();
	if (a == 1){
	//电机执行
		motorExecute(ReceBuf);
	}
}
