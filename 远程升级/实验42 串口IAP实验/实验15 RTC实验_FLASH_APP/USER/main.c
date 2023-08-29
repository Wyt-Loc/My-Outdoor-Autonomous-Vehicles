#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	
#include "usmart.h"	 
#include "rtc.h" 


 int main(void)
 {	 
 	u8 t=0;	
	
	SCB->VTOR = FLASH_BASE | 0x10000; /* Vector Table Relocation in Internal FLASH. */
	
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
 	LED_Init();			     //LED�˿ڳ�ʼ��
	LCD_Init();
	usmart_dev.init(SystemCoreClock/1000000);	//��ʼ��USMART	
	RTC_Init();	  			//RTC��ʼ��
	POINT_COLOR=RED;//��������Ϊ��ɫ 
	LCD_ShowString(60,50,200,16,16,(u8*)"ELITE STM32");	
	LCD_ShowString(60,70,200,16,16,(u8*)"RTC TEST");	
	LCD_ShowString(60,90,200,16,16,(u8*)"ATOM@ALIENTEK");
	LCD_ShowString(60,110,200,16,16,(u8*)"2012/9/7");		
	//��ʾʱ��
	POINT_COLOR=BLUE;//��������Ϊ��ɫ
	LCD_ShowString(60,130,200,16,16,(u8*)"    -  -  ");	   
	LCD_ShowString(60,162,200,16,16,(u8*)"  :  :  ");		    
	while(1)
	{
		if(t!=calendar.sec)
		{
			t=calendar.sec;
			LCD_ShowNum(60,130,calendar.w_year,4,16);									  
			LCD_ShowNum(100,130,calendar.w_month,2,16);									  
			LCD_ShowNum(124,130,calendar.w_date,2,16);	 
			LCD_ShowString(60,182,200,16,16,(u8*)"Hello World");
			switch(calendar.week)
			{
				case 0:
					LCD_ShowString(60,148,200,16,16,(u8*)"Sunday   ");
					break;
				case 1:
					LCD_ShowString(60,148,200,16,16,(u8*)"Monday   ");
					break;
				case 2:
					LCD_ShowString(60,148,200,16,16,(u8*)"Tuesday  ");
					break;
				case 3:
					LCD_ShowString(60,148,200,16,16,(u8*)"Wednesday");
					break;
				case 4:
					LCD_ShowString(60,148,200,16,16,(u8*)"Thursday ");
					break;
				case 5:
					LCD_ShowString(60,148,200,16,16,(u8*)"Friday   ");
					break;
				case 6:
					LCD_ShowString(60,148,200,16,16,(u8*)"Saturday ");
					break;  
			}
			LCD_ShowNum(60,162,calendar.hour,2,16);									  
			LCD_ShowNum(84,162,calendar.min,2,16);									  
			LCD_ShowNum(108,162,calendar.sec,2,16);
			LED0=!LED0;
		}
		delay_ms(10);								  
	};  
 }


