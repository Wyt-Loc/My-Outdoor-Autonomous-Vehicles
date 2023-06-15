#include "sys.h"
#include "usart.h"	  
#include "led.h"
#include "Servos.h"
#include "motorT.h"
#include <stdio.h>
#include <string.h>
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif

#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*使用microLib的方法*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF1[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u8 USART_RX_BUF2[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.

//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA1=0;       //接收状态标记	  
u16 USART_RX_STA2=0;       //接收状态标记	   


void uart_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 

}

void USART1_IRQHandler(void)                	//串口1中断服务程序
	{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
		
		if((USART_RX_STA1&0x8000)==0)//接收未完成
			{
			if(USART_RX_STA1&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)USART_RX_STA1=0;//接收错误,重新开始
				else USART_RX_STA1|=0x8000;	//接收完成了 
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART_RX_STA1|=0x4000;
				else
					{
					USART_RX_BUF1[USART_RX_STA1&0X3FFF]=Res ;
					USART_RX_STA1++;
					if(USART_RX_STA1>(USART_REC_LEN-1))USART_RX_STA1=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}   		 
     } 
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
}



















void uart2_init(u32 bound){
	 //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);	//使能USART2时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	//使能GPIOA时钟
	USART_DeInit(USART2);  //复位串口2
	 //USART2_TX   PA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA2
 
	 //USART2_RX	  PA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA3
 
 
	//USART 初始化设置
 
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
 
	 USART_Init(USART2, &USART_InitStructure); //初始化串口
	#if EN_USART1_RX		  //如果使能了接收  
	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
 
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
	#endif
	USART_Cmd(USART2, ENABLE);                    //使能串口 
 
}


void USART2_IRQHandler(void)                	//串口2中断服务程序
{
	u8 Res;
	#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntEnter();
	#endif
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res = USART_ReceiveData(USART2);//(USART1->DR);	//读取接收到的数据
		
		if((USART_RX_STA2&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA2&0x4000)//接收到了0x0d
			{
				if(Res!=0x0a)USART_RX_STA2=0;//接收错误,重新开始
				else USART_RX_STA2|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d)USART_RX_STA2|=0x4000;
				else
				{
					USART_RX_BUF2[USART_RX_STA2&0X3FFF]=Res ;
					USART_RX_STA2++;
					if(USART_RX_STA2>(USART_REC_LEN-1))USART_RX_STA2=0;//接收数据错误,重新开始接收	  
				}
			}
		}
	 }
	#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntExit();  											 
	#endif
}



u8 usartRecvData(void){
	
	u8 len,t;
	
	if(USART_RX_STA2&0x8000)   // 对接收数据进行判断
	{
	
	 len=USART_RX_STA2&0x3fff;//得到此次接收到的数据长度
		printf("%d",len);
	
	for(t=0;t<len;t++)
	{
		USART_SendData(USART1, USART_RX_BUF2[t]);//向串口1发送数据
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
	}
		
	switch(len){
			
		case 5:
			if(!strcmp(LEFTS,(const char*)USART_RX_BUF2)){  //左转点按
				memset(USART_RX_BUF2,0,sizeof(USART_RX_BUF2));
				return  1;
			}
			if(!strcmp(LEFTL,(const char*)USART_RX_BUF2)){  // 左转长按
				memset(USART_RX_BUF2,0,sizeof(USART_RX_BUF2));
				return 2;
			}
			if(!strcmp(BACKS,(const char*)USART_RX_BUF2)){   //后退点按
				memset(USART_RX_BUF2,0,sizeof(USART_RX_BUF2));
				return 3; 
			}
			if(!strcmp(BACKL,(const char*)USART_RX_BUF2)){   //后退长按
				memset(USART_RX_BUF2,0,sizeof(USART_RX_BUF2));
				return 4; 
			}
			break;
		
		case 6:
			if(!strcmp(RIGHTS,(const char*)USART_RX_BUF2)){  //右转点按
				memset(USART_RX_BUF2,0,sizeof(USART_RX_BUF2));
				return  5;
			}
			if(!strcmp(RIGHTL,(const char*)USART_RX_BUF2)){  //右转长按
				memset(USART_RX_BUF2,0,sizeof(USART_RX_BUF2));
				return  6;
			}
			break;
		
		case 9:
			if(!strcmp(ADVANCESS,(const char*)USART_RX_BUF2)){  //前进点按
				memset(USART_RX_BUF2,0,sizeof(USART_RX_BUF2));
				return  7;
			}
			if(!strcmp(ADVANCESL,(const char*)USART_RX_BUF2)){  //前进长按
				memset(USART_RX_BUF2,0,sizeof(USART_RX_BUF2));
				return  8;
			}
			if(!strcmp(STOPMOTOR,(const char*)USART_RX_BUF2)){  //停止电机长按
				memset(USART_RX_BUF2,0,sizeof(USART_RX_BUF2));
				return  10;
			}
			break;
			
		case 10:
				if(!strcmp(STOPSERVOS,(const char*)USART_RX_BUF2)){  //停止舵机长按
				memset(USART_RX_BUF2,0,sizeof(USART_RX_BUF2));
				return  9;
			}
			break;
	}
		USART_RX_STA2=0;
	}
	return 0;
	
}


__IO uint32_t g_set_speed  = 80;//* radtor;    //圈/s    //600 * 5.729578;   /* 最大速度 单位为 0.1rad/sec */   //0.1弧度每秒  1rad = 5.729578
__IO uint32_t g_step_accel = 20;//* radtor;               /* 加速度 单位为 0.1rad/sec^2 */  //0.1弧度每2次方秒
__IO uint32_t g_step_decel = 20;//* radtor;               /* 减速度 单位为 0.1rad/sec^2 */  //0.1弧度每2次方秒
__IO uint16_t g_step_angle = 5;             /* 设置的步数*/   //对应圈数 8细分下


extern __IO uint32_t g_add_pulse_count;     /* 脉冲个数累计*/

extern u8 clickorlongservos;
extern u8 clickorlongmotor;

void Control(u8 value){
		
	switch(value){
		case 1:
			ServosLeftRightClick(1);  //左转点按
			printf("左转点按");
			break;
		case 5:
			ServosLeftRightClick(2);  //右转点按
			printf("右转点按");
			break;
		case 6:
			clickorlongservos = 2;
			printf("右转长按");
			break;
		case 2:
			clickorlongservos = 1;
			printf("左转长按");
			break;
		case 7:
			create_t_ctrl_param(SPR*g_step_angle, g_step_accel, g_step_decel, g_set_speed);
			g_add_pulse_count=0;
			printf("前进点按");
			break;
		case 3:
			create_t_ctrl_param(-SPR*g_step_angle, g_step_accel, g_step_decel, g_set_speed);
			g_add_pulse_count=0;
			printf("后退点按");
			break;
		case 4:
			clickorlongmotor = 1;
			create_t_ctrl_param(-SPR*g_step_angle*3, g_step_accel, g_step_decel, g_set_speed);
			g_add_pulse_count=0;
			printf("后退长按");
			break;
		case 8:
			clickorlongmotor = 1;
			create_t_ctrl_param(SPR*g_step_angle*3, g_step_accel, g_step_decel, g_set_speed);
			g_add_pulse_count=0;
			printf("前进长按");
			break;
		case 9:
			servosStop(&clickorlongservos);
			printf("舵机停止");
			break;
		case 10:
			motorStopLong(&clickorlongmotor);
			printf("电机停止");
			break;
	}
	
}







#endif	

