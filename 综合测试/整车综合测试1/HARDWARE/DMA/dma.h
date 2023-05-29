#ifndef __DMA_H
#define	__DMA_H	   
#include "sys.h"
#include "delay.h"
#include "dma.h"
#include "timer.h"
#include "usart.h"
#include "stm32f10x_dma.h"

void NVIC_Configuration(void);				    					    

void DMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);//≈‰÷√DMA1_CHx

void DMA_Enable(DMA_Channel_TypeDef*DMA_CHx);// πƒ‹DMA1_CHx

u16 DMA_send_feedback(DMA_Channel_TypeDef* DMA_CHx);

void DMA1_Channel6_IRQHandler(void);
#endif

