/*		ѭ�����н�������			*/

	#include "ringbuffer.h"
	#include "delay.h"
	#include "stdio.h"
	#include "string.h"
	#include "canControl.h"
	#include "can.h"

/**
  * @brief  fifo��ʼ��
  * @param  fifo: ʵ��
  * @param  buffer: fifo�Ļ�����
  * @param  size: ��������С
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
  * @brief  ��ȡ�Ѿ�ʹ�õĿռ�
  * @param  fifo: ʵ��
  * @retval uint16_t: ��ʹ�ø���
  */
uint16_t ringbuffer_getUsedSize(ringbuffer_t *fifo)
{
    if (fifo->in >= fifo->out)
        return (fifo->in - fifo->out);
    else
        return (fifo->size - fifo->out + fifo->in);
}

/**
  * @brief  ��ȡδʹ�ÿռ�
  * @param  fifo: ʵ��
  * @retval uint16_t: ʣ�����
  */
uint16_t ringbuffer_getRemainSize(ringbuffer_t *fifo)
{
    return (fifo->size - ringbuffer_getUsedSize(fifo) - 1);
}


/**
  * @brief  FIFO�Ƿ�Ϊ��
  * @param  fifo: ʵ��
  * @retval uint8_t: 1 Ϊ�� 0 ��Ϊ�գ������ݣ�
  */
uint8_t ringbuffer_isEmpty(ringbuffer_t *fifo)
{
    return (fifo->in == fifo->out);
}

/**
  * @brief  �������ݵ����λ������������ʣ��ռ䣩
  * @param  fifo: ʵ��
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
  * @brief  �������ݵ����λ�����(��ʣ��ռ��⣬�ռ䲻�㷢��ʧ��)
  * @param  fifo: ʵ��
  * @param  data: &#&
  * @param  len: &#&
  * @retval uint8_t: 0 �ɹ� 1ʧ�ܣ��ռ䲻�㣩
  */
uint8_t ringbuffer_in_check(ringbuffer_t *fifo, uint8_t *data, uint16_t len)
{
    uint16_t remainsize = ringbuffer_getRemainSize(fifo);

    if (remainsize < len) //�ռ䲻��
        return 1;

    ringbuffer_in(fifo, data, len);

    return 0;
}


/**
  * @brief  �ӻ��λ�������ȡ����
  * @param  fifo: ʵ��
  * @param  buf: �������
  * @param  len: ������鳤��
  * @retval uint16_t: ʵ�ʶ�ȡ����
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
ringbuffer_t cmdqueue;  // ��Ϣ���г�ʼ��


/***************************************************************
���������� void queueinit(void)
����  �ܡ� ���г�ʼ��
������ֵ�� ��
������ֵ�� ��
****************************************************************/
void queueinit(void){
	
 	ringbuffer_init(&cmdqueue,cmd,512);  //���г�ʼ��
	
}

void ring_Queue_in(uint16_t cmd_len, uint8_t *data){
		uint16_t a = 0;
		ringbuffer_in_check(&cmdqueue,data,cmd_len); // ���
		a = ringbuffer_getUsedSize(&cmdqueue);
		printf("ʹ����%d\t\t",a);
		a = ringbuffer_getRemainSize(&cmdqueue);
		printf("����%d\r\n",a);
}


const int CMDHEAD = 0xAA;
#define DEBUG 0   // �Ƿ���ʾ������Ϣ

uint8_t cmd_falg = 0;
uint8_t datam[9] = {0};  // �������ĵ������
uint8_t datas[9] = {0};  // �������Ķ������
uint8_t datamflag = 0;
uint8_t datasflag = 0;

/***************************************************************
���������� void find_cmd(ringbuffer_t *fifo)
����  �ܡ� ��ѭ���������ҵ�����
������ֵ�� ����
������ֵ�� ��
****************************************************************/
void find_cmd(ringbuffer_t *fifo){
	u8 i = 0;
	uint8_t  val[1];
	uint8_t  vall[9];
		while( !ringbuffer_isEmpty(&cmdqueue))
		{
			switch (cmd_falg)
			{
				//֡ͷ
				case 0:
					ringbuffer_out(&cmdqueue,val,1);
					#if DEBUG
					printf("val1 =  %d \r\n",val[0]);
					#endif	
					if(val[0] == 49)
					{
						cmd_falg = 1;
						//printf("������Ϣͷ1\r\n");
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
						//printf("������Ϣͷ2\r\n");
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
						printf("������Ϣͷ3\r\n");
					}
					else
					{
						cmd_falg = 0;
					}
				break;
					//����֡
				case 3:
					ringbuffer_out(&cmdqueue,val,1);
					#if DEBUG
					printf("val4 =  %d \r\n",val[0]);
					#endif
					if(val[0] == 109) // m
					{
						cmd_falg = 4;
						#if DEBUG
						printf("���ֵ������֡ m \r\n");
						#endif
					}
					else if(val[0] == 115) // s
					{
						cmd_falg = 5;
						#if DEBUG
						printf("���ֶ������֡ m \r\n");
						#endif
					}
					else if(val[0] == 112)
					{
						cmd_falg = 6;
					}
					else 
						cmd_falg = 0;
				break;
					
					//���
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
					printf("�õ��������%d /t",datam[i]);
					printf("\r\n");
					#endif
					datamflag = 1;
					cmd_falg = 0;
				break;
					
					//���
				case 5:
					ringbuffer_out(&cmdqueue,vall,9);
					
					for(i=0;i<9;i++){
						//printf("val6 =  %d \t",vall[i]);
						datas[i] = vall[i]-48;
					}
					#if DEBUG
					for(i=0;i<9;i++)
					printf("�õ��������%d /t",datas[i]);
					printf("\r\n");
					#endif
					datasflag = 1;
					cmd_falg = 0;
				break;
					// ֹͣ
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
���������� void control_CAN(void)
����  �ܡ� ���ݽ������������CAN
������ֵ�� ��
������ֵ�� ��
****************************************************************/
void control_CAN(void){
	
	// ��������
	find_cmd(&cmdqueue);
	// CAN���
	if(datamflag == 1)
	{

		dir = datam[0];
		dis = datam[1] * 10 + datam[2] + datam[3] * 0.1 + datam[4] * 0.01;
		V   = datam[5] * 10 + datam[6] + datam[7] * 0.1 + datam[8] * 0.01;
		
		#if DEBUG
		printf("dir = %d,dis = %f,V = %f \r\n",dir, dis, V);
		printf("CAN�������\r\n");
		#endif
		sendMotorCommand(dir,dis,V,0xEE00); // ���
		datamflag = 0;
		memset(datam,0,sizeof(datam));
	}

	//CAN���
	if(datasflag == 1)
	{
		dis = datas[0] * 100 + datas[1] * 10 + datas[2];
		dir = datas[3]; // ������255��
		V = 1.1;
		#if DEBUG
		printf("CAN�������\r\n");
		printf("dis = %f \r\n", dis);
		printf("dir = %d\r\n",dir);
		#endif
		sendMotorCommand(dir,dis,V,0x1200);  // ���
		datasflag = 0;
		memset(datas,0,sizeof(datas));
	}
}


/*******************************END OF FILE************************************/




