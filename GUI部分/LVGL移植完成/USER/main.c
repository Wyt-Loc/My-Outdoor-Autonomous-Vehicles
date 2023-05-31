#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 
#include "24cxx.h"
#include "w25qxx.h"
#include "touch.h"
#include "timer.h"


#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "my_gui.h"


int main(void)
{

	delay_init();	    	 //��ʱ������ʼ��	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
	LED_Init();			     //LED�˿ڳ�ʼ��
	
	Drive_io_init();
	lv_init();
	lv_port_disp_init();  // lvgl��ʾ�ӿڳ�ʼ��,����lv_init()�ĺ���
	lv_port_indev_init(); // lvgl����ӿڳ�ʼ��,����lv_init()�ĺ���
	TIM3_Int_Init(1,7199);   //�ṩlvgl������
	TIM4_PWM_Init(899, 0); //��������
	
	  /* ���ر�ǩ */
    lvgl_lable_test();
		//��ʾ�ٶ�
		lvgl_lable();
		//���ٰ�ť
		lvgl_button1();
		//���ٰ�ť
		lvgl_button2();
    /* ���ذ�ť */
    lvgl_button_test();
	
	while(1){
		delay_ms(5);
		lv_timer_handler(); // lvgl��������
	}

	
}


