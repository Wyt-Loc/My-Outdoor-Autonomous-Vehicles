
#include "can.h"
#include "usart.h"
#include "delay.h"
#include "led.h"

////////////////  //////////////////
CAN_HandleTypeDef   CAN1_Handler;  //CAN1���
CAN_TxHeaderTypeDef	TxHeader;      //����
CAN_RxHeaderTypeDef	RxHeader;      //����

////CAN��ʼ��
//tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:CAN_SJW_1TQ~CAN_SJW_4TQ
//tbs2:ʱ���2��ʱ�䵥Ԫ.   ��Χ:CAN_BS2_1TQ~CAN_BS2_8TQ;
//tbs1:ʱ���1��ʱ�䵥Ԫ.   ��Χ:CAN_BS1_1TQ~CAN_BS1_16TQ
//brp :�����ʷ�Ƶ��.��Χ:1~1024; tq=(brp)*tpclk1
//������=Fpclk1/((tbs1+tbs2+1)*brp); ����tbs1��tbs2����ֻ�ù�ע��ʶ���ϱ�־����ţ�����CAN_BS2_1TQ�����Ǿ���Ϊtbs2=1�����㼴�ɡ�
//mode:CAN_MODE_NORMAL,��ͨģʽ;CAN_MODE_LOOPBACK,�ػ�ģʽ;
//Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ36M,�������CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_MODE_LOOPBACK);
//������Ϊ:36M/((8+9+1)*4)=500Kbps
//����ֵ:0,��ʼ��OK;
//    ����,��ʼ��ʧ��; 
u8 CAN1_Mode_Init(u32 tsjw,u32 tbs2,u32 tbs1,u16 brp,u32 mode)
{
	CAN_InitTypeDef		CAN1_InitConf;
    
    CAN1_Handler.Instance=CAN1;
	
	CAN1_Handler.Init = CAN1_InitConf;
	
    CAN1_Handler.Init.Prescaler=brp;				//��Ƶϵ��(Fdiv)Ϊbrp+1
    CAN1_Handler.Init.Mode=mode;					//ģʽ���� 
    CAN1_Handler.Init.SyncJumpWidth=tsjw;			//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1TQ~CAN_SJW_4TQ
    CAN1_Handler.Init.TimeSeg1=tbs1;				//tbs1��ΧCAN_BS1_1TQ~CAN_BS1_16TQ
    CAN1_Handler.Init.TimeSeg2=tbs2;				//tbs2��ΧCAN_BS2_1TQ~CAN_BS2_8TQ
    CAN1_Handler.Init.TimeTriggeredMode=DISABLE;	//��ʱ�䴥��ͨ��ģʽ 
    CAN1_Handler.Init.AutoBusOff=DISABLE;			//����Զ����߹���
    CAN1_Handler.Init.AutoWakeUp=DISABLE;			//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
    CAN1_Handler.Init.AutoRetransmission=ENABLE;	//��ֹ�����Զ����� 
    CAN1_Handler.Init.ReceiveFifoLocked=DISABLE;	//���Ĳ�����,�µĸ��Ǿɵ� 
    CAN1_Handler.Init.TransmitFifoPriority=DISABLE;	//���ȼ��ɱ��ı�ʶ������ 
	
    if(HAL_CAN_Init(&CAN1_Handler)!=HAL_OK)			//��ʼ��
		return 1;
    return 0;
}

//CAN�ײ��������������ã�ʱ�����ã��ж�����
//�˺����ᱻHAL_CAN_Init()����
//hcan:CAN���
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_CAN1_CLK_ENABLE();                //ʹ��CAN1ʱ��
    __HAL_RCC_GPIOA_CLK_ENABLE();			    //����GPIOAʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_12;   			//PA12
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;          //���츴��
    GPIO_Initure.Pull=GPIO_PULLUP;              //����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;    //����
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);         //��ʼ��
	
	GPIO_Initure.Pin=GPIO_PIN_11;   			//PA11
	GPIO_Initure.Mode=GPIO_MODE_AF_INPUT;     	//���츴��
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);         //��ʼ��
}

void CAN_Config(void)
{
  CAN_FilterTypeDef  sFilterConfig;

  /*����CAN������*/
  sFilterConfig.FilterBank = 0;                     //������0
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;		//����λģʽ
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;              //32λID
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;          //32λMASK
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;//������0������FIFO0
  sFilterConfig.FilterActivation = ENABLE;          //�����˲���0
  sFilterConfig.SlaveStartFilterBank = 14;

  //����������
  if (HAL_CAN_ConfigFilter(&CAN1_Handler, &sFilterConfig) != HAL_OK)
  {
    while(1){}
  }

  //����CAN��Χ�豸
  if (HAL_CAN_Start(&CAN1_Handler) != HAL_OK)
  {
    while(1){}
  }

  //�������RX֪ͨ
  if (HAL_CAN_ActivateNotification(&CAN1_Handler, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
    while(1){}
  }
  
  /*���ô������*/
  TxHeader.StdId = 0xFFE0>>5;
  TxHeader.ExtId = 0xFFFF>>5; // ��С
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.IDE = CAN_ID_STD;
  TxHeader.DLC = 2;
  TxHeader.TransmitGlobalTime = DISABLE;
}

//can����һ������(�̶���ʽ:IDΪ0X12,��׼֡,����֡)	
//len:���ݳ���(���Ϊ8)				     
//msg:����ָ��,���Ϊ8���ֽ�.
//����ֵ:0,�ɹ�;
//		 ����,ʧ��;
u8 CAN1_Send_Msg(u8* msg,u8 len,uint32_t id)
{
    u8 i=0;
		u32 TxMailbox;
		u8 message[8];
    TxHeader.StdId=0x00;        //��׼��ʶ��
    TxHeader.ExtId=id;        //��չ��ʶ��(29λ)
    TxHeader.IDE=CAN_ID_EXT;    //ʹ����չ֡  ��׼֡
    TxHeader.RTR=CAN_RTR_DATA;  //����֡
    TxHeader.DLC=len;                
    for(i=0;i<len;i++)
    {
		message[i]=msg[i];
			printf("message = %d", message[i]);
	  }
    if(HAL_CAN_AddTxMessage(&CAN1_Handler, &TxHeader, message, &TxMailbox) != HAL_OK)//����
		{
		return 1;
		}
		while(HAL_CAN_GetTxMailboxesFreeLevel(&CAN1_Handler) != 3) {}
		return 0;
}

//can�ڽ������ݲ�ѯ
//buf:���ݻ�����;	 
//����ֵ:0,�����ݱ��յ�;
//		 ����,���յ����ݳ���;
u8 CAN1_Receive_Msg(u8 *buf)
{
 	u32 i;
	u8	RxData[8];

	if(HAL_CAN_GetRxFifoFillLevel(&CAN1_Handler, CAN_RX_FIFO0) != 1)
	{
		return 0xF1;
	}

	if(HAL_CAN_GetRxMessage(&CAN1_Handler, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
	{
		return 0xF2;
	}
    for(i=0;i<RxHeader.DLC;i++)
    buf[i]=RxData[i];
	return RxHeader.DLC;
}
