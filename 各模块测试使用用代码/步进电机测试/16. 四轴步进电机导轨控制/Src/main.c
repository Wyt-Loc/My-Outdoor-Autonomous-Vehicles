/**
  ******************************************************************************
  * �ļ�����: main.c 
  * ��    ��: ӲʯǶ��ʽ�����Ŷ�
  * ��    ��: V1.1
  * ��д����: 2017-4-26
  * ��    ��: STEPMOTOR�����������ʵ��
  ******************************************************************************
  * ˵����
  * ����������Ӳʯstm32������YS-F1Proʹ�á�
  * 
  * �Ա���
  * ��̳��http://www.ing10bbs.com
  * ��Ȩ��ӲʯǶ��ʽ�����Ŷ����У��������á�
  ******************************************************************************
  */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "StepMotor/bsp_StepMotor.h"
#include "key/bsp_key.h"

/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
#define  FASTSEEK_SPEED   200		//�ع��ٶ�
#define  SLOWSEEK_SPEED   70		//�����ٶ�>=�����ٶ�20//�Ӽ��ٲ���̫��
/* ˽�б��� ------------------------------------------------------------------*/

// �ٶ����ֵ���������͵����������Щ�����1800����Щ���Դﵽ4000
__IO uint32_t set_speed  = 300;         // ����ٶ� ��λΪ0.1rad/sec
// ���ٶȺͼ��ٶ�ѡȡһ�����ʵ����Ҫ��ֵԽ���ٶȱ仯Խ�죬�Ӽ��ٽ׶αȽ϶���
// ���Լ��ٶȺͼ��ٶ�ֵһ������ʵ��Ӧ���жೢ�Գ����Ľ��
__IO uint32_t step_accel = 100;         // ���ٶ� ��λΪ0.1rad/sec^2
__IO uint32_t step_decel = 70;          // ���ٶ� ��λΪ0.1rad/sec^2

/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/
/**
  * ��������: ϵͳʱ������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;  // �ⲿ����8MHz
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;  // 9��Ƶ���õ�72MHz��ʱ��
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;       // ϵͳʱ�ӣ�72MHz
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;              // AHBʱ�ӣ�72MHz
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;               // APB1ʱ�ӣ�36MHz
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;               // APB2ʱ�ӣ�72MHz
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

 	// HAL_RCC_GetHCLKFreq()/1000    1ms�ж�һ��
	// HAL_RCC_GetHCLKFreq()/100000	 10us�ж�һ��
	// HAL_RCC_GetHCLKFreq()/1000000 1us�ж�һ��
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);  // ���ò�����ϵͳ�δ�ʱ��
  /* ϵͳ�δ�ʱ��ʱ��Դ */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
  /* ϵͳ�δ�ʱ���ж����ȼ����� */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
  * ��������: ������.
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
int main(void)
{   
	uint8_t  Mode_Count = 0;
								
  /* ��λ�������裬��ʼ��Flash�ӿں�ϵͳ�δ�ʱ�� */
	HAL_Init();
  /* ����ϵͳʱ�� */
	SystemClock_Config();

	KEY_GPIO_Init();
  
  STEPMOTOR_TIMx_Init();	
  /* ����ѭ�� */
	while (1)
	{
				//STEPMOTOR_AxisHome(AXIS_X,FASTSEEK_SPEED,SLOWSEEK_SPEED,step_accel,step_decel);	//����X��ԭ��
		    //STEPMOTOR_AxisHome(AXIS_Y,FASTSEEK_SPEED,SLOWSEEK_SPEED,step_accel,step_decel);	//����X��ԭ��
		//    STEPMOTOR_AxisHome(AXIS_Z,FASTSEEK_SPEED,SLOWSEEK_SPEED,step_accel,step_decel);	//����X��ԭ��
				if(KEY1_StateRead() == KEY_DOWN)
				{
						STEPMOTOR_DisMoveAbs(AXIS_X,100,step_accel,step_decel,set_speed);//X���ƶ���100mm��λ��
					
				}
				if(KEY2_StateRead() == KEY_UP)
				{
						STEPMOTOR_DisMoveAbs(AXIS_Y,200,step_accel,step_decel,set_speed);//Y���ƶ���200mm��λ��
					//	STEPMOTOR_AxisMoveRel(AXIS_Y,200,step_accel,step_decel,set_speed);
				}	
				if(KEY3_StateRead() == KEY_UP)
				{
						STEPMOTOR_DisMoveAbs(AXIS_Z,300,step_accel,step_decel,set_speed);//Z���ƶ���200mm��λ��	
							
				}		
				
				if(KEY4_StateRead() == KEY_UP)
				{
						// STEPMOTOR_AxisMoveRel(AXIS_X,30*SPR*CCW,step_accel,step_decel,set_speed); 	
				}		
							
				
		}
		
		
}
		
		


/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/
