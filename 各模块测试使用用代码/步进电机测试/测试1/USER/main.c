#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "Motor.h"


int main(void)
{
	 
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  delay_init();            //��ʱ������ʼ��
  uart_init(115200);   //���ڳ�ʼ��Ϊ115200
	main_Init();
	delay_ms(1000);
	Pulse_output_1(1000, 3200); // Ƶ�� �� ������
	
  while(1)
  {

  }
	
}


