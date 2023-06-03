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

	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
	uart2_init(115200);  //串口接收
	LED_Init();			     //LED端口初始化
	KEY_Init();
	lv_init();
	lv_port_disp_init();  // lvgl显示接口初始化,放在lv_init()的后面
	lv_port_indev_init(); // lvgl输入接口初始化,放在lv_init()的后面
	TIM5_Int_Init(1,7199);   //提供lvgl的心跳
	time3_initer(0xffff, 35); //步进电机定时器
	Servos_PWM_Init(ARR_5000-1, PSC-1); //舵机定时器初始化

	/* 加载标签 */
	lvgl_lable_test();
	//显示速度
	lvgl_lable();
	//加速按钮
	lvgl_button1();
	//减速按钮
	lvgl_button2();
	/* 加载按钮 */
	lvgl_button_test();

	while(1){

		delay_ms(5);
		lv_timer_handler(); // lvgl的事务处理

		usartReceData(); //串口接收数据

	}
}


