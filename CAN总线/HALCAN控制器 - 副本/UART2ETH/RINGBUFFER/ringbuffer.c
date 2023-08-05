/*		循环队列解析数据			*/

	#include "ringbuffer.h"
	#include "delay.h"
	#include "stdio.h"
	#include "string.h"
	#include "canControl.h"
	#include "can.h"

/**
  * @brief  fifo初始化
  * @param  fifo: 实例
  * @param  buffer: fifo的缓冲区
  * @param  size: 缓冲区大小
  * @retval None
  */

void ringbuffer_init(ringbuffer_t *fifo, uint8_t *buffer, uint16_t size)
{
    fifo->buffer = buffer;
    fifo->size = size;
    fifo->in = 0;
    fifo->out = 0;
}

/**
  * @brief  获取已经使用的空间
  * @param  fifo: 实例
  * @retval uint16_t: 已使用个数
  */
uint16_t ringbuffer_getUsedSize(ringbuffer_t *fifo)
{
    if (fifo->in >= fifo->out)
        return (fifo->in - fifo->out);
    else
        return (fifo->size - fifo->out + fifo->in);
}

/**
  * @brief  获取未使用空间
  * @param  fifo: 实例
  * @retval uint16_t: 剩余个数
  */
uint16_t ringbuffer_getRemainSize(ringbuffer_t *fifo)
{
    return (fifo->size - ringbuffer_getUsedSize(fifo) - 1);
}


/**
  * @brief  FIFO是否为空
  * @param  fifo: 实例
  * @retval uint8_t: 1 为空 0 不为空（有数据）
  */
uint8_t ringbuffer_isEmpty(ringbuffer_t *fifo)
{
    return (fifo->in == fifo->out);
}

/**
  * @brief  发送数据到环形缓冲区（不检测剩余空间）
  * @param  fifo: 实例
  * @param  data: &#&
  * @param  len: &#&
  * @retval none
  */
void ringbuffer_in(ringbuffer_t *fifo, uint8_t *data, uint16_t len)
{
	int i = 0;
    for (i = 0; i < len; i++)
    {
        fifo->buffer[fifo->in] = data[i];
        fifo->in = (fifo->in + 1) % fifo->size;
    }
}


/**
  * @brief  发送数据到环形缓冲区(带剩余空间检测，空间不足发送失败)
  * @param  fifo: 实例
  * @param  data: &#&
  * @param  len: &#&
  * @retval uint8_t: 0 成功 1失败（空间不足）
  */
uint8_t ringbuffer_in_check(ringbuffer_t *fifo, uint8_t *data, uint16_t len)
{
    uint16_t remainsize = ringbuffer_getRemainSize(fifo);

    if (remainsize < len) //空间不足
        return 1;

    ringbuffer_in(fifo, data, len);

    return 0;
}


/**
  * @brief  从环形缓冲区读取数据
  * @param  fifo: 实例
  * @param  buf: 存放数组
  * @param  len: 存放数组长度
  * @retval uint16_t: 实际读取个数
  */
uint16_t ringbuffer_out(ringbuffer_t *fifo, uint8_t *buf, uint16_t len)
{
	int i = 0;
    uint16_t remainToread = ringbuffer_getUsedSize(fifo);

    if (remainToread > len) 
    {
        remainToread = len;
    }
		delay_ms(5);
    for (i = 0; i < remainToread; i++)
    {
        buf[i] = fifo->buffer[fifo->out];
        fifo->out = (fifo->out + 1) % fifo->size;
    }

    return remainToread;
}

uint8_t cmd[512]={0};
ringbuffer_t cmdqueue;  // 消息队列初始化


/***************************************************************
【函数名】 void queueinit(void)
【功  能】 队列初始化
【参数值】 无
【返回值】 无
****************************************************************/
void queueinit(void){
	
 	ringbuffer_init(&cmdqueue,cmd,512);  //队列初始化
	
}

void ring_Queue_in(uint16_t cmd_len, uint8_t *data){
		uint16_t a = 0;
		ringbuffer_in_check(&cmdqueue,data,cmd_len); // 入队
		a = ringbuffer_getUsedSize(&cmdqueue);
		printf("使用了%d\t\t",a);
		a = ringbuffer_getRemainSize(&cmdqueue);
		printf("还有%d\r\n",a);
}


const int CMDHEAD = 0xAA;
#define DEBUG 0   // 是否显示调试信息

uint8_t cmd_falg = 0;
uint8_t datam[9] = {0};  // 解析到的电机数据
uint8_t datas[9] = {0};  // 解析到的舵机数据
uint8_t datamflag = 0;
uint8_t datasflag = 0;

/***************************************************************
【函数名】 void find_cmd(ringbuffer_t *fifo)
【功  能】 从循环队列中找到数据
【参数值】 队列
【返回值】 无
****************************************************************/
void find_cmd(ringbuffer_t *fifo){
	u8 i = 0;
	uint8_t  val[1];
	uint8_t  vall[9];
		while( !ringbuffer_isEmpty(&cmdqueue))
		{
			switch (cmd_falg)
			{
				//帧头
				case 0:
					ringbuffer_out(&cmdqueue,val,1);
					#if DEBUG
					printf("val1 =  %d \r\n",val[0]);
					#endif	
					if(val[0] == 49)
					{
						cmd_falg = 1;
						//printf("发现消息头1\r\n");
					}
					else
					{
						cmd_falg = 0;
					}
				break;
				case 1:
					ringbuffer_out(&cmdqueue,val,1);
					#if DEBUG
					printf("val2 =  %d \r\n",val[0]);
					#endif
					if(val[0] == 55)
					{
						cmd_falg = 2;
						//printf("发现消息头2\r\n");
					}
					else
					{
						cmd_falg = 0;
					}
				break;
				case 2:
					ringbuffer_out(&cmdqueue,val,1);
					#if DEBUG
					printf("val3 =  %d \r\n",val[0]);
					#endif
					if(val[0] == 48)
					{
						cmd_falg = 3;
						printf("发现消息头3\r\n");
					}
					else
					{
						cmd_falg = 0;
					}
				break;
					//功能帧
				case 3:
					ringbuffer_out(&cmdqueue,val,1);
					#if DEBUG
					printf("val4 =  %d \r\n",val[0]);
					#endif
					if(val[0] == 109) // m
					{
						cmd_falg = 4;
						#if DEBUG
						printf("发现电机功能帧 m \r\n");
						#endif
					}
					else if(val[0] == 115) // s
					{
						cmd_falg = 5;
						#if DEBUG
						printf("发现舵机功能帧 m \r\n");
						#endif
					}
					else if(val[0] == 112)
					{
						cmd_falg = 6;
					}
					else 
						cmd_falg = 0;
				break;
					
					//电机
				case 4:
					ringbuffer_out(&cmdqueue,vall,9);
					
					for(i=0;i<9;i++){
						#if DEBUG
						printf("val5 =  %d \t",vall[i]);
						#endif
						datam[i] = vall[i]-48;
					}
					#if DEBUG
					for(i=0;i<9;i++)
					printf("得到电机数据%d /t",datam[i]);
					printf("\r\n");
					#endif
					datamflag = 1;
					cmd_falg = 0;
				break;
					
					//舵机
				case 5:
					ringbuffer_out(&cmdqueue,vall,9);
					
					for(i=0;i<9;i++){
						//printf("val6 =  %d \t",vall[i]);
						datas[i] = vall[i]-48;
					}
					#if DEBUG
					for(i=0;i<9;i++)
					printf("得到舵机数据%d /t",datas[i]);
					printf("\r\n");
					#endif
					datasflag = 1;
					cmd_falg = 0;
				break;
					// 停止
				case 6:
					ringbuffer_out(&cmdqueue,vall,4);
					#if DEBUG
					printf("val7 =  %d \t",vall[i]);
					if(vall[0] == 's' && vall[1] == 't' && vall[2] == 'o' && vall[3] == 'p' )
					{
							printf("stop");
					}
					printf("\r\n");
					#endif
					cmd_falg = 0;
				break;
			}
		}
}

volatile static u8 dir = 0;
volatile static float dis = 0.0f;
volatile static float V = 0.0f;

/***************************************************************
【函数名】 void control_CAN(void)
【功  能】 根据结束到的命令发送CAN
【参数值】 无
【返回值】 无
****************************************************************/
void control_CAN(void){
	
	// 解析命令
	find_cmd(&cmdqueue);
	// CAN电机
	if(datamflag == 1)
	{

		dir = datam[0];
		dis = datam[1] * 10 + datam[2] + datam[3] * 0.1 + datam[4] * 0.01;
		V   = datam[5] * 10 + datam[6] + datam[7] * 0.1 + datam[8] * 0.01;
		
		#if DEBUG
		printf("dir = %d,dis = %f,V = %f \r\n",dir, dis, V);
		printf("CAN电机发送\r\n");
		#endif
		sendMotorCommand(dir,dis,V,0xEE00); // 电机
		datamflag = 0;
		memset(datam,0,sizeof(datam));
	}

	//CAN舵机
	if(datasflag == 1)
	{
		dis = datas[0] * 100 + datas[1] * 10 + datas[2];
		dir = datas[3]; // 代表几个255了
		V = 1.1;
		#if DEBUG
		printf("CAN舵机发送\r\n");
		printf("dis = %f \r\n", dis);
		printf("dir = %d\r\n",dir);
		#endif
		sendMotorCommand(dir,dis,V,0x1200);  // 舵机
		datasflag = 0;
		memset(datas,0,sizeof(datas));
	}
}


/*******************************END OF FILE************************************/




