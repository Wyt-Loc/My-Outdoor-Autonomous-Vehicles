#ifndef __MAIN_H__
#define __MAIN_H__

#include <reg52.h>



//�������߱�������
extern void Delay_ms(unsigned int n);

extern void printGpsBuffer();
extern void parseGpsBuffer();
extern void errorLog(int num);


#endif