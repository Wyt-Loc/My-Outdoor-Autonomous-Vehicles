#include "motorT.h"
#include "math.h"
#include "stdio.h"
#include "dma.h"



/********************************************梯形加减速***********************************************/
speedRampData g_srd               = {STOP,CW,0,0,0,0,0};  /* 加减速变量 */  	
__IO int32_t  g_step_position     = 0;                    /* 当前位置 */
__IO uint8_t  g_motion_sta        = 0;                    /* 是否在运动？0：停止，1：运动 */
__IO uint32_t g_add_pulse_count   = 0;                    /* 脉冲个数累计 */
extern u16 send_buf[1];


/*
 * @brief       生成梯形运动控制参数
 * @param       step：移动的步数 (正数为顺时针，负数为逆时针).
 * @param       accel  加速度,实际值为accel*0.1*rad/sec^2  10倍并且2个脉冲算一个完整的周期
 * @param       decel  减速度,实际值为decel*0.1*rad/sec^2
 * @param       speed  最大速度,实际值为speed*0.1*rad/sec
 * @retval      无
 */
void myStepMotorT(int32_t step, uint32_t accel, uint32_t decel, uint32_t speed){
	
	  __IO uint16_t tim_count;        /* 达到最大速度时的步数*/
    __IO uint32_t max_s_lim;        /* 必须要开始减速的步数（如果加速没有达到最大速度）*/
    __IO uint32_t accel_lim;
	
    if(g_motion_sta != STOP)        /* 只允许步进电机在停止的时候才继续*/
        return;
		
		if(step < 0)                    /* 步数为负数 */
    {
        g_srd.dir = CCW;            /* 逆时针方向旋转 */
				//设置dir正转//ST3_DIR(CCW);				
        step = -step;               /* 获取步数绝对值 */
    }
    else
    {
        g_srd.dir = CW;             /* 顺时针方向旋转 */
        //设置dir反转//ST3_DIR(CW);
    }
		
		if(step == 1)                   /* 步数为1 */
    {
        g_srd.accel_count = -1;     /* 只移动一步 */
        g_srd.run_state = DECEL;    /* 减速状态. */
        g_srd.step_delay = 1000;    /* 默认速度 */
    }
		
		else if(step != 0){
			
			/*设置最大速度极限, 计算得到min_delay用于定时器的计数器的值 min_delay = (alpha / t)/ w*/
        g_srd.min_delay = (int32_t)(A_T_x10 /speed); //匀速运行时的计数值

        /* 通过计算第一个(c0) 的步进延时来设定加速度，其中accel单位为0.1rad/sec^2
         step_delay = 1/tt * sqrt(2*alpha/accel)
         step_delay = ( tfreq*0.69/10 )*10 * sqrt( (2*alpha*100000) / (accel*10) )/100 */
        
        g_srd.step_delay = (int32_t)((T1_FREQ_148 * sqrt(A_SQ / accel))/10); /* c0 */
				
				printf("c0 = %d\r\n", g_srd.min_delay);

        max_s_lim = (uint32_t)(speed*speed / (A_x200*accel/10));/* 计算多少步之后达到最大速度的限制 max_s_lim = speed^2 / (2*alpha*accel) */
			
				printf("s_lim = %d\r\n", max_s_lim);		//必须要开始减速的步数

        if(max_s_lim == 0)                                      /* 如果达到最大速度小于0.5步，我们将四舍五入为0,但实际我们必须移动至少一步才能达到想要的速度 */
        {
            max_s_lim = 1;
        }
        accel_lim = (uint32_t)(step*decel/(accel+decel));       /* 这里不限制最大速度 计算多少步之后我们必须开始减速 n1 = (n1+n2)decel / (accel + decel) */

        if(accel_lim == 0)                                      /* 不足一步 按一步处理*/
        {
            accel_lim = 1;
        }
        if(accel_lim <= max_s_lim)                              /* 加速阶段到不了最大速度就得减速。。。使用限制条件我们可以计算出减速阶段步数 */
        {
            g_srd.decel_val = accel_lim - step;                 /* 减速段的步数 */
        }
        else
        {
            g_srd.decel_val = -(max_s_lim*accel/decel);         /* 减速段的步数 */
        }
        if(g_srd.decel_val == 0)                                /* 不足一步 按一步处理 */
        {
            g_srd.decel_val = -1;
        }
        g_srd.decel_start = step + g_srd.decel_val;             /* 计算开始减速时的步数 */
        
        if(g_srd.step_delay <= g_srd.min_delay)                 /* 如果一开始c0的速度比匀速段速度还大，就不需要进行加速运动，直接进入匀速 */
        {
            g_srd.step_delay = g_srd.min_delay;
            g_srd.run_state = RUN;
        }
        else  
        {
            g_srd.run_state = ACCEL;
        }
        g_srd.accel_count = 0;                                  /* 复位加减速计数值 */

		}

		g_motion_sta = 1;
		//ST3_EN(ON);//这有一个EN使能引脚函数
		tim_count = TIM_GetCounter(TIM3);
		send_buf[0] = tim_count;
		DMA_Enable(DMA1_Channel6);
		//TIM_SetCompare1(TIM3,tim_count+g_srd.step_delay/2.0f);
		TIM_Cmd(TIM3, ENABLE);  /* 使能TIM3 */
		
}



void TIM3_IRQHandler(void)
{
	
		__IO uint32_t tim_count = 0;
    __IO uint32_t tmp = 0;
    uint16_t new_step_delay = 0;                            /* 保存新（下）一个延时周期 */
    __IO static uint16_t last_accel_delay = 0;              /* 加速过程中最后一次延时（脉冲周期） */
    __IO static uint32_t step_count = 0;                    /* 总移动步数计数器*/
    __IO static int32_t rest = 0;                           /* 记录new_step_delay中的余数，提高下一步计算的精度 */
    __IO static uint8_t i = 0;                              /* 定时器使用翻转模式，需要进入两次中断才输出一个完整脉冲 */
	
    if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
    {
			tim_count = TIM_GetCounter(TIM3);
			tmp = tim_count + g_srd.step_delay/2.0f;               /* 整个C值里边是需要翻转两次的所以需要除以2 */
			send_buf[0] = tmp;
			DMA_Enable(DMA1_Channel6);
			//while(DMA_send_feedback(DMA1_Channel6)){}; //发送完成退出

			//TIM_SetCompare1(TIM3,tmp);
			i++;                                                /* 定时器中断次数计数值 */
        if(i == 2)                                          /* 2次，说明已经输出一个完整脉冲 */
        {
            i = 0;                                          /* 清零定时器中断次数计数值 */
            switch(g_srd.run_state)                         /* 加减速曲线阶段 */
            {
            case STOP:
                step_count = 0;                             /* 清零步数计数器 */
                rest = 0;                                   /* 清零余值 */
                /* 关闭通道*/
								TIM_Cmd(TIM3, DISABLE);  /* 使能TIM3 */
								//ST3_EN(EN_OFF); //电机停
								g_motion_sta = 0;                           /* 电机为停止状态  */
								break;
						
						case ACCEL:
                g_add_pulse_count++;                        /* 只用于记录相对位置转动了多少度 */
                step_count++;                               /* 步数加1*/
                if(g_srd.dir == CW)
                {
                    g_step_position++;                      /* 绝对位置加1  记录绝对位置转动多少度*/
                }
                else
                {
                    g_step_position--;                      /* 绝对位置减1*/
                }
                g_srd.accel_count++;                        /* 加速计数值加1*/
                new_step_delay = g_srd.step_delay - (((2 *g_srd.step_delay) + rest)/(4 * g_srd.accel_count + 1));/* 计算新(下)一步脉冲周期(时间间隔) */
                rest = ((2 * g_srd.step_delay)+rest)%(4 * g_srd.accel_count + 1);                                /* 计算余数，下次计算补上余数，减少误差 */
                if(step_count >= g_srd.decel_start)         /* 检查是否到了需要减速的步数 */
                {
                    g_srd.accel_count = g_srd.decel_val;    /* 加速计数值为减速阶段计数值的初始值 */
                    g_srd.run_state = DECEL;                /* 下个脉冲进入减速阶段 */
                }
                else if(new_step_delay <= g_srd.min_delay)  /* 检查是否到达期望的最大速度 计数值越小速度越快，当你的速度和最大速度相等或更快就进入匀速*/
                {
                    last_accel_delay = new_step_delay;      /* 保存加速过程中最后一次延时（脉冲周期）*/
                    new_step_delay = g_srd.min_delay;       /* 使用min_delay（对应最大速度speed）*/
                    rest = 0;                               /* 清零余值 */
                    g_srd.run_state = RUN;                  /* 设置为匀速运行状态 */
                }
                break;

            case RUN:
                g_add_pulse_count++;
                step_count++;                               /* 步数加1 */
                if(g_srd.dir == CW)
                {
                    g_step_position++;                      /* 绝对位置加1 */
                }
                else
                {
                    g_step_position--;                      /* 绝对位置减1*/
                }
                new_step_delay = g_srd.min_delay;           /* 使用min_delay（对应最大速度speed）*/
                if(step_count >= g_srd.decel_start)         /* 需要开始减速 */
                {
                    g_srd.accel_count = g_srd.decel_val;    /* 减速步数做为加速计数值 */
                    new_step_delay = last_accel_delay;      /* 加阶段最后的延时做为减速阶段的起始延时(脉冲周期) */
                    g_srd.run_state = DECEL;                /* 状态改变为减速 */
                }
                break;

            case DECEL:
                step_count++;                               /* 步数加1 */
                g_add_pulse_count++;
                if(g_srd.dir == CW)
                {
                    g_step_position++;                      /* 绝对位置加1 */
                }
                else
                {
                    g_step_position--;                      /* 绝对位置减1 */
                }
                g_srd.accel_count++;
                new_step_delay = g_srd.step_delay - (((2 * g_srd.step_delay) + rest)/(4 * g_srd.accel_count + 1));  /* 计算新(下)一步脉冲周期(时间间隔) */
                rest = ((2 * g_srd.step_delay)+rest)%(4 * g_srd.accel_count + 1);                                   /* 计算余数，下次计算补上余数，减少误差 */

                /* 检查是否为最后一步 */
                if(g_srd.accel_count >= 0)                  /* 判断减速步数是否从负值加到0是的话 减速完成 */
                {
                    g_srd.run_state = STOP;
                }
                break;
            }
            g_srd.step_delay = new_step_delay;              /* 为下个(新的)延时(脉冲周期)赋值 */
        }
			/*
			++i;
			if(i % 2){
				m.pulsesNum++;	//每进两次中断 脉冲数+1
				i = 0;
			}
			//printf("num = %d", m.pulsesNum);
			//if(m.pulsesNum == )
			*/
    }
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清楚中断标志位
}



