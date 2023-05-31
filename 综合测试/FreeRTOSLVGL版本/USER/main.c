#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "timer.h"
#include "usart.h"	 
#include "24cxx.h"
#include "w25qxx.h"
#include "touch.h"
#include "motorT.h"
#include "Servos.h"
#include "FreeRTOS.h"
#include "task.h"




#define radtor  0.01591549


__IO uint32_t g_set_speed  = 400;//* radtor;    //Ȧ/s    //600 * 5.729578;   /* ����ٶ� ��λΪ 0.1rad/sec */   //0.1����ÿ��  1rad = 5.729578
__IO uint32_t g_step_accel = 20;//* radtor;               /* ���ٶ� ��λΪ 0.1rad/sec^2 */  //0.1����ÿ2�η���
__IO uint32_t g_step_decel = 20;//* radtor;               /* ���ٶ� ��λΪ 0.1rad/sec^2 */  //0.1����ÿ2�η���
__IO uint16_t g_step_angle = 50;             /* ���õĲ���*/   //��ӦȦ�� 8ϸ����


extern __IO uint32_t g_add_pulse_count;     /* ��������ۼ�*/


 
//�����Ļ�������Ͻ���ʾ"RST"
void Load_Drow_Dialog(void)
{
	LCD_Clear(WHITE);	//����   
 	POINT_COLOR=BLUE;	//��������Ϊ��ɫ 
	LCD_ShowString(lcddev.width-24,0,200,16,16,"RST");//��ʾ��������
  	POINT_COLOR=RED;	//���û�����ɫ 
}
////////////////////////////////////////////////////////////////////////////////
//���ݴ�����ר�в���
//��ˮƽ��
//x0,y0:����
//len:�߳���
//color:��ɫ
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
	if(len==0)return;
	LCD_Fill(x0,y0,x0+len-1,y0,color);	
}
//��ʵ��Բ
//x0,y0:����
//r:�뾶
//color:��ɫ
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color)
{											  
	u32 i;
	u32 imax = ((u32)r*707)/1000+1;
	u32 sqmax = (u32)r*(u32)r+(u32)r/2;
	u32 x=r;
	gui_draw_hline(x0-r,y0,2*r,color);
	for (i=1;i<=imax;i++) 
	{
		if ((i*i+x*x)>sqmax)// draw lines from outside  
		{
 			if (x>imax) 
			{
				gui_draw_hline (x0-i+1,y0+x,2*(i-1),color);
				gui_draw_hline (x0-i+1,y0-x,2*(i-1),color);
			}
			x--;
		}
		// draw lines from inside (center)  
		gui_draw_hline(x0-x,y0+i,2*x,color);
		gui_draw_hline(x0-x,y0-i,2*x,color);
	}
}  
//������֮��ľ���ֵ 
//x1,x2����ȡ��ֵ��������
//����ֵ��|x1-x2|
u16 my_abs(u16 x1,u16 x2)
{			 
	if(x1>x2)return x1-x2;
	else return x2-x1;
}  
//��һ������
//(x1,y1),(x2,y2):��������ʼ����
//size�������Ĵ�ϸ�̶�
//color����������ɫ
void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	if(x1<size|| x2<size||y1<size|| y2<size)return; 
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		gui_fill_circle(uRow,uCol,size,color);//���� 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}   

////////////////////////////////////////////////////////////////////////////////
 //5�����ص����ɫ												 
const u16 POINT_COLOR_TBL[CT_MAX_TOUCH]={RED,GREEN,BLUE,BROWN,GRED};  
//���败�������Ժ���
void rtp_test(void)
{
	u8 key;
	u8 i=0;
	while(1)
	{
	 	key=KEY_Scan(0);
		tp_dev.scan(0);
		if(tp_dev.sta&TP_PRES_DOWN)			//������������
		{
		 	if(tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height)
			{	
				if(tp_dev.x[0]>(lcddev.width-24)&&tp_dev.y[0]<16) Load_Drow_Dialog();//���
				else TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],RED);		//��ͼ	  			   
			}
		}else delay_ms(10);	//û�а������µ�ʱ�� 	    
		if(key==KEY0_PRES)	//KEY0����,��ִ��У׼����
		{
			LCD_Clear(WHITE);//����
		    TP_Adjust();  	//��ĻУ׼  
			Load_Drow_Dialog();
		}
		i++;
		if(i%20==0)LED0=!LED0;
	}
}

void usartReceData(){
	u8 len;
	u16 t;
	if(USART_RX_STA2&0x8000)
	{
		len=USART_RX_STA2&0x3fff;//�õ��˴ν��յ������ݳ���
		for(t=0;t<len;t++)
		{
			USART_SendData(USART1, USART_RX_BUF2[t]);//�򴮿�1��������
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		}
		if(USART_RX_BUF2[0] == 'g' && USART_RX_BUF2[1] == 'o'){
			LED1 =~ LED1;
			create_t_ctrl_param(SPR*g_step_angle, g_step_accel, g_step_decel, g_set_speed);
			g_add_pulse_count=0;
			printf("ǰ��");
		}
		if(USART_RX_BUF2[0] == 'b' && USART_RX_BUF2[1] == 'a' && USART_RX_BUF2[2] == 'c' && USART_RX_BUF2[3] == 'k'){
			LED1 =~ LED1;
			create_t_ctrl_param(SPR*g_step_angle*2, g_step_accel*1.2, g_step_decel*1.2, g_set_speed*1.3);
			g_add_pulse_count=0;
			printf("����");
		}
		if(USART_RX_BUF2[0] == 'l' && USART_RX_BUF2[1] == 'e' && USART_RX_BUF2[2] == 'f' && USART_RX_BUF2[3] == 't'){
			LED0 =~ LED0;
			create_t_ctrl_param(SPR*g_step_angle*3, g_step_accel*1.3, g_step_decel*1.3, g_set_speed*1.6);
			g_add_pulse_count=0;
			printf("����");
		}
		if(USART_RX_BUF2[0] == 'r' && USART_RX_BUF2[1] == 'i' && USART_RX_BUF2[2] == 'g' && USART_RX_BUF2[3] == 'h' && USART_RX_BUF2[4] == 't'){
			LED0 =~ LED0;
			create_t_ctrl_param(SPR*g_step_angle*4, g_step_accel*1.4, g_step_decel*1.4, g_set_speed*1.9);
			g_add_pulse_count=0;
			printf("����");
		}
		USART_RX_STA2=0;
	}
}


//���ݴ��������Ժ���
void ctp_test(void)
{
	
	u8 t=0; 	    
 	u16 lastpos[5][2];		//��¼���һ�ε����� 
	
	while(1)
	{
		if(KEY_Scan(1) == 1){
			create_t_ctrl_param(SPR*g_step_angle, g_step_accel, g_step_decel, g_set_speed);
			g_add_pulse_count=0;
		}
		if(KEY_Scan(1) == 2){
			LED0 = 1;
		}
		if(KEY_Scan(1) == 3){
				LED0 = 0;
		}
		tp_dev.scan(0);
		for(t=0;t<CT_MAX_TOUCH;t++)
		{
			if((tp_dev.sta)&(1<<t))
			{
				if(tp_dev.x[t]<lcddev.width&&tp_dev.y[t]<lcddev.height)
				{
					LCD_ShowString(80,50,200,16,16,(u8*)"x = ");	 //���� 0 - 480 ʵ�� 1 - 479
					LCD_ShowString(80,70,200,16,16,(u8*)"y = ");	 //���� 0 - 800 ʵ�� 1 - 799
					LCD_ShowxNum(100,50,tp_dev.x[0],3,16,0);
					LCD_ShowxNum(100,70,tp_dev.y[0],3,16,0);
					if(lastpos[t][0]==0XFFFF)
					{
						lastpos[t][0] = tp_dev.x[t];
						lastpos[t][1] = tp_dev.y[t];
					}
					lcd_draw_bline(lastpos[t][0],lastpos[t][1],tp_dev.x[t],tp_dev.y[t],2,POINT_COLOR_TBL[t]);//����
					lastpos[t][0]=tp_dev.x[t];
					lastpos[t][1]=tp_dev.y[t];
					if(tp_dev.x[t]>(lcddev.width-24)&&tp_dev.y[t]<16)
					{
						Load_Drow_Dialog();//���
					}
				}
			}else lastpos[t][0]=0XFFFF;
		}
	}
}



 int main(void)
 {	 		    
	delay_init();	    	 //��ʱ������ʼ��	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	
	uart_init(115200);
	uart2_init(115200);  //���ڽ���
	 
 	LED_Init();			     //LED�˿ڳ�ʼ��
	LCD_Init();	
	lv_init();
	lv_port_disp_init();  // lvgl��ʾ�ӿڳ�ʼ��,����lv_init()�ĺ���
	lv_port_indev_init(); // lvgl����ӿڳ�ʼ��,����lv_init()�ĺ���
	KEY_Init();	 	
	time3_initer(0xffff, 35); //���������ʱ��
	TIM5_Int_Init(1,7199);  //�ṩlvgl����
 	tp_dev.init(); 
	//POIT_COLOR=BRRED;
	
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
			vTaskDelay(5);
			lv_timer_handler(); // lvgl��������
	}

}



