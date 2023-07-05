/*			��̫��������Ϣ				*/
#include "atk_u2e.h"
#include "RingBuffer.h"
#include "sys.h"
#include "usart3.h"
#include "delay.h"
#include "stdio.h"
#include "string.h"


static uint8_t u2e_rxcmdbuf[U2E_RX_CMD_BUF_SIZE]; /*����ģ��������ݻ���*/

/**
 * @brief       ���������ж�ָ����
                ���÷���һ��������һ�����е����һ��λ��
 * 
 * @param       Src:      Ŀ��λ��
 * @param       valnum:   ��Ҫ�ڼ�������
 
 * @note       ģ�鷵�ص�ָ�������ʽΪ��+cmd="par_1","par_2",..."par_n"\r\nOK\r\n
               �����ģ��ATָ������ֲ�
 * 
 * @return      ��
 * 
*/

char key[18] = "FF-FF-FF-FF-FF-FF";         /* ģ���趨Ϊ��MAC��ַ��ʶ���� */
char * GetsearchKey(char* Src,int valnum) {
    uint8_t parposL=0;
    uint8_t parposR=0;
    uint8_t pos = 0;
    char* index = Src;
    
    while((*(index+pos) != '\0')&&(*(index+pos) != '='))        /* �ҵ���һ��������ʼλ�� */
    {
        pos++;
    }
    
    if(*(index+pos+1)!='"')
    {
       return '\0';        /* ָ���ʽ�����ϱ�ģ���趨�ĸ�ʽ */
    }
    
    while(*(index+pos)!='\0')
    {
        while((*(index+pos) != '\0')&&(*(index+pos) != '"'))pos++;
        parposL=pos++;  /* ��¼��ǰֵ��ָ����һ��Ԫ�� */
        while((*(index+pos) != '\0')&&(*(index+pos) != '"'))pos++;
        parposR=pos++;
        if(--valnum==0)break;
    }
    
    if(parposR>parposL)
        return &index[parposL+1];
    else
        return '\0';
    
}

/**
 * @brief       �������ݵ�ģ��
 * 
 * @param       data:   ��Ҫ�������ݵĻ����ַ
 * @param       size:   �������ݴ�С
 * 
 * @return      ��
 * 
*/
void send_data_to_u2e(uint8_t *data, uint32_t size)
{
    usart3_send_data(data, size);
}

/**
 * @brief       �������ģ�鲢��������У��
 * 
 * @param       cmd     :   ��Ҫ���͵�ATָ��
 * @param       ask     :   ��ҪУ���Ӧ������
 * @param       timeout :   ATָ��У�鳬ʱʱ�䣬��λ��100ms
 * 
 * @return      1  :   У��ask���ݳɹ�
 *              0  :   ģ�鷵��OK
 *             -1  :   ģ�鷵��ERROR
 *             -2  :   ����ATָ��У�鳬ʱ
 */
static int send_cmd_to_u2e(char *cmd, char *ask, uint32_t timeout)
{
    uint32_t rx_len = 0;

    /*��ʼ����������*/
    memset(u2e_rxcmdbuf, 0, U2E_RX_CMD_BUF_SIZE);
    RingBuffer_Reset();
    
    /*����ATָ�ģ��*/
    send_data_to_u2e((uint8_t *)cmd, strlen(cmd));

    /*�ȴ�ģ��Ӧ��ATָ����*/
    while (1)
    {
        if (strstr((char *)u2e_rxcmdbuf, ask) != NULL)
        {
            return 1;
        }
        else if (strstr((char *)u2e_rxcmdbuf, "OK") != NULL)
        {
            return 0;
        }
        else if (strstr((char *)u2e_rxcmdbuf, "ERROR") != NULL)
        {
            return -1;
        }

        if (res_usart3_revdata())
        {
            usart3_getRxData(&u2e_rxcmdbuf[rx_len++], 1); /* �Ӵ��ڻ����ж�һ���ֽ� */

            if (rx_len >= U2E_RX_CMD_BUF_SIZE) /* ����Ӧ�����ݳ���������ERROR */
            {
                return -1;
            }
        }
        else
        {
            timeout--;

            if (timeout == 0)
            {
                RingBuffer_Reset();
                return -2;
            }

            delay_ms(100);
        }
    }
}

/**
 * @brief       ģ���������״̬
 * 
 * @param       ��
 * 
 * @return      0  :    �ɹ���������״̬
 *             -1  :    ��������״̬ʧ��
 */
static int u2e_enter_configmode(void)
{
    int res = -1;
    char* pos=0;
    uint8_t atkey[]="AT+AT_INTO=\"FF-FF-FF-FF-01-02\"\r\n";


    /* 1��׼����������״̬ */
    res = send_cmd_to_u2e("AT+SEARCH?\r\n", "\r\nOK\r\n", 5);       /* ����ûָ�ģ��������õķ���������ģ�飬����Ҫ�ȴ�ģ��ָ������ɹ�����������ѭ����ѯ */
	if(res != 0)
    {
        return -1;
    }
    /* �յ�ʶ�𴮣���ȡ�豸ʶ���� */
    pos = GetsearchKey((char*)u2e_rxcmdbuf,2);
    if(pos != 0)
    {
        memcpy(key,pos,strlen(key));
        /* 2.����atkȷ�Ͻ�������״̬ */
        sprintf((char*)atkey,"AT+AT_INTO=\"%s\"\r\n",key);
        res = send_cmd_to_u2e((char*)atkey, "\r\nOK\r\n", 5);
        if(res < 0)
            return -1;
    }
    else
    {
        return -1;
    }

    return 0;
}

/**
 * @brief       ģ�����͸��״̬
 * 
 * @param       ��
 * 
 * @return      0  :    �ɹ�����͸��״̬
 *             -1  :    ����͸��״̬ʧ��
 */
static int u2e_enter_transfermode(void)
{
    int res = 0;
    char* pos=0;
    uint8_t atkey[]="AT+AT_EXIT=\"FF-FF-FF-FF-01-02\"\r\n";

    /* 1.׼����������״̬ */
    while(res != 0)
    {
        res = send_cmd_to_u2e("AT+SEARCH?\r\n", "\r\nOK\r\n", 5);       /* ����ûָ�ģ��������õķ���������ģ�飬����Ҫ�ȴ�ģ��ָ������ɹ�����������ѭ����ѯ */
    }
    
    /* 2.�յ�ʶ�𴮣���ȡ�豸ʶ���� */
    pos = GetsearchKey((char*)u2e_rxcmdbuf,2);
    if(pos != 0)
    {
        memcpy(key,pos,strlen(key));
        /* 3.����atkȷ�Ͻ�������״̬ */
        sprintf((char*)atkey,"AT+AT_INTO=\"%s\"\r\n",key);
        res = send_cmd_to_u2e((char*)atkey, "\r\nOK\r\n", 5);
        if(res==0)
            return 0;
    }

    /* 4.������õ�����ģʽ���Ƿ����ӳɹ� */
    while(res != 0)
    {
        res = send_cmd_to_u2e("AT+NETLK_ST?\r\n", "\r\n+NETLK_ST=\"ON\"\r\n", 5);
    }

    /* 2.�յ�ʶ�𴮣���ȡ�豸ʶ���� */
    pos = GetsearchKey((char*)u2e_rxcmdbuf,2);
    if(pos != 0)
    {
        memcpy(key,pos,strlen(key));
        /* 3.����atkȷ�Ͻ�������״̬ */
        sprintf((char*)atkey,"AT+AT_EXIT=\"%s\"\r\n",key);
        res = send_cmd_to_u2e((char*)atkey, "\r\nOK\r\n", 5);
        if(res==0)
            return 0;
    }

    return -1;
}

static const _u2e_atcmd_st u2e_udp_server_info[] = {

    /*1.ѡ����ģʽΪ��UDP Server*/
    {5, "AT+ECHO?\r\n",       "AT+ECHO=\"ON\"\r\n"},
    {5, "AT+ETH_MOD?\r\n",    "AT+ETH_MOD=\"UDP_SERVER\"\r\n"},
    {5, "AT+LOCAL_PORT?\r\n", "AT+LOCAL_PORT=\"21019\"\r\n"},

    /*2.��������͸��ģʽ�Ĺ�������*/
    {5, "AT+DHCP?\r\n",      "AT+DHCP=\"ON\"\r\n"},
//    {5, "AT+MAC_ID?\r\n",    "AT+MAC_ID=\"04-78-63-01-02-F2\"\r\n"},
    {5, "AT+STATICIP?\r\n",  "AT+STATICIP=\"192.168.30.201\"\r\n"},
    {5, "AT+NETMASK?\r\n",   "AT+NETMASK=\"255.255.255.0\"\r\n"},
    {5, "AT+GATEWAY?\r\n",   "AT+GATEWAY=\"192.168.30.1\"\r\n"},
    {5, "AT+MODUNAME?\r\n",  "AT+MODUNAME=\"ATK-UDPSEV-MOD\"\r\n"},

    /*3.���ڲ���*/
    {5, "AT+UART?\r\n",             "AT+UART=\"115200\",\"1\",\"8\",\"NONE\"\r\n"},
    {5, "AT+UART_PACKLEN?\r\n",     "AT+UART_PACKLEN=\"50\"\r\n"},
    {5, "AT+UART_PACKTIME?\r\n",    "AT+UART_PACKTIME=\"50\"\r\n"},
    
    /*4.����ģ�������ģ���Զ����� */
    {5, "AT+UPDATE?\r\n",   "AT+UPDATE?\r\n"},
};

static const _u2e_atcmd_st u2e_udp_client_info[] = {

    /* 1.ѡ����ģʽΪ��UDP_CLIENT */
    {5, "AT+ECHO?\r\n",        "AT+ECHO=\"ON\"\r\n"},
    {5, "AT+ETH_MOD?\r\n",     "AT+ETH_MOD=\"UDP_CLIENT\"\r\n"},
    {5, "AT+REMOTEIP?\r\n",    "AT+REMOTEIP=\"192.168.30.201\"\r\n"},
    {5, "AT+REMOTE_PORT?\r\n", "AT+REMOTE_PORT=\"10006\"\r\n"},

    /*2.��������͸��ģʽ�Ĺ�������*/
    {5, "AT+DHCP?\r\n",      "AT+DHCP=\"ON\"\r\n"},
//    {5, "AT+MAC_ID?\r\n",    "AT+MAC_ID=\"04-78-63-01-02-F2\"\r\n"},
    {5, "AT+STATICIP?\r\n",  "AT+STATICIP=\"192.168.30.190\"\r\n"},
    {5, "AT+NETMASK?\r\n",   "AT+NETMASK=\"255.255.255.0\"\r\n"},
    {5, "AT+GATEWAY?\r\n",   "AT+GATEWAY=\"192.168.30.1\"\r\n"},
    {5, "AT+MODUNAME?\r\n",  "AT+MODUNAME=\"ATK-UDPCLI-MOD\"\r\n"},

    /*3.���ڲ���*/
    {5, "AT+UART?\r\n",             "AT+UART=\"115200\",\"1\",\"8\",\"NONE\"\r\n"},
    {5, "AT+UART_PACKLEN?\r\n",     "AT+UART_PACKLEN=\"50\"\r\n"},
    {5, "AT+UART_PACKTIME?\r\n",    "AT+UART_PACKTIME=\"50\"\r\n"},

    /*4.����ģ�������ģ���Զ����� */
    {5, "AT+UPDATE?\r\n",   "AT+UPDATE?\r\n"},
};

static const _u2e_atcmd_st u2e_tcp_server_info[] = {

    /* 1.ѡ����ģʽΪ��TCP_SERVER */
    {5, "AT+ECHO?\r\n",       "AT+ECHO=\"ON\"\r\n"},
    {5, "AT+ETH_MOD?\r\n",    "AT+ETH_MOD=\"TCP_SERVER\"\r\n"},
    {5, "AT+LOCAL_PORT?\r\n", "AT+LOCAL_PORT=\"10006\"\r\n"},

    /*2.��������͸��ģʽ�Ĺ�������*/
    {5, "AT+DHCP?\r\n",      "AT+DHCP=\"OFF\"\r\n"},
//    {5, "AT+MAC_ID?\r\n",    "AT+MAC_ID=\"04-78-63-01-02-F2\"\r\n"},
    {5, "AT+STATICIP?\r\n",  "AT+STATICIP=\"192.168.30.201\"\r\n"},
    {5, "AT+NETMASK?\r\n",   "AT+NETMASK=\"255.255.255.0\"\r\n"},
    {5, "AT+GATEWAY?\r\n",   "AT+GATEWAY=\"192.168.30.1\"\r\n"},
    {5, "AT+MODUNAME?\r\n",  "AT+MODUNAME=\"ATK-TCPSRV-MOD\"\r\n"},

    /*3.���ڲ���*/
    {5, "AT+UART?\r\n",             "AT+UART=\"115200\",\"1\",\"8\",\"NONE\"\r\n"},
    {5, "AT+UART_PACKLEN?\r\n",     "AT+UART_PACKLEN=\"50\"\r\n"},
    {5, "AT+UART_PACKTIME?\r\n",    "AT+UART_PACKTIME=\"50\"\r\n"},

    /*4.����ģ�������ģ���Զ����� */
    {5, "AT+UPDATE?\r\n",   "AT+UPDATE?\r\n"},
    /*6.����������������*/
};

static const _u2e_atcmd_st u2e_tcp_client_info[] = {
    /*1.ѡ����ģʽΪ��TCP_CLIENT*/
    {5, "AT+ECHO?\r\n",       "AT+ECHO=\"ON\"\r\n"},
    {5, "AT+ETH_MOD?\r\n",    "AT+ETH_MOD=\"TCP_CLIENT\"\r\n"},
    {5, "AT+REMOTEIP?\r\n",   "AT+REMOTEIP=\"192.168.1.101\"\r\n"},
    {5, "AT+REMOTE_PORT?\r\n","AT+REMOTE_PORT=\"10006\"\r\n"},

    /*2.��������͸��ģʽ�Ĺ�������*/
    {5, "AT+DHCP?\r\n",      "AT+DHCP=\"ON\"\r\n"},
//    {5, "AT+MAC_ID?\r\n",    "AT+MAC_ID=\"04-78-63-01-02-F2\"\r\n"},
    {5, "AT+STATICIP?\r\n",  "AT+STATICIP=\"192.168.1.101\"\r\n"},
    {5, "AT+NETMASK?\r\n",   "AT+NETMASK=\"255.255.255.0\"\r\n"},
    {5, "AT+GATEWAY?\r\n",   "AT+GATEWAY=\"192.168.1.101\"\r\n"},
    {5, "AT+MODUNAME?\r\n",  "AT+MODUNAME=\"ATK-TCPCLI-MOD\"\r\n"},

    /*3.���ڲ���*/
    {5, "AT+UART?\r\n",             "AT+UART=\"115200\",\"1\",\"8\",\"NONE\"\r\n"},
    {5, "AT+UART_PACKLEN?\r\n",     "AT+UART_PACKLEN=\"50\"\r\n"},
    {5, "AT+UART_PACKTIME?\r\n",    "AT+UART_PACKTIME=\"50\"\r\n"},

    /*4.����ģ�������ģ���Զ����� */
    {5, "AT+UPDATE?\r\n",    "AT+UPDATE?\r\n"},
};

static const _u2e_atcmd_st u2e_atkcloud_info[]={
    /* 1.����ָ�����,ѡ����ģʽ */
    {5, "AT+ECHO?\r\n",       "AT+ECHO=\"ON\"\r\n"},
    {5, "AT+ETH_MOD?\r\n",    "AT+ETH_MOD=\"TCP_CLOUD\"\r\n"},

    /*2.�����뱾��·��ƥ��Ĺ�������*/
    {5, "AT+DHCP?\r\n",       "AT+DHCP=\"ON\"\r\n"},
//    {5, "AT+MAC_ID?\r\n",     "AT+MAC_ID=\"04-78-63-01-02-F2\"\r\n"},
    {5, "AT+STATICIP?\r\n",   "AT+STATICIP=\"192.168.30.190\"\r\n"},
    {5, "AT+NETMASK?\r\n",    "AT+NETMASK=\"255.255.255.0\"\r\n"},
    {5, "AT+GATEWAY?\r\n",    "AT+GATEWAY=\"192.168.30.1\"\r\n"},
    {5, "AT+DNS_SERVER?\r\n", "AT+DNS_SERVER=\"114.114.114.114\"\r\n"},      /* ����DNS�������Խ������� */
    {5, "AT+MODUNAME?\r\n",   "AT+MODUNAME=\"ATK-CLOUD-MOD\"\r\n"},

    /*3.���ڲ���*/
    {5, "AT+UART?\r\n",             "AT+UART=\"115200\",\"1\",\"8\",\"NONE\"\r\n"},
    {5, "AT+UART_PACKLEN?\r\n",     "AT+UART_PACKLEN=\"50\"\r\n"},
    {5, "AT+UART_PACKTIME?\r\n",    "AT+UART_PACKTIME=\"50\"\r\n"},
    
    /*4.����ԭ���ƹ��ܣ�ʹ��*/
    {5, "AT+CLOUD_LINK_EN?\r\n",   "AT+CLOUD_LINK_EN=\"ON\"\r\n"},
    {5, "AT+COLUD_DOMAIN?\r\n",    "AT+COLUD_DOMAIN=\"CLOUD.ALIENTEK.COM\"\r\n"},
    {5, "AT+CLOUD_PORT?\r\n",      "AT+CLOUD_PORT=\"59666\"\r\n"},
    {5, "AT+ATKLINKPWD?\r\n",      "AT+ATKLINKPWD=\"12345678\"\r\n"},
    {5, "AT+ATKYUNDEVID?\r\n",     "AT+ATKYUNDEVID=\"65112596110969426801\"\r\n"},

    /*5.����ģ�������ģ���Զ����� */
    {5, "AT+UPDATE?\r\n",    "AT+UPDATE?\r\n"},
};

static const _u2e_atcmd_st u2e_udpmulticast_info[]={
    /* 1.����ָ�����,ѡ����ģʽ */
    {5, "AT+ECHO?\r\n",       "AT+ECHO=\"ON\"\r\n"},
    {5, "AT+ETH_MOD?\r\n",    "AT+ETH_MOD=\"UDP_MULTICAST\"\r\n"},

    /*2.�����뱾��·��ƥ��Ĺ�������*/
    {5, "AT+DHCP?\r\n",       "AT+DHCP=\"ON\"\r\n"},
//    {5, "AT+MAC_ID?\r\n",     "AT+MAC_ID=\"04-78-63-01-02-F2\"\r\n"},
    {5, "AT+STATICIP?\r\n",   "AT+STATICIP=\"192.168.30.190\"\r\n"},
    {5, "AT+NETMASK?\r\n",    "AT+NETMASK=\"255.255.255.0\"\r\n"},
    {5, "AT+GATEWAY?\r\n",    "AT+GATEWAY=\"192.168.30.1\"\r\n"},
    {5, "AT+DNS_SERVER?\r\n", "AT+DNS_SERVER=\"114.114.114.114\"\r\n"},      /* ����DNS�������Խ������� */
    {5, "AT+MODUNAME?\r\n",   "AT+MODUNAME=\"ATK-CLOUD-MOD\"\r\n"},

    /*3.���ڲ���*/
    {5, "AT+UART?\r\n",             "AT+UART=\"115200\",\"1\",\"8\",\"NONE\"\r\n"},
    {5, "AT+UART_PACKLEN?\r\n",     "AT+UART_PACKLEN=\"50\"\r\n"},
    {5, "AT+UART_PACKTIME?\r\n",    "AT+UART_PACKTIME=\"50\"\r\n"},
    
    /*4.����ģ�������ģ���Զ����� */
    {5, "AT+UPDATE?\r\n",    "AT+UPDATE?\r\n"},
};

/**
 * @brief       ����ģ�鹤������
 * 
 * @param       work_param      :   ����ģʽ���ATָ�����
 * @param       num             :   ��Ҫ���õ�ATָ���������
 * 
 * @return      0  :    ���в������óɹ�
 *              n  :    �ڼ�����������ʧ��
 */
static int u2e_config_work_param(_u2e_atcmd_st *work_param, uint8_t num)
{
    int i;
    int res = 0;

    for (i = 0; i < num; i++)
    {
        res = send_cmd_to_u2e((work_param + i)->read_cmd,
                              (work_param + i)->write_cmd + strlen((work_param + i)->read_cmd) - 1,
                              work_param[i].timeout);
        if (res == 1) /*���ģ���ڲ���������Ҫ���õĲ���һ�£�����Ҫ�ظ�ȥ����*/
        {
            continue;
        }
        else /*ģ���ڲ����������ò�����һ�£���Ҫ����ģ���ڲ�����*/
        {
            res = send_cmd_to_u2e((work_param + i)->write_cmd,
                                  "\r\nOK",
                                  (work_param + i)->timeout);

            if (res < 0)
            {
                return (i+1);       /* ����ָ����ţ���ʼΪ1 */
            }
        }
    }

    return 0;
}

/**
 * @brief       ��ʼ��ģ��Ĺ���״̬
 * 
 * @param       work_mode   :   ģ�鹤��ģʽ
 *  @arg        U2E_UDP_SERVER,      0,  UDP������
 *  @arg        U2E_UDP_CLIENT,      1,  UDP�ͻ���ģʽ
 *  @arg        U2E_TCP_SERVER,      2,  TCP������
 *  @arg        U2E_TCP_CLIENT,      3,  TCP�ͻ���
 * 
 * @return      0   :   ��ʼ���ɹ�
 *             -1   :   ������״̬ʧ��
 *             -2   :   ģ�鹤����������ʧ��
 */

int u2e_config_init(_u2e_work_mode_eu work_mode)
{
	
	int res;
	
		
    
    GPIO_InitTypeDef gpio_initure;    /*GPIO�˿����ò������λ��*/
    U2E_MODULE_DF_CLK_ENABLE();
    U2E_MODULE_ST_CLK_ENABLE();
    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_AFIO_REMAP_SWJ_NOJTAG();    /* �ر�JTAG, ʹ��SWD, ����PA15, �ſ��Ե���ͨIOʹ�� */    

    gpio_initure.Pin = U2E_MODULE_DF_PIN;               /*���ڷ������ź�*/
    gpio_initure.Mode = GPIO_MODE_OUTPUT_PP;            /*�����������*/
    gpio_initure.Pull = GPIO_PULLUP;                    /*����*/
    gpio_initure.Speed = GPIO_SPEED_FREQ_HIGH;          /*IO�ٶ�����Ϊ����*/
    HAL_GPIO_Init(U2E_MODULE_DF_PORT, &gpio_initure);
    U2E_MODULE_DF(1);                                   /* ���ߴ�����ʹģ���������� */

    gpio_initure.Pin = U2E_MODULE_ST_PIN;               /*���ڷ������ź�*/
    gpio_initure.Pull = GPIO_PULLUP;                    /*����*/
    gpio_initure.Mode = GPIO_MODE_INPUT;                /*��������ģʽ*/
    HAL_GPIO_Init(U2E_MODULE_ST_PORT, &gpio_initure);

    usart3_init(115200);                                /* ����3��ʼ��Ϊ115200 */


//    /* 1.��λģ�����������ò�����Ĭ�ϣ����ڲ���Ϊ��115200��8����λ����У��(ѡ��) */
//    U2E_MODULE_DF(0);
//    delay_ms(1000);
//    U2E_MODULE_DF(1);
//    //while(U2E_MODULE_ST != 1);      /* ģ��������״̬ */
//    delay_ms(3000);

    /*2.ģ���������״̬*/
    while ( u2e_enter_configmode() != 0 );


    /*3.����ģ��Ĺ�������*/
    switch (work_mode)
    {
        case U2E_UDP_SERVER:
        {
            res = u2e_config_work_param((_u2e_atcmd_st *)&u2e_udp_server_info, sizeof(u2e_udp_server_info) / sizeof(_u2e_atcmd_st));
            break;
        }
        case U2E_UDP_CLIENT:
        {
            res = u2e_config_work_param((_u2e_atcmd_st *)&u2e_udp_client_info, sizeof(u2e_udp_client_info) / sizeof(_u2e_atcmd_st));
            break;
        }
        case U2E_TCP_SERVER:
        {
            res = u2e_config_work_param((_u2e_atcmd_st *)&u2e_tcp_server_info, sizeof(u2e_tcp_server_info) / sizeof(_u2e_atcmd_st));
            break;
        }
        case U2E_TCP_CLIENT:
        {
            res = u2e_config_work_param((_u2e_atcmd_st *)&u2e_tcp_client_info, sizeof(u2e_tcp_client_info) / sizeof(_u2e_atcmd_st));
            break;
        }
        case U2E_ATK_CLOUD:
        {
            res = u2e_config_work_param((_u2e_atcmd_st *)&u2e_atkcloud_info, sizeof(u2e_atkcloud_info) / sizeof(_u2e_atcmd_st));
            break;
        }
        case U2E_UDP_MULTICAST:
        {
            res = u2e_config_work_param((_u2e_atcmd_st *)&u2e_udpmulticast_info, sizeof(u2e_udpmulticast_info) / sizeof(u2e_udpmulticast_info));
            break;
        }
        default:
        {
            break;
        }
    }

    if( res != 0 )
    {
        return -2;
    }
    
    /* �ȴ�ģ�����óɹ��������һ�����ø����˴��ڵĲ����ʣ�����Ҫ�������³�ʼ��STM32����3�Ĳ����� */
//    usart3_init(36, 921600);        /* ���³�ʼ������3��ʼ��Ϊ921600 */
    //u2e_enter_transfermode();

    return 0;
}
