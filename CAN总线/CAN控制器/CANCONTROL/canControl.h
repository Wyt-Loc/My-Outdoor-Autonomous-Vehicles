#ifndef __CANCONTROL_H
#define __CANCONTROL_H	 
#include "sys.h"


#define ERRORRANGE  10  // 可接受的错误范围

char motorValueVerify(u8 dir, float distance, float speed);
u8* motorCommandGenerate(u8 dir, float distance, float speed);
u8 sendMotorCommand(u8 dir, float distance, float speed);
void test(void);
void ControlEntryFunction(u8 dir, float distance, float speed);

		 				    
#endif
