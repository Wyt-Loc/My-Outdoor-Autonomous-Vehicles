#include "timer.h"
#include "led.h"
#include "usart.h"

void TIM3_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	 TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	/* 使能定时器3时钟 */
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  /* 使能GPIO外设 */   
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; /* TIM_CH1*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  /* 复用推挽输出 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);/* 初始化GPIO */
 
   /* 初始化TIM3 */
	TIM_TimeBaseStructure.TIM_Period = arr; /* 设置在下一个更新事件装入活动的自动重装载寄存器周期的值 */
	TIM_TimeBaseStructure.TIM_Prescaler =psc; /* 设置用来作为TIMx时钟频率除数的预分频值 */
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; /* 设置时钟分割:TDTS = Tck_tim */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  /*TIM向上计数模式 */
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); /* 根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位 */
	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; /* 选择定时器模式:TIM脉冲宽度调制模式1 */
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; /* 比较输出使能 */
	//TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  /* 使能TIM3在CCR1上的预装载寄存器*/
	TIM_OCInitStructure.TIM_Pulse= 100;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; /* 输出极性:TIM输出比较极性高 */
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  /* 根据T指定的参数初始化外设TIM3 OC1 */
	//TIM_DMACmd(TIM3, TIM_DMA_Update, ENABLE);	/* 如果是要调节占空比就把这行去掉注释，然后注释掉下面那行，再把DMA通道6改为DMA通道3 */
	TIM_DMACmd(TIM3, TIM_DMA_CC1, ENABLE);
	TIM_Cmd(TIM3, ENABLE);  /* 使能TIM3 */
	
}



/*
#define SEND_BUF_SIZE 8200	//发送数据长度,最好等于sizeof(TEXT_TO_SEND)+2的整数倍.
u8 SendBuff[SEND_BUF_SIZE];	//发送数据缓冲区

void MYDMA_Init(){
	
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //使能 DMA 时钟
	DMA_DeInit(DMA1_Channel6);   // 将DMA的通道1寄存器重设为缺省值 
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = TIM3->PSC; 	//TIM3CH1 CRR寄存器
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)SendBuff; //DMA 内存基地址 要发送内容的地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  //数据传输方向，从内存读取发送到外设
	DMA_InitStructure.DMA_BufferSize = SEND_BUF_SIZE;  //DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //工作在正常缓存模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMA通道 x拥有中优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	
	
	
	DMA_Init(DMA1_Channel6, &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器
	
}

u16 DMA1_MEM_LEN;

//开启一次DMA传输
void MYDMA_Enable(DMA_Channel_TypeDef*DMA_CHx)
{
	//SendBuff[0] = 100;
	int i;
	for (i = 0; i < sizeof(SEND_BUF_SIZE); i++){
		SendBuff[i] = i;
	}
	
	DMA_Cmd(DMA_CHx, ENABLE);  //关闭USART1 TX DMA1 所指示的通道      
 	DMA_SetCurrDataCounter(DMA_CHx,DMA1_MEM_LEN);//DMA通道的DMA缓存的大小
 	DMA_Cmd(DMA_CHx, DISABLE);  //使能USART1 TX DMA1 所指示的通道 

}

*/


Motor m;

// 角度转脉冲  通过角度正负设置方向
void step_angletwopulse(int angle){	
	
	if(angle >= 0) m.dir = 1;
	else m.dir = 0;
	
	m.pulsesNum = angle / m.stepAngle / m.subdivision;
	
	if(m.pulsesNum == 0){ //如果脉冲数为0 则关闭PWM输出
			
		TIM_Cmd(TIM3, DISABLE);  //关闭TIM3
	
	}
	else{
	
		TIM_Cmd(TIM3, ENABLE);  //使能TIM3

	}
	
}



// 控制定时器输出多少个脉冲数
void step_pulseNum(int pulsenum){
	
	if(pulsenum != 0){  //如果脉冲数不为0，则输出脉冲
			
	}
	
}





