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

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init();
	LED_Init();
	uart_init(115200);
	uart2_init(115200);  //���ڽ���
	KEY_Init();
	time3_initer(0xffff, 35); //���������ʱ��
	Servos_PWM_Init(ARR_5000-1, PSC-1); //�����ʱ����ʼ��
	BaseTimer_Init();
	delay_ms(20);
	TIM_SetCompare1(TIM4,559);
	TIM5_Int_Init(1,7199);   //�ṩlvgl������
	
	lv_init();
	lv_port_disp_init();  // lvgl��ʾ�ӿڳ�ʼ��,����lv_init()�ĺ���
	lv_port_indev_init(); // lvgl����ӿڳ�ʼ��,����lv_init()�ĺ���
	
	/* ���ر�ǩ */
	lvgl_lable_test();
	//��ʾ�ٶ�
	lvgl_lable();
	//���ٰ�ť
	lvgl_button1();
	//���ٰ�ť
	//lvgl_button2();
	/* ���ذ�ť */
	lvgl_button_test();

	while(1){

		delay_ms(5);
		lv_timer_handler(); // lvgl��������
		Control(usartRecvData());

//		if(KEY_Scan(1) == 2){
//			//ServosLeftRightClick(1);
//			Control(7);
//			LED0 = 1;
//		}
//		
//		if(KEY_Scan(1) == 3){
//			//ServosLeftRightClick(2);
//			Control(7);
//			LED0 = 0;
//		}

	}
}




