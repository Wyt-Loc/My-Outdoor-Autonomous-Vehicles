/**
  ******************************************************************************
  * 文件名程: main.c 
  * 作    者: 硬石嵌入式开发团队
  * 版    本: V1.1
  * 编写日期: 2017-4-26
  * 功    能: STEPMOTOR步进电机控制实现
  ******************************************************************************
  * 说明：
  * 本例程配套硬石stm32开发板YS-F1Pro使用。
  * 
  * 淘宝：
  * 论坛：http://www.ing10bbs.com
  * 版权归硬石嵌入式开发团队所有，请勿商用。
  ******************************************************************************
  */
/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "StepMotor/bsp_StepMotor.h"
#include "key/bsp_key.h"

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
#define  FASTSEEK_SPEED   200		//回归速度
#define  SLOWSEEK_SPEED   70		//爬行速度>=基底速度20//加减速不宜太大
/* 私有变量 ------------------------------------------------------------------*/

// 速度最大值由驱动器和电机决定，有些最大是1800，有些可以达到4000
__IO uint32_t set_speed  = 300;         // 最高速度 单位为0.1rad/sec
// 加速度和减速度选取一般根据实际需要，值越大速度变化越快，加减速阶段比较抖动
// 所以加速度和减速度值一般是在实际应用中多尝试出来的结果
__IO uint32_t step_accel = 100;         // 加速度 单位为0.1rad/sec^2
__IO uint32_t step_decel = 70;          // 减速度 单位为0.1rad/sec^2

/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/
/**
  * 函数功能: 系统时钟配置
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;  // 外部晶振，8MHz
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;  // 9倍频，得到72MHz主时钟
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;       // 系统时钟：72MHz
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;              // AHB时钟：72MHz
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;               // APB1时钟：36MHz
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;               // APB2时钟：72MHz
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

 	// HAL_RCC_GetHCLKFreq()/1000    1ms中断一次
	// HAL_RCC_GetHCLKFreq()/100000	 10us中断一次
	// HAL_RCC_GetHCLKFreq()/1000000 1us中断一次
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);  // 配置并启动系统滴答定时器
  /* 系统滴答定时器时钟源 */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
  /* 系统滴答定时器中断优先级配置 */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * 函数功能: 主函数.
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
int main(void)
{   
	uint8_t  Mode_Count = 0;
								
  /* 复位所有外设，初始化Flash接口和系统滴答定时器 */
	HAL_Init();
  /* 配置系统时钟 */
	SystemClock_Config();

	KEY_GPIO_Init();
  
  STEPMOTOR_TIMx_Init();	
  /* 无限循环 */
	while (1)
	{
				//STEPMOTOR_AxisHome(AXIS_X,FASTSEEK_SPEED,SLOWSEEK_SPEED,step_accel,step_decel);	//搜索X轴原点
		    //STEPMOTOR_AxisHome(AXIS_Y,FASTSEEK_SPEED,SLOWSEEK_SPEED,step_accel,step_decel);	//搜索X轴原点
		//    STEPMOTOR_AxisHome(AXIS_Z,FASTSEEK_SPEED,SLOWSEEK_SPEED,step_accel,step_decel);	//搜索X轴原点
				if(KEY1_StateRead() == KEY_DOWN)
				{
						STEPMOTOR_DisMoveAbs(AXIS_X,100,step_accel,step_decel,set_speed);//X轴移动到100mm的位置
					
				}
				if(KEY2_StateRead() == KEY_UP)
				{
						STEPMOTOR_DisMoveAbs(AXIS_Y,200,step_accel,step_decel,set_speed);//Y轴移动到200mm的位置
					//	STEPMOTOR_AxisMoveRel(AXIS_Y,200,step_accel,step_decel,set_speed);
				}	
				if(KEY3_StateRead() == KEY_UP)
				{
						STEPMOTOR_DisMoveAbs(AXIS_Z,300,step_accel,step_decel,set_speed);//Z轴移动到200mm的位置	
							
				}		
				
				if(KEY4_StateRead() == KEY_UP)
				{
						// STEPMOTOR_AxisMoveRel(AXIS_X,30*SPR*CCW,step_accel,step_decel,set_speed); 	
				}		
							
				
		}
		
		
}
		
		


/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
