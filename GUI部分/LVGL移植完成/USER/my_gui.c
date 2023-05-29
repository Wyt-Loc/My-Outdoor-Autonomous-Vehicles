#include "my_gui.h"
#include "led.h"
#include "lvgl.h"						//lvgl的主要部分 大部分都在这里定义
#include "lv_port_indev.h" 	//主要是得到触摸屏按下时的一些参数
#include "touch.h"
#include "timer.h"


lv_obj_t *label; 		// 测试界面对象
lv_obj_t *labe2; 		// 测试界面对象


//是否按下
u8 ispressed(){
	tp_dev.scan(0);
	if(tp_dev.sta & TP_PRES_DOWN)
		return true;
	return false;
}

u16 duty = 99; 
void Forward_rotation(){
	//正转
	GPIO_SetBits(GPIOB,GPIO_Pin_2);  				 	//高电平转
	TIM_SetCompare1(TIM4, duty);
	GPIO_ResetBits(GPIOB,GPIO_Pin_7);
}
void Stop_it(){
		//停止
	GPIO_ResetBits(GPIOB,GPIO_Pin_2);  				 	//高电平转
	TIM_SetCompare1(TIM4, duty);
	GPIO_ResetBits(GPIOB,GPIO_Pin_7);
}

// 加速
void Speed_up(){
	
	duty = duty+100;
	TIM_SetCompare1(TIM4, duty);
	
}

//减速
void Speed_down(){
	
	duty = duty-100;
	TIM_SetCompare1(TIM4, duty);
	
}

/**
 * @brief 创建标签 显示状态
 */
void lvgl_lable_test(){
    /* 创建一个标签 */
    label = lv_label_create(lv_scr_act());
    if (NULL != label)
    {
        // lv_obj_set_x(label, 90);                         // 设置控件的X坐标
        // lv_obj_set_y(label, 100);                        // 设置控件的Y坐标
        // lv_obj_set_size(label, 60, 20);                  // 设置控件大小
				Stop_it();
        lv_label_set_text(label, "stop...");                // 初始显示 0
        // lv_obj_center(label);                            // 居中显示
        lv_obj_align(label, LV_ALIGN_CENTER, 0, -50);       // 居中显示后，向上偏移50
    }
}

int speedv = 0;

/**
 * @brief 创建标签显示速度
 */
void lvgl_lable(){
    /* 创建一个标签 */
    labe2 = lv_label_create(lv_scr_act());
    if (NULL != labe2)
    {
        // lv_obj_set_x(label, 90);                         // 设置控件的X坐标
        // lv_obj_set_y(label, 100);                        // 设置控件的Y坐标
        // lv_obj_set_size(label, 60, 20);                  // 设置控件大小
        lv_label_set_text_fmt(labe2, "speed = %d", speedv); // 初始显示 0
        // lv_obj_center(label);                            // 居中显示
        lv_obj_align(labe2, LV_ALIGN_CENTER, 100, -50);       // 居中显示后，向上偏移50
    }
}


/**
 * @brief 按钮事件回调函数
 */
static void btn_event_callback(lv_event_t* event)
{
    static uint32_t counter = 1;
 
    lv_obj_t* btn = lv_event_get_target(event);                 //获取事件对象
    if (btn != NULL)
    {
				LED0 = ~LED0;
				if(counter % 2){
					Forward_rotation();
					lv_label_set_text(label, "start...");            //设置显示内容
				}
				else{
					Stop_it();
					lv_label_set_text(label, "stop...");
				}
				if(counter == 3)
					counter = 1;
        lv_obj_align(label, LV_ALIGN_CENTER, 0, -50);           // 居中显示后，向上偏移50
        counter++;
    }
}


//加速回调函数
static void btn1_event_callback(lv_event_t* event)
{
   lv_obj_t* btn = lv_event_get_target(event);                 //获取事件对象
    if (btn != NULL)
    {
			if(speedv < 5){
				Speed_up();
				speedv += 1;
				lv_label_set_text_fmt(labe2, "speed = %d", speedv);            //设置显示内容
			}
			else if(speedv >= 5){
				speedv = 5;
				lv_label_set_text(labe2, "speed is max");            //设置显示内容
			}
			
			lv_obj_align(labe2, LV_ALIGN_CENTER, 0, -80);           // 居中显示后，向上偏移50
    }
}


//减速回调函数
static void btn2_event_callback(lv_event_t* event)
{
    lv_obj_t* btn = lv_event_get_target(event);                 //获取事件对象
    if (btn != NULL)
    {
			if(speedv <= 0){
				speedv = 0;
				lv_label_set_text(labe2, "speed is min");            //设置显示内容
			}
			else{
				speedv -= 1;
				Speed_down();
				lv_label_set_text_fmt(labe2, "speed = %d", speedv);            //设置显示内容
			}
			lv_obj_align(labe2, LV_ALIGN_CENTER, 0, -80);           // 居中显示后，向上偏移50
    }
}


/**
 * @brief 创建按钮
 */
void lvgl_button_test(){
    /* 在当前界面中创建一个按钮 */
    lv_obj_t* btn = lv_btn_create(lv_scr_act());                                        // 创建Button对象
    if (btn != NULL)
    {

        lv_obj_set_size(btn, 80, 20);                                                   // 设置对象宽度和高度
        // lv_obj_set_pos(btn, 90, 200);                                                // 设置按钮的X和Y坐标
        lv_obj_add_event_cb(btn, btn_event_callback, LV_EVENT_CLICKED, NULL);           // 给对象添加CLICK事件和事件处理回调函数
        lv_obj_align(btn, LV_ALIGN_CENTER, 0, 50);                                      // 居中显示后，向下偏移50
 
        lv_obj_t* btn_label = lv_label_create(btn);                                     // 基于Button对象创建Label对象
        if (btn_label != NULL)
        {
            lv_label_set_text(btn_label, "switch");                                     // 设置显示内容
            lv_obj_center(btn_label);                                                   // 对象居中显示
        }
    }
}


/**
 * @brief 创建按钮加速
 */
void lvgl_button1(){
    /* 在当前界面中创建一个按钮 */
    lv_obj_t* btn1 = lv_btn_create(lv_scr_act());                                        // 创建Button对象
    if (btn1 != NULL)
    {
        lv_obj_set_size(btn1, 80, 20);                                                   // 设置对象宽度和高度
        // lv_obj_set_pos(btn, 90, 200);                                               // 设置按钮的X和Y坐标
        lv_obj_add_event_cb(btn1, btn1_event_callback, LV_EVENT_CLICKED, NULL);           // 给对象添加CLICK事件和事件处理回调函数
        lv_obj_align(btn1, LV_ALIGN_CENTER, 100, 50);                                      // 居中显示后，向下偏移50
 
        lv_obj_t* btn_label = lv_label_create(btn1);                                     // 基于Button对象创建Label对象
        if (btn_label != NULL)
        {
            lv_label_set_text_fmt(btn_label, "speed+1");                                     // 设置显示内容
            lv_obj_center(btn_label);                                                   // 对象居中显示
        }
    }
}



/**
 * @brief 创建按钮减速
 */
void lvgl_button2(){
    /* 在当前界面中创建一个按钮 */
    lv_obj_t* btn = lv_btn_create(lv_scr_act());                                        // 创建Button对象
    if (btn != NULL)
    {
        lv_obj_set_size(btn, 80, 20);                                                   // 设置对象宽度和高度
        // lv_obj_set_pos(btn, 90, 200);                                                // 设置按钮的X和Y坐标
        lv_obj_add_event_cb(btn, btn2_event_callback, LV_EVENT_CLICKED, NULL);           // 给对象添加CLICK事件和事件处理回调函数
        lv_obj_align(btn, LV_ALIGN_CENTER, -100, 50);                                      // 居中显示后，向下偏移50
 
        lv_obj_t* btn_label = lv_label_create(btn);                                     // 基于Button对象创建Label对象
        if (btn_label != NULL)
        {
            lv_label_set_text(btn_label, "speed-1");                                     // 设置显示内容
            lv_obj_center(btn_label);                                                   // 对象居中显示
        }
    }
}





