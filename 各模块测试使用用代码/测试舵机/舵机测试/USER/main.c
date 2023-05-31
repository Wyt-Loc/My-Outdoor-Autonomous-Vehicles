#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"



/*һ�����еĲ��Ի��ڶ�������Ⱥ�ת���Ƕ��������*/


//#define ARR_2000 2000							//ѡ��ARR��ֵ
#define ARR_5000 5000


#ifdef ARR_2000

#define PSC 720

#define ServoAngleMAXLeft 250   	//���������ֵ   ---����
#define ServoAngleMINLeft 50			//��������Сֵ
#define ServoMedianLeft  100			//�����ֵ

#define ServoAngleMAXRight 250   	//���������ֵ   ---�Ҷ��
#define ServoAngleMINRight 50			//��������Сֵ
#define ServoMedianRight  100			//�����ֵ

#elif ARR_5000

#define PSC 288

#define ServoAngleMAXLeft 680   	//���������ֵ   ---����
#define ServoAngleMINLeft 135			//��������Сֵ
#define ServoMedianLeft  405			//�����ֵ

#define ServoAngleMAXRight 680   	//���������ֵ   ---�Ҷ��
#define ServoAngleMINRight 135			//��������Сֵ
#define ServoMedianRight  405			//�����ֵ

#endif


//#define MethodOne  1
#define MethodTwo  2

int main(void)
{
	
	u8 key=0;
	u16 duty = 130;   //��ŷ�Χ  50 -- 250  ��Ӧ0 -- 180��
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(115200);
	KEY_Init();
	TIM4_PWM_Init(ARR_5000-1, PSC-1);  	// 50hz   0.02s����
	
	while (1)
	{

		key = KEY_Scan(0);
		
		
#ifdef MethodOne			   //���Է���һ:
		
		if (key == 1 && duty < ServoAngleMAXLeft){    //1��KEY0����   duty +5
			
			duty += 5;
			TIM_SetCompare2(TIM3,duty);	
			printf("arr = %d",duty);
			
		}

		if (key == 2 && duty > ServoAngleMINRight){	  //2��KEY1����		duty -5
			
			duty -= 5;
			TIM_SetCompare2(TIM3,duty);	
			printf("arr = %d",duty);
			
		}
		
		if(key == 3){																	//3. KEY3����    ����ص�����λ��
			
			TIM_SetCompare2(TIM3,ServoMedianRight);	
			printf("arr = %d",duty);
			
		}
		
#elif	 	MethodTwo			  //���Է�����:
		
		if (key == 1 ){    //1��KEY0����   �ص�����Ƕ����λ��
			 
			duty = ServoAngleMAXLeft;
			TIM_SetCompare1(TIM4,ServoAngleMAXLeft);	
			printf("arr = %d",duty);
			
		}

		if (key == 2 ){	  //2��KEY1����		 �ص�����Ƕ���Сλ��
			
			duty = ServoAngleMINRight;
			TIM_SetCompare1(TIM4,ServoAngleMINRight);	
			printf("arr = %d",duty);
			
		}
		
		if(key == 3 ){																	//3. KEY3����    �ص��������λ��
			
			duty = ServoMedianRight;
			TIM_SetCompare1(TIM4,ServoMedianRight);	
			printf("arr = %d",duty);
			
		}
		
#endif
		
	}
}

