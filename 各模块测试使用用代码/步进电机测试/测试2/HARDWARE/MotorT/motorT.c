#include "motorT.h"
#include "math.h"
#include "stdio.h"
#include "dma.h"



/********************************************���μӼ���***********************************************/
speedRampData g_srd               = {STOP,CW,0,0,0,0,0};  /* �Ӽ��ٱ��� */  	
__IO int32_t  g_step_position     = 0;                    /* ��ǰλ�� */
__IO uint8_t  g_motion_sta        = 0;                    /* �Ƿ����˶���0��ֹͣ��1���˶� */
__IO uint32_t g_add_pulse_count   = 0;                    /* ��������ۼ� */
extern u16 send_buf[1];


/*
 * @brief       ���������˶����Ʋ���
 * @param       step���ƶ��Ĳ��� (����Ϊ˳ʱ�룬����Ϊ��ʱ��).
 * @param       accel  ���ٶ�,ʵ��ֵΪaccel*0.1*rad/sec^2  10������2��������һ������������
 * @param       decel  ���ٶ�,ʵ��ֵΪdecel*0.1*rad/sec^2
 * @param       speed  ����ٶ�,ʵ��ֵΪspeed*0.1*rad/sec
 * @retval      ��
 */
void myStepMotorT(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed){
	
	  __IO uint16_t tim_count;        /* �ﵽ����ٶ�ʱ�Ĳ���*/
    __IO uint32_t max_s_lim;        /* ����Ҫ��ʼ���ٵĲ������������û�дﵽ����ٶȣ�*/
    __IO uint32_t accel_lim;
	
    if(g_motion_sta != STOP)        /* ֻ�����������ֹͣ��ʱ��ż���*/
        return;
		
		if(step < 0)                    /* ����Ϊ���� */
    {
        g_srd.dir = CCW;            /* ��ʱ�뷽����ת */
				//����dir��ת//ST3_DIR(CCW);				
        step = -step;               /* ��ȡ��������ֵ */
    }
    else
    {
        g_srd.dir = CW;             /* ˳ʱ�뷽����ת */
        //����dir��ת//ST3_DIR(CW);
    }
		
		if(step == 1)                   /* ����Ϊ1 */
    {
        g_srd.accel_count = -1;     /* ֻ�ƶ�һ�� */
        g_srd.run_state = DECEL;    /* ����״̬. */
        g_srd.step_delay = 1000;    /* Ĭ���ٶ� */
    }
		
		else if(step != 0){
			
			/*��������ٶȼ���, ����õ�min_delay���ڶ�ʱ���ļ�������ֵ min_delay = (alpha / t)/ w*/
        g_srd.min_delay = (int32_t)(A_T_x10 /speed); //��������ʱ�ļ���ֵ

        /* ͨ�������һ��(c0) �Ĳ�����ʱ���趨���ٶȣ�����accel��λΪ0.1rad/sec^2
         step_delay = 1/tt * sqrt(2*alpha/accel)
         step_delay = ( tfreq*0.69/10 )*10 * sqrt( (2*alpha*100000) / (accel*10) )/100 */
        
        g_srd.step_delay = (int32_t)((T1_FREQ_148 * sqrt(A_SQ / accel))/10); /* c0 */
				
				printf("c0 = %d\r\n", g_srd.min_delay);

        max_s_lim = (uint32_t)(speed*speed / (A_x200*accel/10));/* ������ٲ�֮��ﵽ����ٶȵ����� max_s_lim = speed^2 / (2*alpha*accel) */
			
				printf("s_lim = %d\r\n", max_s_lim);		//����Ҫ��ʼ���ٵĲ���

        if(max_s_lim == 0)                                      /* ����ﵽ����ٶ�С��0.5�������ǽ���������Ϊ0,��ʵ�����Ǳ����ƶ�����һ�����ܴﵽ��Ҫ���ٶ� */
        {
            max_s_lim = 1;
        }
        accel_lim = (uint32_t)(step*decel/(accel+decel));       /* ���ﲻ��������ٶ� ������ٲ�֮�����Ǳ��뿪ʼ���� n1 = (n1+n2)decel / (accel + decel) */

        if(accel_lim == 0)                                      /* ����һ�� ��һ������*/
        {
            accel_lim = 1;
        }
        if(accel_lim <= max_s_lim)                              /* ���ٽ׶ε���������ٶȾ͵ü��١�����ʹ�������������ǿ��Լ�������ٽ׶β��� */
        {
            g_srd.decel_val = accel_lim - step;                 /* ���ٶεĲ��� */
        }
        else
        {
            g_srd.decel_val = -(max_s_lim*accel/decel);         /* ���ٶεĲ��� */
        }
        if(g_srd.decel_val == 0)                                /* ����һ�� ��һ������ */
        {
            g_srd.decel_val = -1;
        }
        g_srd.decel_start = step + g_srd.decel_val;             /* ���㿪ʼ����ʱ�Ĳ��� */
        
        if(g_srd.step_delay <= g_srd.min_delay)                 /* ���һ��ʼc0���ٶȱ����ٶ��ٶȻ��󣬾Ͳ���Ҫ���м����˶���ֱ�ӽ������� */
        {
            g_srd.step_delay = g_srd.min_delay;
            g_srd.run_state = RUN;
        }
        else  
        {
            g_srd.run_state = ACCEL;
        }
        g_srd.accel_count = 0;                                  /* ��λ�Ӽ��ټ���ֵ */

		}

		g_motion_sta = 1;
		//ST3_EN(ON);//����һ��ENʹ�����ź���
		tim_count = TIM_GetCounter(TIM3);
		send_buf[0] = tim_count;
		DMA_Enable(DMA1_Channel6);
		//TIM_SetCompare1(TIM3,tim_count+g_srd.step_delay/2.0f);
		TIM_Cmd(TIM3, ENABLE);  /* ʹ��TIM3 */
		
}



void TIM3_IRQHandler(void)
{
	
		__IO uint32_t tim_count = 0;
    __IO uint32_t tmp = 0;
    uint16_t new_step_delay = 0;                            /* �����£��£�һ����ʱ���� */
    __IO static uint16_t last_accel_delay = 0;              /* ���ٹ��������һ����ʱ���������ڣ� */
    __IO static uint32_t step_count = 0;                    /* ���ƶ�����������*/
    __IO static int32_t rest = 0;                           /* ��¼new_step_delay�е������������һ������ľ��� */
    __IO static uint8_t i = 0;                              /* ��ʱ��ʹ�÷�תģʽ����Ҫ���������жϲ����һ���������� */
	
    if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
    {
			tim_count = TIM_GetCounter(TIM3);
			tmp = tim_count + g_srd.step_delay/2.0f;               /* ����Cֵ�������Ҫ��ת���ε�������Ҫ����2 */
			send_buf[0] = tmp;
			DMA_Enable(DMA1_Channel6);
			//while(DMA_send_feedback(DMA1_Channel6)){}; //��������˳�

			//TIM_SetCompare1(TIM3,tmp);
			i++;                                                /* ��ʱ���жϴ�������ֵ */
        if(i == 2)                                          /* 2�Σ�˵���Ѿ����һ���������� */
        {
            i = 0;                                          /* ���㶨ʱ���жϴ�������ֵ */
            switch(g_srd.run_state)                         /* �Ӽ������߽׶� */
            {
            case STOP:
                step_count = 0;                             /* ���㲽�������� */
                rest = 0;                                   /* ������ֵ */
                /* �ر�ͨ��*/
								TIM_Cmd(TIM3, DISABLE);  /* ʹ��TIM3 */
								//ST3_EN(EN_OFF); //���ͣ
								g_motion_sta = 0;                           /* ���Ϊֹͣ״̬  */
								break;
						
						case ACCEL:
                g_add_pulse_count++;                        /* ֻ���ڼ�¼���λ��ת���˶��ٶ� */
                step_count++;                               /* ������1*/
                if(g_srd.dir == CW)
                {
                    g_step_position++;                      /* ����λ�ü�1  ��¼����λ��ת�����ٶ�*/
                }
                else
                {
                    g_step_position--;                      /* ����λ�ü�1*/
                }
                g_srd.accel_count++;                        /* ���ټ���ֵ��1*/
                new_step_delay = g_srd.step_delay - (((2 *g_srd.step_delay) + rest)/(4 * g_srd.accel_count + 1));/* ������(��)һ����������(ʱ����) */
                rest = ((2 * g_srd.step_delay)+rest)%(4 * g_srd.accel_count + 1);                                /* �����������´μ��㲹��������������� */
                if(step_count >= g_srd.decel_start)         /* ����Ƿ�����Ҫ���ٵĲ��� */
                {
                    g_srd.accel_count = g_srd.decel_val;    /* ���ټ���ֵΪ���ٽ׶μ���ֵ�ĳ�ʼֵ */
                    g_srd.run_state = DECEL;                /* �¸����������ٽ׶� */
                }
                else if(new_step_delay <= g_srd.min_delay)  /* ����Ƿ񵽴�����������ٶ� ����ֵԽС�ٶ�Խ�죬������ٶȺ�����ٶ���Ȼ����ͽ�������*/
                {
                    last_accel_delay = new_step_delay;      /* ������ٹ��������һ����ʱ���������ڣ�*/
                    new_step_delay = g_srd.min_delay;       /* ʹ��min_delay����Ӧ����ٶ�speed��*/
                    rest = 0;                               /* ������ֵ */
                    g_srd.run_state = RUN;                  /* ����Ϊ��������״̬ */
                }
                break;

            case RUN:
                g_add_pulse_count++;
                step_count++;                               /* ������1 */
                if(g_srd.dir == CW)
                {
                    g_step_position++;                      /* ����λ�ü�1 */
                }
                else
                {
                    g_step_position--;                      /* ����λ�ü�1*/
                }
                new_step_delay = g_srd.min_delay;           /* ʹ��min_delay����Ӧ����ٶ�speed��*/
                if(step_count >= g_srd.decel_start)         /* ��Ҫ��ʼ���� */
                {
                    g_srd.accel_count = g_srd.decel_val;    /* ���ٲ�����Ϊ���ټ���ֵ */
                    new_step_delay = last_accel_delay;      /* �ӽ׶�������ʱ��Ϊ���ٽ׶ε���ʼ��ʱ(��������) */
                    g_srd.run_state = DECEL;                /* ״̬�ı�Ϊ���� */
                }
                break;

            case DECEL:
                step_count++;                               /* ������1 */
                g_add_pulse_count++;
                if(g_srd.dir == CW)
                {
                    g_step_position++;                      /* ����λ�ü�1 */
                }
                else
                {
                    g_step_position--;                      /* ����λ�ü�1 */
                }
                g_srd.accel_count++;
                new_step_delay = g_srd.step_delay - (((2 * g_srd.step_delay) + rest)/(4 * g_srd.accel_count + 1));  /* ������(��)һ����������(ʱ����) */
                rest = ((2 * g_srd.step_delay)+rest)%(4 * g_srd.accel_count + 1);                                   /* �����������´μ��㲹��������������� */

                /* ����Ƿ�Ϊ���һ�� */
                if(g_srd.accel_count >= 0)                  /* �жϼ��ٲ����Ƿ�Ӹ�ֵ�ӵ�0�ǵĻ� ������� */
                {
                    g_srd.run_state = STOP;
                }
                break;
            }
            g_srd.step_delay = new_step_delay;              /* Ϊ�¸�(�µ�)��ʱ(��������)��ֵ */
        }
			/*
			++i;
			if(i % 2){
				m.pulsesNum++;	//ÿ�������ж� ������+1
				i = 0;
			}
			//printf("num = %d", m.pulsesNum);
			//if(m.pulsesNum == )
			*/
    }
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
}



