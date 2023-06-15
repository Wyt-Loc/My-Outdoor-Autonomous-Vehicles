#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收

extern u8  USART_RX_BUF1[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA1;         		//接收状态标记	
extern u8  USART_RX_BUF2[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA2;         		//接收状态标记	

//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);
void uart2_init(u32 bound);



// 字符集 这块自己规定与PC端的一一对应
// 暂时只有校验

#define LEFTS       "lefts"      // 左转点按
#define RIGHTS      "rights"     // 右转点按

#define LEFTL		    "leftl"		   // 左转长按
#define RIGHTL      "rightl"	   // 右转长按

#define ADVANCESS   "advancess"  // 前进点按
#define BACKS		    "backs" 		 // 后退点按

#define ADVANCESL   "advancesl"  // 前进长按
#define BACKL				"backl"			 // 后退长按

#define STOPSERVOS   "stopservos"  //停止舵机长按动作
#define STOPMOTOR		 "stopmotor"   //停止电机长按动作



u8 usartRecvData(void);
void Control(u8 value);



#endif





