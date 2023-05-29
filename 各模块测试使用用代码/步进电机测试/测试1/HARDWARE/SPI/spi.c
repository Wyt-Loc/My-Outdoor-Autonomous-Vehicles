#include "spi.h"
#include "delay.h"
#include "stdio.h"
#include "led.h"
#include "timer.h"
void SPI1_Init(void);			 //��ʼ��SPI��
void SPI1_SetSpeed(u8 SpeedSet); //����SPI�ٶ�   
uint16_t SPI1_ReadWriteByte(uint16_t TxData);//SPI���߶�дһ���ֽ�

//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ������SD Card/W25X16/24L01/JF24C							  
//SPI�ڳ�ʼ��
//�������Ƕ�SPI1�ĳ�ʼ��

SPI_InitTypeDef  SPI_InitStructure;

void SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA|RCC_APB2Periph_SPI1, ENABLE );	
 
/************��������*************/
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;  //SPI_NSS
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //��������
// 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_4);
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;		//SPI_SCK
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //��������
// 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;		//SPI_MISO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//GPIO_Mode_AF_OD;  //���ÿ�©�������ӻ���   /���츴��
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;        //SPI_MOSI
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //��������
// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//�ӻ�
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;		//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//ѡ���˴���ʱ�ӵ���̬:ʱ�����ո�
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//���ݲ����ڵڶ���ʱ����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPI1, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���

	SPI_Cmd(SPI1, ENABLE); //ʹ��SPI����

  GPIO_ResetBits(GPIOA,GPIO_Pin_4);
}


void SPI2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTBʱ��ʹ�� 
	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI2,  ENABLE );//SPI2ʱ��ʹ�� 	
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;  //sck
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;  //miso
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //���츴�����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;  //mosi
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //���츴�����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/*
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_13|GPIO_Pin_14| GPIO_Pin_15;  //mosi
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //���츴�����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	*/
	//����PB12��Ϊ�ӻ�SPIͨ�ŵ�Ƭѡ�ܽţ������ڳ�ʼ���������0�Ϳ����ˣ�������ʾѡ��Ϊ�ӻ�
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_12);
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;		//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//ѡ���˴���ʱ�ӵ���̬:ʱ�����ո�
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//���ݲ����ڵڶ���ʱ����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPI2, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
 
	SPI_Cmd(SPI2, ENABLE); //ʹ��SPI����
	
  GPIO_ResetBits(GPIOB,GPIO_Pin_12);
}

/*����û��SPI1_SetSpeed����Ŷ��*/

uint16_t SPI2_ReadWriteByte(uint16_t TxData)
{		
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){}//�ȴ���������  
	SPI_I2S_SendData(SPI2, TxData); //ͨ������SPIx����һ��byte  ����
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET){} //�ȴ�������һ��byte  
	return SPI_I2S_ReceiveData(SPI2); //����ͨ��SPIx������յ�����	  
}




//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
uint16_t SPI1_ReadWriteByte(uint16_t TxData)
{		
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}//�ȴ���������  
	SPI_I2S_SendData(SPI1, TxData); //ͨ������SPIx����һ��byte  ����
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){} //�ȴ�������һ��byte  
	return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����	  
}



void test(){
			 u16 temp1;
			 u16 temp2;
			 temp1 =  SPI1_ReadWriteByte(99);
			 //temp2 =  SPI2_ReadWriteByte(88);
			 //temp1=SPI1->DR; //����ͨ��SPIx������յ�����
       printf("temp1 = %d\r\n",temp1);//��������е�0x01
			 //printf("temp2 = %d\r\n",temp2);//��������е�0x01
			 delay_ms(1000);
}



//���潫���յ�������
uint16_t Recv_Data_SPI(void){
	uint16_t recv = 0;
	//uint16_t recv2 = 0;
	recv = SPI1_ReadWriteByte(Reply_Signal);   //�յ�����һ������  ��㷢һ������
	//recv2 = SPI2_ReadWriteByte(Reply_Signal);   //�յ�����һ������  ��㷢һ������
	printf("recv = %d\r\n",recv);
	//printf("recv2 = %d\r\n",recv2);
	if(recv == LEFT )
	return LEFT;
	else if(recv == RIGHT )
		return RIGHT;
	else if(recv == RING )
		return RING;
	else 
		return 0;
}


void Control_Motor(uint16_t id){
		switch(id){
			case LEFT:{
				EN1;
				EN2;
				SPI1_ReadWriteByte(LEFT);
				//SPI2_ReadWriteByte(LEFT);
				printf("��ת��");
				LEFT1;
				LEFT2;
				break;
			}
			case RIGHT:{
				EN1;
				EN2;
				SPI1_ReadWriteByte(RIGHT);
				//SPI2_ReadWriteByte(RIGHT);
				printf("��ת��");
				RIGHT1;
				RIGHT2;
				break;
			}
			case RING:{
				EN1;
				EN2;
				SPI1_ReadWriteByte(RING);
				//SPI2_ReadWriteByte(RING);
				printf("��ת��");
				RING1;
				RING2;
				break;
			}
		}
}










