/**
 ****************************************************************************************************
 * @file        demo.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MO1053 ����ʵ��
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ��Ӣ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/SRAM/sram.h"
#include "./MALLOC/malloc.h"
#include "./TEXT/text.h"
#include "./FATFS/exfuns/exfuns.h"
#include "./BSP/NORFLASH/norflash.h"
#include "./BSP/ATK_MO1053/atk_mo1053.h"
#include "./BSP/ATK_MO1053/patch_flac.h"
#include "string.h"
#include "demo.h"

/**
 * @brief       ��ʾ��Ŀ����
 * @param       index: ��ǰ����
 * @param       total: ���ļ���
 * @retval      ��
 */
static void demo_audio_index_show(uint16_t index, uint16_t total)
{
    /* ��ʾ��ǰ��Ŀ������,������Ŀ�� */
    lcd_show_xnum(10 + 0, 220, index, 3, 16, 0X80, RED);    /* ���� */
    lcd_show_char(10 + 24, 220, '/', 16, 0, RED);
    lcd_show_xnum(10 + 32, 220, total, 3, 16, 0X80, RED);   /* ����Ŀ */
    /* printf�����ǰ�ߴ�ӡ���ǣ���ǰ������ţ����ߴ�ӡ������������ */
    printf("Current song number: %d       Total number of Music Libraries:%d\r\n", index, total);
}

static void demo_audio_vol_show(uint8_t vol)
{
    lcd_show_string(10 + 110, 220, 200, 16, 16, "VOL:", RED);
    lcd_show_xnum(10 + 142, 220, vol, 2, 16, 0X80, RED);    /* ��ʾ���� */
    printf("Current volume:%d\r\n", vol);                   /* printf����д�ӡ���ǣ���ǰ���� */
}

/**
 * @brief       ATK-MO1053 �洢�����LCD UI����
 * @param       ��
 * @retval      ��
 */
static void demo_atk_mo1053_memorytest_lcd_ui_init(void)
{
    lcd_show_string(10, 140, 200, 16, 16, "Memory test", RED);
    delay_ms(1000);
}

/**
 * @brief       ATK-MO1053 ���Ҳ����LCD UI����
 * @param       ��
 * @retval      ��
 */
static void demo_atk_mo1053_sinewavetest_lcd_ui_init(void)
{
    lcd_show_string(10, 140, 200, 16, 16, "Sine wave test", RED);
    delay_ms(1000);
}

/**
 * @brief       �õ�path·����, Ŀ���ļ����ܸ���
 * @param       path: ·��
 * @retval      ����Ч�ļ�����
 */
uint16_t audio_get_tnum(char *path)
{
    uint8_t res;
    uint16_t rval = 0;
    DIR tdir;                                                   /* ��ʱĿ¼ */
    FILINFO *tfileinfo;                                         /* ��ʱ�ļ���Ϣ */

    tfileinfo = (FILINFO *)mymalloc(SRAMIN, sizeof(FILINFO));   /* �����ڴ� */
    res = f_opendir(&tdir, (const TCHAR *)path);                /* ��Ŀ¼ */

    if (res == FR_OK && tfileinfo)
    {
        while (1)                                               /* ��ѯ�ܵ���Ч�ļ��� */
        {
            res = f_readdir(&tdir, tfileinfo);                  /* ��ȡĿ¼�µ�һ���ļ� */

            if (res != FR_OK || tfileinfo->fname[0] == 0)
            {
                break;                                          /* ������/��ĩβ��,�˳� */
            }

            res = exfuns_file_type(tfileinfo->fname);
            if ((res & 0XF0) == 0X40)                           /* ȡ����λ,�����ǲ��������ļ� */
            {
                rval++;                                         /* ��Ч�ļ�������1 */
            }
        }
    }
    
    myfree(SRAMIN, tfileinfo);
    return rval;
}

/**
 * @brief       ��ʾ����ʱ��,ǧ�����ʵ���Ϣ
 * @param       lenth: �����ܳ���
 * @retval      ��
 */
void audio_msg_show(uint32_t lenth)
{
    static uint16_t playtime = 0;                                   /* ����ʱ���� */
    static uint16_t bitrate = 0;                                    /* ���������� */
    uint16_t time = 0;                                              /*  ʱ����� */
    uint16_t temp = 0;

    if (bitrate == 0)                                               /* δ���¹� */
    {
        playtime = 0;
        bitrate = atk_mo1053_get_bitrate();                         /* ��ñ����� */
    }

    time = atk_mo1053_get_decode_time();                            /* �õ�����ʱ�� */

    if (playtime == 0)
    {
        playtime = time;
    }
    else if ((time != playtime) && (time != 0))                     /* 1sʱ�䵽,������ʾ���� */
    {
        playtime = time;                                            /* ����ʱ�� */
        temp = atk_mo1053_get_bitrate();                            /* ��ñ����� */

        if (temp != bitrate)
        {
            bitrate = temp;                                         /* ����KBPS */
        }

        /* ��ʾ����ʱ�� */
        lcd_show_xnum(10, 200, time / 60, 2, 16, 0X80, RED);        /* ���� */
        lcd_show_char(10 + 16, 200, ':', 16, 0, RED);
        lcd_show_xnum(10 + 24, 200, time % 60, 2, 16, 0X80, RED);   /* ���� */
        lcd_show_char(10 + 40, 200, '/', 16, 0, RED);

        /* ��ʾ��ʱ�� */
        if (bitrate)
        {
            time = (lenth / bitrate) / 125;                         /* �õ������� (�ļ�����(�ֽ�)/(1000/8)/������=���������� */
        }
        else
        {
            time = 0;                                               /* �Ƿ�λ�� */
        }
        
        lcd_show_xnum(10 + 48, 200, time / 60, 2, 16, 0X80, RED);   /* ���� */
        lcd_show_char(10 + 64, 200, ':', 16, 0, RED);
        lcd_show_xnum(10 + 72, 200, time % 60, 2, 16, 0X80, RED);   /* ���� */

        /* ��ʾλ�� */
        lcd_show_xnum(10 + 110, 200, bitrate, 4, 16, 0X80, RED);    /* ��ʾλ�� */
        lcd_show_string(10 + 142, 200, 200, 16, 16, "Kbps", RED);
        LED0_TOGGLE ();                                             /* DS0��ת */
    }
}

/**
 * @brief       ����һ��ָ���ĸ���
 * @param       pname   : ��·�����ļ���
 * @retval      ���Ž��
 *   @arg       KEY0_PRES , ��һ��
 *   @arg       KEY1_PRES , ��һ��
 *   @arg       ����      , ����
 */
uint8_t audio_play_song(char *pname)
{
    FIL *fmp3;
    uint16_t br;
    uint8_t res, rval;
    uint8_t *databuf;
    uint16_t i = 0;
    uint8_t key;

    rval = 0;
    fmp3 = (FIL *)mymalloc(SRAMIN, sizeof(FIL));                        /* �����ڴ� */
    databuf = (uint8_t *)mymalloc(SRAMIN, 4096);                        /* ����4096�ֽڵ��ڴ����� */

    if (databuf == NULL || fmp3 == NULL)rval = 0XFF ;                   /* �ڴ�����ʧ�� */

    if (rval == 0)
    {
        atk_mo1053_restart_play();                                      /* �������� */
        atk_mo1053_set_all();                                           /* ������������Ϣ */
        atk_mo1053_reset_decode_time();                                 /* ��λ����ʱ�� */
        res = exfuns_file_type(pname);                                  /* �õ��ļ���׺ */
                            
        if (res == T_FLAC)                                              /* �����flac,����patch */
        {
            atk_mo1053_load_patch((uint16_t *)vs1053b_patch, VS1053B_PATCHLEN);
        }

        res = f_open(fmp3, (const TCHAR *)pname, FA_READ);              /* ���ļ� */

        if (res == 0)                                                   /* �򿪳ɹ� */
        {
            atk_mo1053_spi_speed_high();                                /* ���� */

            while (rval == 0)
            {
                res = f_read(fmp3, databuf, 4096, (UINT *)&br);         /* ����4096���ֽ� */
                i = 0;

                do                                                      /* ������ѭ�� */
                {
                    if (atk_mo1053_send_music_data(databuf + i) == 0)   /* ��VS10XX������Ƶ���� */
                    {
                        i += 32;
                    }
                    else
                    {
                        key = key_scan(0);

                        switch (key)
                        {
                            case KEY0_PRES:
                            {
                                rval=1;                                 /* ��һ�� */
                                break;
                            }
                            case KEY1_PRES: 
                            {
                                rval=2;                                 /* ��һ�� */
                                break;
                            }
                            
                        }
                        audio_msg_show(fmp3->obj.objsize);              /* ��ʾ��Ϣ */
                    }
                } while (i < 4096);                                     /* ѭ������4096���ֽ� */

                if (br != 4096 || res != 0)
                {
                    rval = KEY0_PRES;
                    break;                                              /* ������ */
                }
            }

            f_close(fmp3);
        }
        else
        {
            rval = 0XFF;                                                /* ���ִ��� */
        }
    }
    
    myfree(SRAMIN, databuf);
    myfree(SRAMIN, fmp3);
    return rval;
}


/**
 * @brief       ATK-MO1053 ����������
 * @param       ��
 * @retval      ��
 */
static void demo_sensor_test(void)
{
    uint8_t i = 0;
    uint8_t res;
    DIR mp3dir;                                                                             /* Ŀ¼ */
    FILINFO *mp3fileinfo;                                                                   /* �ļ���Ϣ */
    char *pname;                                                                            /* ��·�����ļ��� */
    uint16_t totmp3num;                                                                     /* �����ļ����� */
    uint16_t curindex;                                                                      /* ͼƬ��ǰ���� */
    uint8_t key;                                                                            /* ��ֵ */
    uint16_t temp;                                                              
    uint16_t *mp3offsettbl;                                                                 /* ���������� */
   
    while (f_opendir(&mp3dir, "0:/MUSIC"))                                                  /* ��ͼƬ�ļ��� */
    {
        lcd_show_string(10, 160, 200, 16, 16, "MUSIC file folder error", RED);
        delay_ms(200);
        lcd_fill(10, 160, 240, 206, WHITE);                                                 /* �����ʾ */
        delay_ms(200);
    }
    
    totmp3num = audio_get_tnum("0:/MUSIC");                                                 /* �õ�����Ч�ļ��� */
    
    while (totmp3num == NULL)                                                               /* �����ļ�����Ϊ0 */
    {
        lcd_show_string(10, 160, 200, 16, 16, "No music files", RED);
        delay_ms(200);
        lcd_fill(30, 190, 240, 146, WHITE);                                                 /* �����ʾ */
        delay_ms(200);
    }
    
    mp3fileinfo = (FILINFO *)mymalloc(SRAMIN, sizeof(FILINFO));                             /* Ϊ���ļ������������ڴ� */
    pname = mymalloc(SRAMIN,  2 * FF_MAX_LFN + 1);                                          /* Ϊ��·�����ļ��������ڴ� */
    mp3offsettbl = mymalloc(SRAMIN, 2 * totmp3num);                                         /* ����2*totmp3num���ֽڵ��ڴ�, ���ڴ�������ļ����� */
    
    while (mp3fileinfo == NULL || pname == NULL || mp3offsettbl == NULL)                    /* �ڴ������� */
    {
        lcd_show_string(10, 160, 200, 16, 16, "memory allocation failed", RED);
        delay_ms(200);
        lcd_fill(30, 190, 240, 146, WHITE);                                                 /* �����ʾ */
        delay_ms(200);
    }
    
    atk_mo1053_reset();
    atk_mo1053_soft_reset();
    
    vsset.mvol = 220;                                                                       /* Ĭ����������Ϊ220 */
    demo_audio_vol_show((vsset.mvol - 100) / 5);                                            /* ����������: 100~250, ��ʾ��ʱ��, ���չ�ʽ(vol-100)/5, ��ʾ, Ҳ����0~30 */
    
    /* ��¼���� */
    res = f_opendir(&mp3dir, "0:/MUSIC");                                                   /* ��Ŀ¼ */
    
    if (res == FR_OK)
    {
        curindex = 0;                                                                       /* ��ǰ����Ϊ0 */
        
        while (1)                                                                           /* ȫ����ѯһ�� */
        {
            temp = mp3dir.dptr;                                                             /* ��¼��ǰoffset */
            res = f_readdir(&mp3dir, mp3fileinfo);                                          /* ��ȡĿ¼�µ�һ���ļ� */
            
            if (res != FR_OK || mp3fileinfo->fname[0] == 0)
            {
                break;                                                                      /* ������/��ĩβ��,�˳� */
            }
            
            res = exfuns_file_type(mp3fileinfo->fname);
            
            if ((res & 0XF0) == 0X40)                                                       /* ȡ����λ,�����ǲ��������ļ� */
            {
                mp3offsettbl[curindex] = temp;                                              /* ��¼���� */
                curindex++;
            }
        }
    }
    
    curindex = 0;                                                                           /* ��0��ʼ��ʾ */
    res = f_opendir(&mp3dir, (const TCHAR *)"0:/MUSIC");                                    /* ��Ŀ¼ */
    
    while (res == FR_OK)                                                                    /* �򿪳ɹ� */
    {
        dir_sdi(&mp3dir, mp3offsettbl[curindex]);                                           /* �ı䵱ǰĿ¼���� */
        res = f_readdir(&mp3dir, mp3fileinfo);                                              /* ��ȡĿ¼�µ�һ���ļ� */
        
        if (res != FR_OK || mp3fileinfo->fname[0] == 0)
        {
            break;                                                                          /* ������/��ĩβ��,�˳� */
        }
        
        strcpy((char *)pname, "0:/MUSIC/");                                                 /* ����·��(Ŀ¼) */
        strcat((char *)pname, (const char *)mp3fileinfo->fname);                            /* ���ļ������ں��� */
        lcd_fill(10, 180, lcddev.width, 190 + 16, WHITE);                                   /* ���֮ǰ����ʾ */
        text_show_string(10, 180, lcddev.width - 30, 16, mp3fileinfo->fname, 16, 0, RED);   /* ��ʾ�������� */
        printf("********************************\r\n");
        printf("Playing: %s     Kilobit rate: 1411Kbps\r\n", mp3fileinfo->fname);           /* printf����д�ӡ���ǣ����ڲ����Լ������� */
        demo_audio_index_show(curindex + 1, totmp3num);
        printf("\r\n");
        key = audio_play_song(pname);                                                       /* �������MP3 */
        
         if (key == KEY1_PRES)                                                              /* ��һ�� */
            {
                if (curindex)
                {
                    curindex--;
                }
                else
                {
                    curindex = totmp3num - 1;
                }
            }
            else if (key == KEY0_PRES)                                                      /* ��һ�� */
            {
                curindex++;
                
                if (curindex >= totmp3num)
                {
                    curindex = 0;                                                           /* ��ĩβ��ʱ��, �Զ���ͷ��ʼ */
                }
            }
            else
            {
                break;                                                                      /* �����˴��� */
            }
            
            delay_ms(10);
            i++;
            
            if (i == 25)
            {
                LED0_TOGGLE();
                i = 0;
            }
    }
    myfree(SRAMIN, mp3fileinfo);                                                            /* �ͷ��ڴ� */
    myfree(SRAMIN, pname);                                                                  /* �ͷ��ڴ� */
    myfree(SRAMIN, mp3offsettbl);                                                           /* �ͷ��ڴ� */
}


/**
 * @brief       ������ʾ��ں���
 * @param       ��
 * @retval      ��
 */
void demo_run(void)
{
    uint8_t ret;
    
    /* ��ʼ��ATK-MO1053 */
    ret = atk_mo1053_init();
    if (ret != 0)
    {
        printf("ATK-MO1053 init failed!\r\n");
        while(1)
        {
            lcd_show_string(10, 97, 200, 16, 16, "MO1053 Error!!!", RED);
            delay_ms(500);
            lcd_show_string(10, 97, 200, 16, 16, "                  ", WHITE);
            delay_ms(500);
        }
    }
    
    /* ATK-MO1053 SRAM��ʼ�� */
    ret = sram_init();
    if (ret != 0)
    {
        printf("ATK-MO1053 sram_init failed!\r\n");
        while(1)
        {
            LED0_TOGGLE();
            delay_ms(200);
        }
    }
    /* ��ʼ���ڲ�SRAM�ڴ�� */
    my_mem_init(SRAMIN);
    /* ��ʼ���ⲿSRAM�ڴ�� */
    my_mem_init(SRAMEX);
    /* Ϊfatfs��ر��������ڴ� */
    exfuns_init();
    
    f_mount(fs[0], "0:", 1);                                /* ����SD�� */
    f_mount(fs[1], "1:", 1);                                /* ����FLASH */
    
    /* ATK-MO1053 �ֿ��ʼ�� */
    ret = fonts_init();
    
    if (ret != 0)                                            /* ����ֿ� */
    {
        lcd_show_string(10, 160, 200, 16, 16, "Font Error!", RED);
        delay_ms(200);
        lcd_fill(10, 160, 240, 66, WHITE);                   /* �����ʾ */
        delay_ms(200);
    }
    
    printf("MO1053 OK\r\n");
    lcd_show_string(10, 97, 200, 16, 16, "MO1053 OK", RED);
    
    demo_atk_mo1053_memorytest_lcd_ui_init();
    printf("Ram Test:0X%04X\r\n", atk_mo1053_ram_test());   /* ��ӡRAM���Խ�� */
    demo_atk_mo1053_sinewavetest_lcd_ui_init();
    atk_mo1053_sine_test();
    lcd_show_string(10, 140, 200, 16, 16, "                  ", WHITE);
    
    while (1)
    {
        demo_sensor_test();
        LED0_TOGGLE();
        delay_ms(500);
    }
}
