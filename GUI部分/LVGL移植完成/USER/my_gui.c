#include "my_gui.h"
#include "led.h"
#include "lvgl.h"						//lvgl����Ҫ���� �󲿷ֶ������ﶨ��
#include "lv_port_indev.h" 	//��Ҫ�ǵõ�����������ʱ��һЩ����
#include "touch.h"
#include "timer.h"


lv_obj_t *label; 		// ���Խ������
lv_obj_t *labe2; 		// ���Խ������


//�Ƿ���
u8 ispressed(){
	tp_dev.scan(0);
	if(tp_dev.sta & TP_PRES_DOWN)
		return true;
	return false;
}

u16 duty = 99; 
void Forward_rotation(){
	//��ת
	GPIO_SetBits(GPIOB,GPIO_Pin_2);  				 	//�ߵ�ƽת
	TIM_SetCompare1(TIM4, duty);
	GPIO_ResetBits(GPIOB,GPIO_Pin_7);
}
void Stop_it(){
		//ֹͣ
	GPIO_ResetBits(GPIOB,GPIO_Pin_2);  				 	//�ߵ�ƽת
	TIM_SetCompare1(TIM4, duty);
	GPIO_ResetBits(GPIOB,GPIO_Pin_7);
}

// ����
void Speed_up(){
	
	duty = duty+100;
	TIM_SetCompare1(TIM4, duty);
	
}

//����
void Speed_down(){
	
	duty = duty-100;
	TIM_SetCompare1(TIM4, duty);
	
}

/**
 * @brief ������ǩ ��ʾ״̬
 */
void lvgl_lable_test(){
    /* ����һ����ǩ */
    label = lv_label_create(lv_scr_act());
    if (NULL != label)
    {
        // lv_obj_set_x(label, 90);                         // ���ÿؼ���X����
        // lv_obj_set_y(label, 100);                        // ���ÿؼ���Y����
        // lv_obj_set_size(label, 60, 20);                  // ���ÿؼ���С
				Stop_it();
        lv_label_set_text(label, "stop...");                // ��ʼ��ʾ 0
        // lv_obj_center(label);                            // ������ʾ
        lv_obj_align(label, LV_ALIGN_CENTER, 0, -50);       // ������ʾ������ƫ��50
    }
}

int speedv = 0;

/**
 * @brief ������ǩ��ʾ�ٶ�
 */
void lvgl_lable(){
    /* ����һ����ǩ */
    labe2 = lv_label_create(lv_scr_act());
    if (NULL != labe2)
    {
        // lv_obj_set_x(label, 90);                         // ���ÿؼ���X����
        // lv_obj_set_y(label, 100);                        // ���ÿؼ���Y����
        // lv_obj_set_size(label, 60, 20);                  // ���ÿؼ���С
        lv_label_set_text_fmt(labe2, "speed = %d", speedv); // ��ʼ��ʾ 0
        // lv_obj_center(label);                            // ������ʾ
        lv_obj_align(labe2, LV_ALIGN_CENTER, 100, -50);       // ������ʾ������ƫ��50
    }
}


/**
 * @brief ��ť�¼��ص�����
 */
static void btn_event_callback(lv_event_t* event)
{
    static uint32_t counter = 1;
 
    lv_obj_t* btn = lv_event_get_target(event);                 //��ȡ�¼�����
    if (btn != NULL)
    {
				LED0 = ~LED0;
				if(counter % 2){
					Forward_rotation();
					lv_label_set_text(label, "start...");            //������ʾ����
				}
				else{
					Stop_it();
					lv_label_set_text(label, "stop...");
				}
				if(counter == 3)
					counter = 1;
        lv_obj_align(label, LV_ALIGN_CENTER, 0, -50);           // ������ʾ������ƫ��50
        counter++;
    }
}


//���ٻص�����
static void btn1_event_callback(lv_event_t* event)
{
   lv_obj_t* btn = lv_event_get_target(event);                 //��ȡ�¼�����
    if (btn != NULL)
    {
			if(speedv < 5){
				Speed_up();
				speedv += 1;
				lv_label_set_text_fmt(labe2, "speed = %d", speedv);            //������ʾ����
			}
			else if(speedv >= 5){
				speedv = 5;
				lv_label_set_text(labe2, "speed is max");            //������ʾ����
			}
			
			lv_obj_align(labe2, LV_ALIGN_CENTER, 0, -80);           // ������ʾ������ƫ��50
    }
}


//���ٻص�����
static void btn2_event_callback(lv_event_t* event)
{
    lv_obj_t* btn = lv_event_get_target(event);                 //��ȡ�¼�����
    if (btn != NULL)
    {
			if(speedv <= 0){
				speedv = 0;
				lv_label_set_text(labe2, "speed is min");            //������ʾ����
			}
			else{
				speedv -= 1;
				Speed_down();
				lv_label_set_text_fmt(labe2, "speed = %d", speedv);            //������ʾ����
			}
			lv_obj_align(labe2, LV_ALIGN_CENTER, 0, -80);           // ������ʾ������ƫ��50
    }
}


/**
 * @brief ������ť
 */
void lvgl_button_test(){
    /* �ڵ�ǰ�����д���һ����ť */
    lv_obj_t* btn = lv_btn_create(lv_scr_act());                                        // ����Button����
    if (btn != NULL)
    {

        lv_obj_set_size(btn, 80, 20);                                                   // ���ö����Ⱥ͸߶�
        // lv_obj_set_pos(btn, 90, 200);                                                // ���ð�ť��X��Y����
        lv_obj_add_event_cb(btn, btn_event_callback, LV_EVENT_CLICKED, NULL);           // ���������CLICK�¼����¼�����ص�����
        lv_obj_align(btn, LV_ALIGN_CENTER, 0, 50);                                      // ������ʾ������ƫ��50
 
        lv_obj_t* btn_label = lv_label_create(btn);                                     // ����Button���󴴽�Label����
        if (btn_label != NULL)
        {
            lv_label_set_text(btn_label, "switch");                                     // ������ʾ����
            lv_obj_center(btn_label);                                                   // ���������ʾ
        }
    }
}


/**
 * @brief ������ť����
 */
void lvgl_button1(){
    /* �ڵ�ǰ�����д���һ����ť */
    lv_obj_t* btn1 = lv_btn_create(lv_scr_act());                                        // ����Button����
    if (btn1 != NULL)
    {
        lv_obj_set_size(btn1, 80, 20);                                                   // ���ö����Ⱥ͸߶�
        // lv_obj_set_pos(btn, 90, 200);                                               // ���ð�ť��X��Y����
        lv_obj_add_event_cb(btn1, btn1_event_callback, LV_EVENT_CLICKED, NULL);           // ���������CLICK�¼����¼�����ص�����
        lv_obj_align(btn1, LV_ALIGN_CENTER, 100, 50);                                      // ������ʾ������ƫ��50
 
        lv_obj_t* btn_label = lv_label_create(btn1);                                     // ����Button���󴴽�Label����
        if (btn_label != NULL)
        {
            lv_label_set_text_fmt(btn_label, "speed+1");                                     // ������ʾ����
            lv_obj_center(btn_label);                                                   // ���������ʾ
        }
    }
}



/**
 * @brief ������ť����
 */
void lvgl_button2(){
    /* �ڵ�ǰ�����д���һ����ť */
    lv_obj_t* btn = lv_btn_create(lv_scr_act());                                        // ����Button����
    if (btn != NULL)
    {
        lv_obj_set_size(btn, 80, 20);                                                   // ���ö����Ⱥ͸߶�
        // lv_obj_set_pos(btn, 90, 200);                                                // ���ð�ť��X��Y����
        lv_obj_add_event_cb(btn, btn2_event_callback, LV_EVENT_CLICKED, NULL);           // ���������CLICK�¼����¼�����ص�����
        lv_obj_align(btn, LV_ALIGN_CENTER, -100, 50);                                      // ������ʾ������ƫ��50
 
        lv_obj_t* btn_label = lv_label_create(btn);                                     // ����Button���󴴽�Label����
        if (btn_label != NULL)
        {
            lv_label_set_text(btn_label, "speed-1");                                     // ������ʾ����
            lv_obj_center(btn_label);                                                   // ���������ʾ
        }
    }
}





