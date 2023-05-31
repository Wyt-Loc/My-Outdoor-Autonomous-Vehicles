#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"



/*一下所有的测试基于舵机灵敏度和转动角度最大的情况*/


//#define ARR_2000 2000							//选择ARR的值
#define ARR_5000 5000


#ifdef ARR_2000

#define PSC 720

#define ServoAngleMAXLeft 250   	//舵机打角最大值   ---左舵机
#define ServoAngleMINLeft 50			//舵机打角最小值
#define ServoMedianLeft  100			//舵机中值

#define ServoAngleMAXRight 250   	//舵机打角最大值   ---右舵机
#define ServoAngleMINRight 50			//舵机打角最小值
#define ServoMedianRight  100			//舵机中值

#elif ARR_5000

#define PSC 288

#define ServoAngleMAXLeft 680   	//舵机打角最大值   ---左舵机
#define ServoAngleMINLeft 135			//舵机打角最小值
#define ServoMedianLeft  405			//舵机中值

#define ServoAngleMAXRight 680   	//舵机打角最大值   ---右舵机
#define ServoAngleMINRight 135			//舵机打角最小值
#define ServoMedianRight  405			//舵机中值

#endif


//#define MethodOne  1
#define MethodTwo  2

int main(void)
{
	
	u8 key=0;
	u16 duty = 130;   //大概范围  50 -- 250  对应0 -- 180°
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(115200);
	KEY_Init();
	TIM4_PWM_Init(ARR_5000-1, PSC-1);  	// 50hz   0.02s周期
	
	while (1)
	{

		key = KEY_Scan(0);
		
		
#ifdef MethodOne			   //测试方法一:
		
		if (key == 1 && duty < ServoAngleMAXLeft){    //1，KEY0按下   duty +5
			
			duty += 5;
			TIM_SetCompare2(TIM3,duty);	
			printf("arr = %d",duty);
			
		}

		if (key == 2 && duty > ServoAngleMINRight){	  //2，KEY1按下		duty -5
			
			duty -= 5;
			TIM_SetCompare2(TIM3,duty);	
			printf("arr = %d",duty);
			
		}
		
		if(key == 3){																	//3. KEY3按下    舵机回到中心位置
			
			TIM_SetCompare2(TIM3,ServoMedianRight);	
			printf("arr = %d",duty);
			
		}
		
#elif	 	MethodTwo			  //测试方法二:
		
		if (key == 1 ){    //1，KEY0按下   回到舵机角度最大位置
			 
			duty = ServoAngleMAXLeft;
			TIM_SetCompare1(TIM4,ServoAngleMAXLeft);	
			printf("arr = %d",duty);
			
		}

		if (key == 2 ){	  //2，KEY1按下		 回到舵机角度最小位置
			
			duty = ServoAngleMINRight;
			TIM_SetCompare1(TIM4,ServoAngleMINRight);	
			printf("arr = %d",duty);
			
		}
		
		if(key == 3 ){																	//3. KEY3按下    回到舵机中心位置
			
			duty = ServoMedianRight;
			TIM_SetCompare1(TIM4,ServoMedianRight);	
			printf("arr = %d",duty);
			
		}
		
#endif
		
	}
}

