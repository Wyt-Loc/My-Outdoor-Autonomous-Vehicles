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
#include "motorT.h"
#include "Servos.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "my_gui.h"


int main(void)
{

	delay_init();	    	 //��ʱ������ʼ��	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200
	uart2_init(115200);  //���ڽ���
	LED_Init();			     //LED�˿ڳ�ʼ��
	KEY_Init();
	lv_init();
	lv_port_disp_init();  // lvgl��ʾ�ӿڳ�ʼ��,����lv_init()�ĺ���
	lv_port_indev_init(); // lvgl����ӿڳ�ʼ��,����lv_init()�ĺ���
	TIM5_Int_Init(1,7199);   //�ṩlvgl������
	time3_initer(0xffff, 35); //���������ʱ��
	Servos_PWM_Init(ARR_5000-1, PSC-1); //�����ʱ����ʼ��

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

		usartReceData(); //���ڽ�������

	}
}


