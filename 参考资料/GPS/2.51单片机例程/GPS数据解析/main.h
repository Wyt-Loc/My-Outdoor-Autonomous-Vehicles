#ifndef __MAIN_H__
#define __MAIN_H__

#include <reg52.h>



//函数或者变量声明
extern void Delay_ms(unsigned int n);

extern void printGpsBuffer();
extern void parseGpsBuffer();
extern void errorLog(int num);


#endif