/*			以太网接收信息				*/
#include "atk_u2e.h"
#include "RingBuffer.h"
#include "sys.h"
#include "usart3.h"
#include "delay.h"
#include "stdio.h"
#include "string.h"


static uint8_t u2e_rxcmdbuf[U2E_RX_CMD_BUF_SIZE]; /*处理模块相关数据缓存*/

/**
 * @brief       本函数不判断指令结果
                调用返回一个串在另一个串中的最后一处位置
 * 
 * @param       Src:      目标位置
 * @param       valnum:   需要第几个参数
 
 * @note       模块返回的指令参数格式为：+cmd="par_1","par_2",..."par_n"\r\nOK\r\n
               详情见模块AT指令操作手册
 * 
 * @return      无
 * 
*/

char key[18] = "FF-FF-FF-FF-FF-FF";         /* 模块设定为以MAC地址作识别码 */
char * GetsearchKey(char* Src,int valnum) {
    uint8_t parposL=0;
    uint8_t parposR=0;
    uint8_t pos = 0;
    char* index = Src;
    
    while((*(index+pos) != '\0')&&(*(index+pos) != '='))        /* 找到第一个参数起始位置 */
    {
        pos++;
    }
    
    if(*(index+pos+1)!='"')
    {
       return '\0';        /* 指令格式不符合本模块设定的格式 */
    }
    
    while(*(index+pos)!='\0')
    {
        while((*(index+pos) != '\0')&&(*(index+pos) != '"'))pos++;
        parposL=pos++;  /* 记录当前值并指向下一个元素 */
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
 * @brief       发送数据到模块
 * 
 * @param       data:   需要发送数据的缓存地址
 * @param       size:   发送数据大小
 * 
 * @return      无
 * 
*/
void send_data_to_u2e(uint8_t *data, uint32_t size)
{
    usart3_send_data(data, size);
}

/**
 * @brief       发送命令到模块并进行数据校验
 * 
 * @param       cmd     :   需要发送的AT指令
 * @param       ask     :   需要校验的应答数据
 * @param       timeout :   AT指令校验超时时间，单位：100ms
 * 
 * @return      1  :   校验ask数据成功
 *              0  :   模块返回OK
 *             -1  :   模块返回ERROR
 *             -2  :   发送AT指令校验超时
 */
static int send_cmd_to_u2e(char *cmd, char *ask, uint32_t timeout)
{
    uint32_t rx_len = 0;

    /*初始化缓存数据*/
    memset(u2e_rxcmdbuf, 0, U2E_RX_CMD_BUF_SIZE);
    RingBuffer_Reset();
    
    /*发送AT指令到模块*/
    send_data_to_u2e((uint8_t *)cmd, strlen(cmd));

    /*等待模块应答AT指令结果*/
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
            usart3_getRxData(&u2e_rxcmdbuf[rx_len++], 1); /* 从串口缓存中读一个字节 */

            if (rx_len >= U2E_RX_CMD_BUF_SIZE) /* 接收应答数据超长，返回ERROR */
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
 * @brief       模块进入配置状态
 * 
 * @param       无
 * 
 * @return      0  :    成功进入配置状态
 *             -1  :    进入配置状态失败
 */
static int u2e_enter_configmode(void)
{
    int res = -1;
    char* pos=0;
    uint8_t atkey[]="AT+AT_INTO=\"FF-FF-FF-FF-01-02\"\r\n";


    /* 1。准备进入配置状态 */
    res = send_cmd_to_u2e("AT+SEARCH?\r\n", "\r\nOK\r\n", 5);       /* 如果用恢复模块出厂设置的方法来设置模块，则需要等待模块恢复参数成功，这里用死循环查询 */
	if(res != 0)
    {
        return -1;
    }
    /* 收到识别串，提取设备识别码 */
    pos = GetsearchKey((char*)u2e_rxcmdbuf,2);
    if(pos != 0)
    {
        memcpy(key,pos,strlen(key));
        /* 2.发送atk确认进入配置状态 */
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
 * @brief       模块进入透传状态
 * 
 * @param       无
 * 
 * @return      0  :    成功进入透传状态
 *             -1  :    进入透传状态失败
 */
static int u2e_enter_transfermode(void)
{
    int res = 0;
    char* pos=0;
    uint8_t atkey[]="AT+AT_EXIT=\"FF-FF-FF-FF-01-02\"\r\n";

    /* 1.准备进入配置状态 */
    while(res != 0)
    {
        res = send_cmd_to_u2e("AT+SEARCH?\r\n", "\r\nOK\r\n", 5);       /* 如果用恢复模块出厂设置的方法来设置模块，则需要等待模块恢复参数成功，这里用死循环查询 */
    }
    
    /* 2.收到识别串，提取设备识别码 */
    pos = GetsearchKey((char*)u2e_rxcmdbuf,2);
    if(pos != 0)
    {
        memcpy(key,pos,strlen(key));
        /* 3.发送atk确认进入配置状态 */
        sprintf((char*)atkey,"AT+AT_INTO=\"%s\"\r\n",key);
        res = send_cmd_to_u2e((char*)atkey, "\r\nOK\r\n", 5);
        if(res==0)
            return 0;
    }

    /* 4.检查配置的网络模式下是否连接成功 */
    while(res != 0)
    {
        res = send_cmd_to_u2e("AT+NETLK_ST?\r\n", "\r\n+NETLK_ST=\"ON\"\r\n", 5);
    }

    /* 2.收到识别串，提取设备识别码 */
    pos = GetsearchKey((char*)u2e_rxcmdbuf,2);
    if(pos != 0)
    {
        memcpy(key,pos,strlen(key));
        /* 3.发送atk确认进入配置状态 */
        sprintf((char*)atkey,"AT+AT_EXIT=\"%s\"\r\n",key);
        res = send_cmd_to_u2e((char*)atkey, "\r\nOK\r\n", 5);
        if(res==0)
            return 0;
    }

    return -1;
}

static const _u2e_atcmd_st u2e_udp_server_info[] = {

    /*1.选择工作模式为：UDP Server*/
    {5, "AT+ECHO?\r\n",       "AT+ECHO=\"ON\"\r\n"},
    {5, "AT+ETH_MOD?\r\n",    "AT+ETH_MOD=\"UDP_SERVER\"\r\n"},
    {5, "AT+LOCAL_PORT?\r\n", "AT+LOCAL_PORT=\"21019\"\r\n"},

    /*2.配置网络透传模式的工作参数*/
    {5, "AT+DHCP?\r\n",      "AT+DHCP=\"ON\"\r\n"},
//    {5, "AT+MAC_ID?\r\n",    "AT+MAC_ID=\"04-78-63-01-02-F2\"\r\n"},
    {5, "AT+STATICIP?\r\n",  "AT+STATICIP=\"192.168.30.201\"\r\n"},
    {5, "AT+NETMASK?\r\n",   "AT+NETMASK=\"255.255.255.0\"\r\n"},
    {5, "AT+GATEWAY?\r\n",   "AT+GATEWAY=\"192.168.30.1\"\r\n"},
    {5, "AT+MODUNAME?\r\n",  "AT+MODUNAME=\"ATK-UDPSEV-MOD\"\r\n"},

    /*3.串口参数*/
    {5, "AT+UART?\r\n",             "AT+UART=\"115200\",\"1\",\"8\",\"NONE\"\r\n"},
    {5, "AT+UART_PACKLEN?\r\n",     "AT+UART_PACKLEN=\"50\"\r\n"},
    {5, "AT+UART_PACKTIME?\r\n",    "AT+UART_PACKTIME=\"50\"\r\n"},
    
    /*4.更新模块参数，模块自动重启 */
    {5, "AT+UPDATE?\r\n",   "AT+UPDATE?\r\n"},
};

static const _u2e_atcmd_st u2e_udp_client_info[] = {

    /* 1.选择工作模式为：UDP_CLIENT */
    {5, "AT+ECHO?\r\n",        "AT+ECHO=\"ON\"\r\n"},
    {5, "AT+ETH_MOD?\r\n",     "AT+ETH_MOD=\"UDP_CLIENT\"\r\n"},
    {5, "AT+REMOTEIP?\r\n",    "AT+REMOTEIP=\"192.168.30.201\"\r\n"},
    {5, "AT+REMOTE_PORT?\r\n", "AT+REMOTE_PORT=\"10006\"\r\n"},

    /*2.配置网络透传模式的工作参数*/
    {5, "AT+DHCP?\r\n",      "AT+DHCP=\"ON\"\r\n"},
//    {5, "AT+MAC_ID?\r\n",    "AT+MAC_ID=\"04-78-63-01-02-F2\"\r\n"},
    {5, "AT+STATICIP?\r\n",  "AT+STATICIP=\"192.168.30.190\"\r\n"},
    {5, "AT+NETMASK?\r\n",   "AT+NETMASK=\"255.255.255.0\"\r\n"},
    {5, "AT+GATEWAY?\r\n",   "AT+GATEWAY=\"192.168.30.1\"\r\n"},
    {5, "AT+MODUNAME?\r\n",  "AT+MODUNAME=\"ATK-UDPCLI-MOD\"\r\n"},

    /*3.串口参数*/
    {5, "AT+UART?\r\n",             "AT+UART=\"115200\",\"1\",\"8\",\"NONE\"\r\n"},
    {5, "AT+UART_PACKLEN?\r\n",     "AT+UART_PACKLEN=\"50\"\r\n"},
    {5, "AT+UART_PACKTIME?\r\n",    "AT+UART_PACKTIME=\"50\"\r\n"},

    /*4.更新模块参数，模块自动重启 */
    {5, "AT+UPDATE?\r\n",   "AT+UPDATE?\r\n"},
};

static const _u2e_atcmd_st u2e_tcp_server_info[] = {

    /* 1.选择工作模式为：TCP_SERVER */
    {5, "AT+ECHO?\r\n",       "AT+ECHO=\"ON\"\r\n"},
    {5, "AT+ETH_MOD?\r\n",    "AT+ETH_MOD=\"TCP_SERVER\"\r\n"},
    {5, "AT+LOCAL_PORT?\r\n", "AT+LOCAL_PORT=\"10006\"\r\n"},

    /*2.配置网络透传模式的工作参数*/
    {5, "AT+DHCP?\r\n",      "AT+DHCP=\"OFF\"\r\n"},
//    {5, "AT+MAC_ID?\r\n",    "AT+MAC_ID=\"04-78-63-01-02-F2\"\r\n"},
    {5, "AT+STATICIP?\r\n",  "AT+STATICIP=\"192.168.30.201\"\r\n"},
    {5, "AT+NETMASK?\r\n",   "AT+NETMASK=\"255.255.255.0\"\r\n"},
    {5, "AT+GATEWAY?\r\n",   "AT+GATEWAY=\"192.168.30.1\"\r\n"},
    {5, "AT+MODUNAME?\r\n",  "AT+MODUNAME=\"ATK-TCPSRV-MOD\"\r\n"},

    /*3.串口参数*/
    {5, "AT+UART?\r\n",             "AT+UART=\"115200\",\"1\",\"8\",\"NONE\"\r\n"},
    {5, "AT+UART_PACKLEN?\r\n",     "AT+UART_PACKLEN=\"50\"\r\n"},
    {5, "AT+UART_PACKTIME?\r\n",    "AT+UART_PACKTIME=\"50\"\r\n"},

    /*4.更新模块参数，模块自动重启 */
    {5, "AT+UPDATE?\r\n",   "AT+UPDATE?\r\n"},
    /*6.其他参数功能配置*/
};

static const _u2e_atcmd_st u2e_tcp_client_info[] = {
    /*1.选择工作模式为：TCP_CLIENT*/
    {5, "AT+ECHO?\r\n",       "AT+ECHO=\"ON\"\r\n"},
    {5, "AT+ETH_MOD?\r\n",    "AT+ETH_MOD=\"TCP_CLIENT\"\r\n"},
    {5, "AT+REMOTEIP?\r\n",   "AT+REMOTEIP=\"192.168.1.101\"\r\n"},
    {5, "AT+REMOTE_PORT?\r\n","AT+REMOTE_PORT=\"10006\"\r\n"},

    /*2.配置网络透传模式的工作参数*/
    {5, "AT+DHCP?\r\n",      "AT+DHCP=\"ON\"\r\n"},
//    {5, "AT+MAC_ID?\r\n",    "AT+MAC_ID=\"04-78-63-01-02-F2\"\r\n"},
    {5, "AT+STATICIP?\r\n",  "AT+STATICIP=\"192.168.1.101\"\r\n"},
    {5, "AT+NETMASK?\r\n",   "AT+NETMASK=\"255.255.255.0\"\r\n"},
    {5, "AT+GATEWAY?\r\n",   "AT+GATEWAY=\"192.168.1.101\"\r\n"},
    {5, "AT+MODUNAME?\r\n",  "AT+MODUNAME=\"ATK-TCPCLI-MOD\"\r\n"},

    /*3.串口参数*/
    {5, "AT+UART?\r\n",             "AT+UART=\"115200\",\"1\",\"8\",\"NONE\"\r\n"},
    {5, "AT+UART_PACKLEN?\r\n",     "AT+UART_PACKLEN=\"50\"\r\n"},
    {5, "AT+UART_PACKTIME?\r\n",    "AT+UART_PACKTIME=\"50\"\r\n"},

    /*4.更新模块参数，模块自动重启 */
    {5, "AT+UPDATE?\r\n",    "AT+UPDATE?\r\n"},
};

static const _u2e_atcmd_st u2e_atkcloud_info[]={
    /* 1.开启指令回显,选择工作模式 */
    {5, "AT+ECHO?\r\n",       "AT+ECHO=\"ON\"\r\n"},
    {5, "AT+ETH_MOD?\r\n",    "AT+ETH_MOD=\"TCP_CLOUD\"\r\n"},

    /*2.配置与本地路由匹配的工作参数*/
    {5, "AT+DHCP?\r\n",       "AT+DHCP=\"ON\"\r\n"},
//    {5, "AT+MAC_ID?\r\n",     "AT+MAC_ID=\"04-78-63-01-02-F2\"\r\n"},
    {5, "AT+STATICIP?\r\n",   "AT+STATICIP=\"192.168.30.190\"\r\n"},
    {5, "AT+NETMASK?\r\n",    "AT+NETMASK=\"255.255.255.0\"\r\n"},
    {5, "AT+GATEWAY?\r\n",    "AT+GATEWAY=\"192.168.30.1\"\r\n"},
    {5, "AT+DNS_SERVER?\r\n", "AT+DNS_SERVER=\"114.114.114.114\"\r\n"},      /* 配置DNS服务器以解析域名 */
    {5, "AT+MODUNAME?\r\n",   "AT+MODUNAME=\"ATK-CLOUD-MOD\"\r\n"},

    /*3.串口参数*/
    {5, "AT+UART?\r\n",             "AT+UART=\"115200\",\"1\",\"8\",\"NONE\"\r\n"},
    {5, "AT+UART_PACKLEN?\r\n",     "AT+UART_PACKLEN=\"50\"\r\n"},
    {5, "AT+UART_PACKTIME?\r\n",    "AT+UART_PACKTIME=\"50\"\r\n"},
    
    /*4.配置原子云功能：使能*/
    {5, "AT+CLOUD_LINK_EN?\r\n",   "AT+CLOUD_LINK_EN=\"ON\"\r\n"},
    {5, "AT+COLUD_DOMAIN?\r\n",    "AT+COLUD_DOMAIN=\"CLOUD.ALIENTEK.COM\"\r\n"},
    {5, "AT+CLOUD_PORT?\r\n",      "AT+CLOUD_PORT=\"59666\"\r\n"},
    {5, "AT+ATKLINKPWD?\r\n",      "AT+ATKLINKPWD=\"12345678\"\r\n"},
    {5, "AT+ATKYUNDEVID?\r\n",     "AT+ATKYUNDEVID=\"65112596110969426801\"\r\n"},

    /*5.更新模块参数，模块自动重启 */
    {5, "AT+UPDATE?\r\n",    "AT+UPDATE?\r\n"},
};

static const _u2e_atcmd_st u2e_udpmulticast_info[]={
    /* 1.开启指令回显,选择工作模式 */
    {5, "AT+ECHO?\r\n",       "AT+ECHO=\"ON\"\r\n"},
    {5, "AT+ETH_MOD?\r\n",    "AT+ETH_MOD=\"UDP_MULTICAST\"\r\n"},

    /*2.配置与本地路由匹配的工作参数*/
    {5, "AT+DHCP?\r\n",       "AT+DHCP=\"ON\"\r\n"},
//    {5, "AT+MAC_ID?\r\n",     "AT+MAC_ID=\"04-78-63-01-02-F2\"\r\n"},
    {5, "AT+STATICIP?\r\n",   "AT+STATICIP=\"192.168.30.190\"\r\n"},
    {5, "AT+NETMASK?\r\n",    "AT+NETMASK=\"255.255.255.0\"\r\n"},
    {5, "AT+GATEWAY?\r\n",    "AT+GATEWAY=\"192.168.30.1\"\r\n"},
    {5, "AT+DNS_SERVER?\r\n", "AT+DNS_SERVER=\"114.114.114.114\"\r\n"},      /* 配置DNS服务器以解析域名 */
    {5, "AT+MODUNAME?\r\n",   "AT+MODUNAME=\"ATK-CLOUD-MOD\"\r\n"},

    /*3.串口参数*/
    {5, "AT+UART?\r\n",             "AT+UART=\"115200\",\"1\",\"8\",\"NONE\"\r\n"},
    {5, "AT+UART_PACKLEN?\r\n",     "AT+UART_PACKLEN=\"50\"\r\n"},
    {5, "AT+UART_PACKTIME?\r\n",    "AT+UART_PACKTIME=\"50\"\r\n"},
    
    /*4.更新模块参数，模块自动重启 */
    {5, "AT+UPDATE?\r\n",    "AT+UPDATE?\r\n"},
};

/**
 * @brief       配置模块工作参数
 * 
 * @param       work_param      :   工作模式相关AT指令参数
 * @param       num             :   需要配置的AT指令参数数量
 * 
 * @return      0  :    所有参数配置成功
 *              n  :    第几个参数配置失败
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
        if (res == 1) /*如果模块内部参数和需要配置的参数一致，不需要重复去配置*/
        {
            continue;
        }
        else /*模块内部参数与配置参数不一致，需要更新模块内部参数*/
        {
            res = send_cmd_to_u2e((work_param + i)->write_cmd,
                                  "\r\nOK",
                                  (work_param + i)->timeout);

            if (res < 0)
            {
                return (i+1);       /* 返回指令序号，起始为1 */
            }
        }
    }

    return 0;
}

/**
 * @brief       初始化模块的工作状态
 * 
 * @param       work_mode   :   模块工作模式
 *  @arg        U2E_UDP_SERVER,      0,  UDP服务器
 *  @arg        U2E_UDP_CLIENT,      1,  UDP客户端模式
 *  @arg        U2E_TCP_SERVER,      2,  TCP服务器
 *  @arg        U2E_TCP_CLIENT,      3,  TCP客户端
 * 
 * @return      0   :   初始化成功
 *             -1   :   进入配状态失败
 *             -2   :   模块工作参数配置失败
 */

int u2e_config_init(_u2e_work_mode_eu work_mode)
{
	
	int res;
	
		
    
    GPIO_InitTypeDef gpio_initure;    /*GPIO端口设置参数存放位置*/
    U2E_MODULE_DF_CLK_ENABLE();
    U2E_MODULE_ST_CLK_ENABLE();
    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_AFIO_REMAP_SWJ_NOJTAG();    /* 关闭JTAG, 使能SWD, 这样PA15, 才可以当普通IO使用 */    

    gpio_initure.Pin = U2E_MODULE_DF_PIN;               /*串口发送引脚号*/
    gpio_initure.Mode = GPIO_MODE_OUTPUT_PP;            /*复用推挽输出*/
    gpio_initure.Pull = GPIO_PULLUP;                    /*上拉*/
    gpio_initure.Speed = GPIO_SPEED_FREQ_HIGH;          /*IO速度设置为高速*/
    HAL_GPIO_Init(U2E_MODULE_DF_PORT, &gpio_initure);
    U2E_MODULE_DF(1);                                   /* 拉高此引脚使模块正常工作 */

    gpio_initure.Pin = U2E_MODULE_ST_PIN;               /*串口发送引脚号*/
    gpio_initure.Pull = GPIO_PULLUP;                    /*上拉*/
    gpio_initure.Mode = GPIO_MODE_INPUT;                /*复用输入模式*/
    HAL_GPIO_Init(U2E_MODULE_ST_PORT, &gpio_initure);

    usart3_init(115200);                                /* 串口3初始化为115200 */


//    /* 1.复位模块进入出厂设置参数：默认，串口参数为：115200，8数据位，无校验(选用) */
//    U2E_MODULE_DF(0);
//    delay_ms(1000);
//    U2E_MODULE_DF(1);
//    //while(U2E_MODULE_ST != 1);      /* 模块进入空闲状态 */
//    delay_ms(3000);

    /*2.模块进入配置状态*/
    while ( u2e_enter_configmode() != 0 );


    /*3.配置模块的工作参数*/
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
    
    /* 等待模块配置成功，如果上一步配置更改了串口的波特率，则需要这里重新初始化STM32串口3的波特率 */
//    usart3_init(36, 921600);        /* 重新初始化串口3初始化为921600 */
    //u2e_enter_transfermode();

    return 0;
}
