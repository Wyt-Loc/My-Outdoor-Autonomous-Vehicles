#ifndef __SPI_H
#define __SPI_H
#include "sys.h"


//DIR
#define LEFT1 PBout(3) = 1,  Pulse_output_1(625,3200) // 2秒转1圈 整个转向 0.5秒完成
#define LEFT2 PBout(4) = 1,  Pulse_output_2(625,3200) // 2秒转1圈 整个转向 0.5秒完成

#define RIGHT1 PBout(3) = 0, Pulse_output_1(625,3200) // 2秒转1圈 整个转向 0.5秒完成
#define RIGHT2 PBout(4) = 0, Pulse_output_2(625,3200) // 2秒转1圈 整个转向 0.5秒完成

//EN
#define EN1 	PBout(0) = 1
#define EN2   PBout(1) = 1

#define STOP1 PBout(0) = 0
#define STOP2 PBout(1) = 0

#define  RING1  Pulse_output_1(625,3200) // 2秒转1圈 整个转向 0.5秒完成
#define  RING2  Pulse_output_2(625,3200) // 2秒转1圈 整个转向 0.5秒完成


//指令集合  自定义  从机接收
#define Reply_Signal  254 //答复信号
#define LEFT 253  //左转
#define RIGHT 252 //右转
#define RING 249  //原地旋转




			  	    													  
void SPI1_Init(void);			 //初始化SPI口
uint16_t SPI1_ReadWriteByte(uint16_t TxData);//SPI总线读写一个字节
uint16_t Recv_Data_SPI(void);
void Control_Motor(uint16_t id);


void SPI2_Init(void);
uint16_t SPI2_ReadWriteByte(uint16_t TxData);//SPI总线读写一个字节
	
		 
#endif



