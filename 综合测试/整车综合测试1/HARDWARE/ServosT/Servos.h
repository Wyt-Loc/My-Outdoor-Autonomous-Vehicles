#ifndef __Servos_H
#define __Servos_H

#include "sys.h"


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




void Servos_PWM_Init(u16 arr,u16 psc);
void ServosLeftRightClick(u8 leftorright);
void ServosLeftRightLong(void);

#endif

