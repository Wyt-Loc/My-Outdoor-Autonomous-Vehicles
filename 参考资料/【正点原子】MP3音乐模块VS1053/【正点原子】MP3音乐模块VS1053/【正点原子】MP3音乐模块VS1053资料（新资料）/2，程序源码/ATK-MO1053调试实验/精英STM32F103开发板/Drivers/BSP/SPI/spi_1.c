/**
 ****************************************************************************************************
 * @file        spi.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       SPI ��������
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20200424
 * ��һ�η���
 * V1.1 20200429
 * 1, ���SPI1֧��
 * 2, ���spi1_init/spi1_set_speed��spi1_read_write_byte��������
 * V1.2 20200612
 * spi�� �շ�����ֱ��ʹ�üĴ���������ʹ��HAL����շ����� HAL_SPI_TransmitReceive Ч��̫��ֱ�ӵ��²����쳣
 * �������ǲ�ʹ��Systick�������������ó�ʱֵΪ0ʱSPIֱ�Ӳ���ʧ�ܣ����г�ʱ�޷��˳������⣬
 * �����ֱ��ʹ�üĴ������������������Ƶ����92Mhz���ϵĻ��ſ�����������
 ****************************************************************************************************
 */

#include "./BSP/SPI/spi.h"

SPI_HandleTypeDef g_spi2_handler;             /* SPI2��� */
SPI_HandleTypeDef g_spi1_handler;             /* SPI1��� */

/**
 * @brief       SPI��ʼ������
 *   @note      ����ģʽ,8λ����,��ֹӲ��Ƭѡ
 * @param       ��
 * @retval      ��
 */
void spi2_init(void)
{
    SPI2_SPI_CLK_ENABLE();          /* SPI2ʱ��ʹ�� */

    g_spi2_handler.Instance = SPI2_SPI;                                /* SPI2 */
    g_spi2_handler.Init.Mode = SPI_MODE_MASTER;                        /* ����SPI����ģʽ������Ϊ��ģʽ */
    g_spi2_handler.Init.Direction = SPI_DIRECTION_2LINES;              /* ����SPI�������˫�������ģʽ:SPI����Ϊ˫��ģʽ */
    g_spi2_handler.Init.DataSize = SPI_DATASIZE_8BIT;                  /* ����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ */
    g_spi2_handler.Init.CLKPolarity = SPI_POLARITY_HIGH;               /* ����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ */
    g_spi2_handler.Init.CLKPhase = SPI_PHASE_2EDGE;                    /* ����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ����� */
    g_spi2_handler.Init.NSS = SPI_NSS_SOFT;                            /* NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ���� */
    g_spi2_handler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256; /* ���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256 */
    g_spi2_handler.Init.FirstBit = SPI_FIRSTBIT_MSB;                   /* ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ */
    g_spi2_handler.Init.TIMode = SPI_TIMODE_DISABLE;                   /* �ر�TIģʽ */
    g_spi2_handler.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;   /* �ر�Ӳ��CRCУ�� */
    g_spi2_handler.Init.CRCPolynomial = 7;                             /* CRCֵ����Ķ���ʽ */
    HAL_SPI_Init(&g_spi2_handler);                                     /* ��ʼ�� */

    __HAL_SPI_ENABLE(&g_spi2_handler); /* ʹ��SPI2 */

    spi2_read_write_byte(0Xff);      /* ��������, ʵ���Ͼ��ǲ���8��ʱ������, �ﵽ���DR������, �Ǳ��� */
}

/**
 * @brief       SPI�ײ�������ʱ��ʹ�ܣ���������
 *   @note      �˺����ᱻHAL_SPI_Init()����
 * @param       hspi:SPI���
 * @retval      ��
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef GPIO_Initure;
    if(hspi->Instance == SPI2_SPI)
    {
        SPI2_SCK_GPIO_CLK_ENABLE();     /* SPI2_SCK��ʱ��ʹ�� */
        SPI2_MISO_GPIO_CLK_ENABLE();    /* SPI2_MISO��ʱ��ʹ�� */
        SPI2_MOSI_GPIO_CLK_ENABLE();    /* SPI2_MOSI��ʱ��ʹ�� */

        /* SCK����ģʽ����(�������) */
        GPIO_Initure.Pin = SPI2_SCK_GPIO_PIN;
        GPIO_Initure.Mode = GPIO_MODE_AF_PP;       
        GPIO_Initure.Pull = GPIO_PULLUP;           
        GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH; 
        HAL_GPIO_Init(SPI2_SCK_GPIO_PORT, &GPIO_Initure);
        
        /* MISO����ģʽ����(��������) */
        GPIO_Initure.Pin = SPI2_MISO_GPIO_PIN ;
        GPIO_Initure.Mode = GPIO_MODE_AF_INPUT;
        HAL_GPIO_Init(SPI2_MISO_GPIO_PORT, &GPIO_Initure);
        
        /* MOSI����ģʽ����(�������) */
        GPIO_Initure.Pin = SPI2_MOSI_GPIO_PIN;
        GPIO_Initure.Mode = GPIO_MODE_AF_PP;       
        HAL_GPIO_Init(SPI2_MOSI_GPIO_PORT, &GPIO_Initure);

    }
    else if(hspi->Instance == SPI1_SPI)
    {
        SPI1_SCK_GPIO_CLK_ENABLE();     /* SPI1_SCK  ��ʱ��ʹ�� */
        SPI1_MISO_GPIO_CLK_ENABLE();    /* SPI1_MISO ��ʱ��ʹ�� */
        SPI1_MOSI_GPIO_CLK_ENABLE();    /* SPI1_MOSI ��ʱ��ʹ�� */

        /* SCK����ģʽ����(�������) */
        GPIO_Initure.Pin = SPI1_SCK_GPIO_PIN;
        GPIO_Initure.Mode = GPIO_MODE_AF_PP;       
        GPIO_Initure.Pull = GPIO_PULLUP;           
        GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH; 
        HAL_GPIO_Init(SPI1_SCK_GPIO_PORT, &GPIO_Initure);
        
        /* MISO����ģʽ����(��������) */
        GPIO_Initure.Pin = SPI1_MISO_GPIO_PIN ;
        GPIO_Initure.Mode = GPIO_MODE_AF_INPUT;
        HAL_GPIO_Init(SPI1_MISO_GPIO_PORT, &GPIO_Initure);
        
        /* MOSI����ģʽ����(�������) */
        GPIO_Initure.Pin = SPI1_MOSI_GPIO_PIN;
        GPIO_Initure.Mode = GPIO_MODE_AF_PP;       
        HAL_GPIO_Init(SPI1_MOSI_GPIO_PORT, &GPIO_Initure);
    }
}


/**
 * @brief       SPI2�ٶ����ú���
 *   @note      SPI2ʱ��ѡ������APB1, ��PCLK1, Ϊ36Mhz
 *              SPI�ٶ� = PCLK1 / 2^(speed + 1)
 * @param       speed   : SPI2ʱ�ӷ�Ƶϵ��
                        ȡֵΪSPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_2 256
 * @retval      ��
 */
void spi2_set_speed(uint8_t speed)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(speed)); /* �ж���Ч�� */
    __HAL_SPI_DISABLE(&g_spi2_handler);             /* �ر�SPI */
    g_spi2_handler.Instance->CR1 &= 0XFFC7;         /* λ3-5���㣬�������ò����� */
    g_spi2_handler.Instance->CR1 |= speed << 3;     /* ����SPI�ٶ� */
    __HAL_SPI_ENABLE(&g_spi2_handler);              /* ʹ��SPI */
}

/**
 * @brief       SPI2��дһ���ֽ�����
 * @param       txdata  : Ҫ���͵�����(1�ֽ�)
 * @retval      ���յ�������(1�ֽ�)
 */
uint8_t spi2_read_write_byte(uint8_t txdata)
{
    uint8_t rxdata;
    HAL_SPI_TransmitReceive(&g_spi2_handler, &txdata, &rxdata, 1, 1000);
    return rxdata; /* �����յ������� */
}

/***************************************************************************************************************************/
/**
 * @brief       SPI1��ʼ������
 *   @note      ����ģʽ,8λ����,��ֹӲ��Ƭѡ
 * @param       ��
 * @retval      ��
 */
void spi1_init(void)
{
    SPI1_SPI_CLK_ENABLE();          /* SPI1      ʱ��ʹ�� */

    g_spi1_handler.Instance = SPI1_SPI;                                /* SPI1 */
    g_spi1_handler.Init.Mode = SPI_MODE_MASTER;                        /* ����SPI����ģʽ������Ϊ��ģʽ */
    g_spi1_handler.Init.Direction = SPI_DIRECTION_2LINES;              /* ����SPI�������˫�������ģʽ:SPI����Ϊ˫��ģʽ */
    g_spi1_handler.Init.DataSize = SPI_DATASIZE_8BIT;                  /* ����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ */
    g_spi1_handler.Init.CLKPolarity = SPI_POLARITY_HIGH;               /* ����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ */
    g_spi1_handler.Init.CLKPhase = SPI_PHASE_2EDGE;                    /* ����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ����� */
    g_spi1_handler.Init.NSS = SPI_NSS_SOFT;                            /* NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ���� */
    g_spi1_handler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256; /* ���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256 */
    g_spi1_handler.Init.FirstBit = SPI_FIRSTBIT_MSB;                   /* ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ */
    g_spi1_handler.Init.TIMode = SPI_TIMODE_DISABLE;                   /* �ر�TIģʽ */
    g_spi1_handler.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;   /* �ر�Ӳ��CRCУ�� */
    g_spi1_handler.Init.CRCPolynomial = 7;                             /* CRCֵ����Ķ���ʽ */
    HAL_SPI_Init(&g_spi1_handler);                                     /* ��ʼ�� */

    __HAL_SPI_ENABLE(&g_spi1_handler); /* ʹ��SPI1 */

    spi1_read_write_byte(0Xff);      /* ��������, ʵ���Ͼ��ǲ���8��ʱ������, �ﵽ���DR������, �Ǳ��� */

}

/**
 * @brief       SPI1�ٶ����ú���
 *   @note      SPI1ʱ��ѡ������APB2, ��PCLK2, Ϊ72Mhz
 *              SPI�ٶ� = PCLK1 / 2^(speed + 1)
 * @param       speed   : SPI1ʱ�ӷ�Ƶϵ��
 * @retval      ��
 */
void spi1_set_speed(uint8_t speed)
{
    speed &= 0X07;                                  /* ���Ʒ�Χ */
    __HAL_SPI_DISABLE(&g_spi1_handler);             /* �ر�SPI */
    g_spi1_handler.Instance->CR1 &= 0XFFC7;         /* λ3-5���㣬�������ò����� */
    g_spi1_handler.Instance->CR1 |= speed << 3;     /* ����SPI�ٶ� */
    __HAL_SPI_ENABLE(&g_spi1_handler);              /* ʹ��SPI */
}

/**
 * @brief       SPI1��дһ���ֽ�����
 * @param       txdata  : Ҫ���͵�����(1�ֽ�)
 * @retval      ���յ�������(1�ֽ�)
 */
uint8_t spi1_read_write_byte(uint8_t txdata)
{
    while ((g_spi1_handler.Instance->SR & 1 << 1) == 0);   /* �ȴ��������� */
    g_spi1_handler.Instance->DR = txdata;                  /* ����һ��byte */
    while ((g_spi1_handler.Instance->SR & 1 << 0) == 0);   /* �ȴ�������һ��byte */
    return g_spi1_handler.Instance->DR;                    /* �����յ������� */
}










