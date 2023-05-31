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


__IO uint32_t g_set_speed  = 400;//* radtor;    //圈/s    //600 * 5.729578;   /* 最大速度 单位为 0.1rad/sec */   //0.1弧度每秒  1rad = 5.729578
__IO uint32_t g_step_accel = 20;//* radtor;               /* 加速度 单位为 0.1rad/sec^2 */  //0.1弧度每2次方秒
__IO uint32_t g_step_decel = 20;//* radtor;               /* 减速度 单位为 0.1rad/sec^2 */  //0.1弧度每2次方秒
__IO uint16_t g_step_angle = 50;             /* 设置的步数*/   //对应圈数 8细分下


extern __IO uint32_t g_add_pulse_count;     /* 脉冲个数累计*/


 
//清空屏幕并在右上角显示"RST"
void Load_Drow_Dialog(void)
{
	LCD_Clear(WHITE);	//清屏   
 	POINT_COLOR=BLUE;	//设置字体为蓝色 
	LCD_ShowString(lcddev.width-24,0,200,16,16,"RST");//显示清屏区域
  	POINT_COLOR=RED;	//设置画笔蓝色 
}
////////////////////////////////////////////////////////////////////////////////
//电容触摸屏专有部分
//画水平线
//x0,y0:坐标
//len:线长度
//color:颜色
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
	if(len==0)return;
	LCD_Fill(x0,y0,x0+len-1,y0,color);	
}
//画实心圆
//x0,y0:坐标
//r:半径
//color:颜色
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
//两个数之差的绝对值 
//x1,x2：需取差值的两个数
//返回值：|x1-x2|
u16 my_abs(u16 x1,u16 x2)
{			 
	if(x1>x2)return x1-x2;
	else return x2-x1;
}  
//画一条粗线
//(x1,y1),(x2,y2):线条的起始坐标
//size：线条的粗细程度
//color：线条的颜色
void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	if(x1<size|| x2<size||y1<size|| y2<size)return; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		gui_fill_circle(uRow,uCol,size,color);//画点 
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
 //5个触控点的颜色												 
const u16 POINT_COLOR_TBL[CT_MAX_TOUCH]={RED,GREEN,BLUE,BROWN,GRED};  
//电阻触摸屏测试函数
void rtp_test(void)
{
	u8 key;
	u8 i=0;
	while(1)
	{
	 	key=KEY_Scan(0);
		tp_dev.scan(0);
		if(tp_dev.sta&TP_PRES_DOWN)			//触摸屏被按下
		{
		 	if(tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height)
			{	
				if(tp_dev.x[0]>(lcddev.width-24)&&tp_dev.y[0]<16) Load_Drow_Dialog();//清除
				else TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],RED);		//画图	  			   
			}
		}else delay_ms(10);	//没有按键按下的时候 	    
		if(key==KEY0_PRES)	//KEY0按下,则执行校准程序
		{
			LCD_Clear(WHITE);//清屏
		    TP_Adjust();  	//屏幕校准  
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
		len=USART_RX_STA2&0x3fff;//得到此次接收到的数据长度
		for(t=0;t<len;t++)
		{
			USART_SendData(USART1, USART_RX_BUF2[t]);//向串口1发送数据
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
		}
		if(USART_RX_BUF2[0] == 'g' && USART_RX_BUF2[1] == 'o'){
			LED1 =~ LED1;
			create_t_ctrl_param(SPR*g_step_angle, g_step_accel, g_step_decel, g_set_speed);
			g_add_pulse_count=0;
			printf("前进");
		}
		if(USART_RX_BUF2[0] == 'b' && USART_RX_BUF2[1] == 'a' && USART_RX_BUF2[2] == 'c' && USART_RX_BUF2[3] == 'k'){
			LED1 =~ LED1;
			create_t_ctrl_param(SPR*g_step_angle*2, g_step_accel*1.2, g_step_decel*1.2, g_set_speed*1.3);
			g_add_pulse_count=0;
			printf("后退");
		}
		if(USART_RX_BUF2[0] == 'l' && USART_RX_BUF2[1] == 'e' && USART_RX_BUF2[2] == 'f' && USART_RX_BUF2[3] == 't'){
			LED0 =~ LED0;
			create_t_ctrl_param(SPR*g_step_angle*3, g_step_accel*1.3, g_step_decel*1.3, g_set_speed*1.6);
			g_add_pulse_count=0;
			printf("向左");
		}
		if(USART_RX_BUF2[0] == 'r' && USART_RX_BUF2[1] == 'i' && USART_RX_BUF2[2] == 'g' && USART_RX_BUF2[3] == 'h' && USART_RX_BUF2[4] == 't'){
			LED0 =~ LED0;
			create_t_ctrl_param(SPR*g_step_angle*4, g_step_accel*1.4, g_step_decel*1.4, g_set_speed*1.9);
			g_add_pulse_count=0;
			printf("向右");
		}
		USART_RX_STA2=0;
	}
}


//电容触摸屏测试函数
void ctp_test(void)
{
	
	u8 t=0; 	    
 	u16 lastpos[5][2];		//记录最后一次的数据 
	
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
					LCD_ShowString(80,50,200,16,16,(u8*)"x = ");	 //横向 0 - 480 实际 1 - 479
					LCD_ShowString(80,70,200,16,16,(u8*)"y = ");	 //纵向 0 - 800 实际 1 - 799
					LCD_ShowxNum(100,50,tp_dev.x[0],3,16,0);
					LCD_ShowxNum(100,70,tp_dev.y[0],3,16,0);
					if(lastpos[t][0]==0XFFFF)
					{
						lastpos[t][0] = tp_dev.x[t];
						lastpos[t][1] = tp_dev.y[t];
					}
					lcd_draw_bline(lastpos[t][0],lastpos[t][1],tp_dev.x[t],tp_dev.y[t],2,POINT_COLOR_TBL[t]);//画线
					lastpos[t][0]=tp_dev.x[t];
					lastpos[t][1]=tp_dev.y[t];
					if(tp_dev.x[t]>(lcddev.width-24)&&tp_dev.y[t]<16)
					{
						Load_Drow_Dialog();//清除
					}
				}
			}else lastpos[t][0]=0XFFFF;
		}
	}
}



 int main(void)
 {	 		    
	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	
	uart_init(115200);
	uart2_init(115200);  //串口接收
	 
 	LED_Init();			     //LED端口初始化
	LCD_Init();	
	lv_init();
	lv_port_disp_init();  // lvgl显示接口初始化,放在lv_init()的后面
	lv_port_indev_init(); // lvgl输入接口初始化,放在lv_init()的后面
	KEY_Init();	 	
	time3_initer(0xffff, 35); //步进电机定时器
	TIM5_Int_Init(1,7199);  //提供lvgl心跳
 	tp_dev.init(); 
	//POIT_COLOR=BRRED;
	
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
			vTaskDelay(5);
			lv_timer_handler(); // lvgl的事务处理
	}

}



