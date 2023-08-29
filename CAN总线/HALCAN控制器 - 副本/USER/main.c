#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "usart3.h"
#include "can.h"
#include "timer.h"
#include "canControl.h"
#include "string.h"
#include "atk_u2e.h"
#include "MatchingCommands.h"
#include "ringbuffer.h"



char *Modbuf[]={
    "UDP_SERVER",
    "UDP_CLIENT",
    "TCP_Server",
    "TCP_Client",
    "TCP_CLOUD ",
    "UDP_MULTICAST",
};


char *test_buf[]={
    "Data from UDP_Server mode",
    "Data from UDP_Client mode",
    "Data from TCP_Server mode",
    "Data from TCP_Client mode",
    "CLOUD.ALIENTEK.COM",
    "Data from 233.0.0.6",
};


uint8_t g_data_rxbuf[256];       /* 用于接收数据的缓存 */


void Nextval(char T[],int *next)
{
	int lenT=strlen(T);
	int k=-1;//前缀 
	int j=0;//后缀 
	next[0]=-1; 
	
	while(j<lenT)
	{
		if(k==-1||T[j]==T[k])
		{
			j++;
			k++;
			//较之前next数组求法，改动在下面4行
			if(T[j]!=T[k])
			{
				next[j]=k;//之前只有这一行
			}
			else
			{
				next[j]=next[k];/*为不能出现T[j] = T[ next[j ]]，
				                  所以当出现时连续进行两次回溯，k = next[next[k]]  */ 
			 } 
		}
		else 
		{
			k=next[k];
		 } 
	}
}

int KMP(uint8_t S[],uint8_t T[])
{
	int i=0,j=0,lenS,lenT;
	int next[100];
	
	lenS=strlen((const char *)S);
	lenT=strlen((const char *)T);
	
	Nextval((char*)T,next);
	
	while(i<lenS&&j<lenT)
	{
		if(j==-1||S[i]==T[j])
		{
			i++;
			j++;
		}
		else
		{ 
			j=next[j];
		}
	}
	 
	if(j==lenT)
	{
	    printf("Match succeed!\n");
	 	return i-j;
	} 
	else
	{
		printf("Match failed!\n");
		return -1;
  }
}

//解析方向
u8 parsingDirData(uint8_t arr[]){
	
	u8 data = 2;
	
	if((arr[0] - '0') == 1)
		return 1;
	else if ((arr[0] - '0') == 0)
		return 0;
	
	return data;
}

//解析距离
float parsingDisData(uint8_t arr[]){
	
	float data;
	float shiwei = 0; // 十位
	float gewei = 0;  // 个位
	float hou1 = 0; // 小数点后一位
	float hou2 = 0;  // 小数点后两位
	
	// 距离数据为
	if((arr[1] - '0') == 0){
		gewei = arr[2] - '0';
		hou1 = arr[3] - '0';
		hou2 = arr[4] - '0';
	}
	
	//最大一次不超过10.00
	else if((arr[1] - '0') != 0){
		shiwei = arr[1] - '0';
		gewei = arr[2] - '0';
		hou1 = arr[3] - '0';
		hou2 = arr[4] - '0';
	}
	printf("shiwei = %f, gewei = %f, hou1 = %f,hou2 = %f", shiwei, gewei, hou1, hou2);
	// 距离
	data = shiwei*10 + gewei + (hou1 / 10.0f) + (hou2 / 100.0f);

	return data;
}

//解析速度
float parsingVData(uint8_t arr[]){
	
	float data;
	float shiwei = 0; // 十位
	float gewei = 0;  // 个位
	float hou1 = 0; // 小数点后一位
	float hou2 = 0;  // 小数点后两位
	//0 < speed < 10   0 < distance < 50
	
	// 速度数据为
	if((arr[5] - '0') == 0){
		gewei = arr[6] - '0';
		hou1 = arr[7] - '0';
		hou2 = arr[8] - '0';
	}
	
	//最大一次不超过10.00
	else if((arr[5] - '0') != 0){
		shiwei = arr[5] - '0';
		gewei = arr[6] - '0';
		hou1 = arr[7] - '0';
		hou2 = arr[8] - '0';
	}
	
	// 速度
	data = shiwei*10 + gewei + (hou1 / 10.0f) + (hou2 / 100.0f);
	
	return data;
	
}


int main(void)
{

	int rx_len = 0;
	uint8_t ok[]={"ok\r\n"}; 	// 应答信号
  HAL_Init();                    	 	//初始化HAL库
  Stm32_Clock_Init(RCC_PLL_MUL9);   	//设置时钟,72M
	delay_init(72);               		//初始化延时函数
	uart_init(115200);					//初始化串口
	delay_ms(100);
	//LED_Init();							//初始化LED
	//KEY_Init();							//初始化按键
 	CAN1_Mode_Init(CAN_SJW_1TQ,CAN_BS2_8TQ,CAN_BS1_9TQ,4,CAN_MODE_NORMAL); //CAN初始化,波特率500Kbps      
	CAN_Config();
	TIM3_Init(100-1,72-1);    // 每秒计数10000次 can消息计数用
	printf("ko");
	u2e_config_init(U2E_TCP_CLIENT); /* 选择模式 */
	printf("ok");
	delay_ms(100);
	queueinit();
	printf("初始化完成");
//	a = ringbuffer_getUsedSize(&cmdqueue);
//	printf("指令存储控件大小使用了%d\t\t",a);
//	a = ringbuffer_getRemainSize(&cmdqueue);
//	printf("还剩下%d\r\n",a);

	while (1)
	{
		//delay_ms(1000);
			/* 透传功能下，从队列中把所有数据提取出来*/
			while (res_usart3_revdata())
			{
					usart3_getRxData(&g_data_rxbuf[rx_len++], 1); /* 从串口缓存中读一个字节 */

					if (rx_len >= U2E_RX_CMD_BUF_SIZE)
					{
							rx_len = 0; /*接收应答数据超长，丢弃部分数据*/
					}
			}

//   第二代数据解析方法
			if(rx_len>0)
			{
				usart1_send_data(g_data_rxbuf,rx_len);       /* 转发到串口1 */
				printf("\r\n");
				// 
				ring_Queue_in(rx_len,g_data_rxbuf);
				control_CAN();  // 主要逻辑都在这
				// 清除标志位
				rx_len = 0;
				usart3_send_data(ok,2);      /* 发送ok数据到服务器 */
			}

// 			最初的数据解析方法
//			if(rx_len>0)
//			{
//				//printf("len = %d",rx_len);
//				if (rx_len == 13) {  // 舵机数据 --->  控制舵机
//					
//					for(i=0;i<8;i++){ 
//						if(onecp[i]==g_data_rxbuf[i])
//							js++;
//					}
//					
//					if (js == 8){ // 如果帧头匹配成功
//						// 开始解析舵机数据  匹配功能帧表示 “s”
//						if(g_data_rxbuf[8] == 's'){
//							for ( i = 0; i < 4; i++)
//								three[i] = g_data_rxbuf[i + 8];  // 得到舵机数据
//						}
//						js = 0;
//					}
//					if (js == 0){
//						// 发送到舵机
//						//ControlEntryFunction((u8)parsingDirData(three), parsingDisData(three), parsingVData(three));
//					}
//				}

//				else if (rx_len == 18){  // 电机数据  ----> 控制电机
//					for(i=0;i<8;i++){ 
//						if(onecp[i]==g_data_rxbuf[i])
//							js++;
//					}
//					
//					if (js == 8){ // 如果帧头匹配成功
//						// 开始解析舵机数据  匹配功能帧表示 “s”
//						if(g_data_rxbuf[8] == 's'){
//							for ( i = 0; i < 9; i++)
//								two[i] = g_data_rxbuf[i + 8];  // 得到舵机数据
//						}
//						js = 0;
//					}
//					if (js == 0){
//						// 发送到舵机
//						//ControlEntryFunction((u8)parsingDirData(two), parsingDisData(two), parsingVData(two));
//					}
//				}
//				
//				else if (rx_len == 23){ // 全部数据
//				
//					target=KMP(g_data_rxbuf,fun1);
//					target1=KMP(g_data_rxbuf,fun2);
//					
//					// 校验帧头
//					for(i=0;i<8;i++){ 
//						one[i]=g_data_rxbuf[i];
//					}

//					for(i=0;i<8;i++){  
//						if(one[i]==onecp[i])
//							js++;
//					}
//					if(js==8)
//					{
//						target=KMP(g_data_rxbuf,fun1);
//						target1=KMP(g_data_rxbuf,fun2);
//						
//						printf("target = %d   %d ", target,target1);
//						
//						// 校验电机
//						if(target == 8 && target1 == 18)
//						{
//							for(i=target+1;i<target1;i++)
//							{
//								two[i-9] = g_data_rxbuf[i];
//							}
//						}
//						printf("电机数据 = %s\r\n", two);
//						// 校验舵机
//						for(i=target1+1;i<target1+5;i++){
//							three[i-target1-1] = g_data_rxbuf[i];
//						}
//						for(i =0;i<4;i++)
//							three[i] = three[i];
//						
//						//发送校验成功标记
//						usart3_send_data(ok,2);      /* 发送数据到服务器 */
//						//ControlEntryFunction(1, 13.14f, 9.99f);
//						printf("dir = %d, dis = %f, V = %f", parsingDirData(two), parsingDisData(two), parsingVData(two) );
//						
//						//CAN执行
//						//ControlEntryFunction((u8)parsingDirData(two), parsingDisData(two), parsingVData(two));
//					}
//						rx_len = 0;
//				}
//					//usart1_send_data(g_data_rxbuf,rx_len);       /* 转发到串口1 */
//						//printf("\r\n");
//			}
	}
}

