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
	uart2_init(115200);  //串口接收
	KEY_Init();
	time3_initer(0xffff, 35); //步进电机定时器
	Servos_PWM_Init(ARR_5000-1, PSC-1); //舵机定时器初始化
	BaseTimer_Init();
	delay_ms(20);
	TIM_SetCompare1(TIM4,559);
	TIM5_Int_Init(1,7199);   //提供lvgl的心跳
	
	lv_init();
	lv_port_disp_init();  // lvgl显示接口初始化,放在lv_init()的后面
	lv_port_indev_init(); // lvgl输入接口初始化,放在lv_init()的后面
	
	/* 加载标签 */
	lvgl_lable_test();
	//显示速度
	lvgl_lable();
	//加速按钮
	lvgl_button1();
	//减速按钮
	//lvgl_button2();
	/* 加载按钮 */
	lvgl_button_test();

	while(1){

		delay_ms(5);
		lv_timer_handler(); // lvgl的事务处理
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




