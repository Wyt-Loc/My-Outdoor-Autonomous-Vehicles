#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "Motor.h"


int main(void)
{
	 
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  delay_init();            //延时函数初始化
  uart_init(115200);   //串口初始化为115200
	main_Init();
	delay_ms(1000);
	Pulse_output_1(1000, 3200); // 频率 和 脉冲数
	
  while(1)
  {

  }
	
}


