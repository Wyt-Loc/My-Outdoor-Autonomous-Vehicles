/* 	CANBUS控制器所有指令从这发出			*/
 
 #include <math.h>
 #include <stdio.h>
 #include <string.h>
 #include "timer.h"
 #include "canControl.h"
 #include "can.h" 


/***************************************************************
【函数名】 char motorValueVerify(u8 dir, float distance, float speed)
【功  能】 校验参数是否正确  参数是否存在负数，或其他
【参数值】 是否正确 0 < speed < 10   0 < distance < 50
【返回值】 对返回 1，否则 返回 0
****************************************************************/
char motorValueVerify(u8 dir, float distance, float speed){
	
	u8 flag = 0;
	
    if (dir > 255) {
      printf("Error: Invalid dir parameter! \n");
			flag = 1;
    }
    if (isnan(distance) || !isfinite(distance) || distance < 0 || distance > 255) {
      printf("Error: Invalid distance parameter! \n");
			flag = 1;
    }
    if (isnan(speed) || !isfinite(speed) || speed < 0) {
			printf("Error: Invalid speed parameter! \n");
			flag = 1;  
    }
		if (speed > 10) {
			printf("The speed parameter exceeds the maximum value! \n");
			flag = 1;
		}
		if (distance > 50.0f){
			printf("Error: The distance parameter exceeds the maximum value \n");
			flag = 1;
		}
		if(flag == 0) return 1; else return 0;

}


/***************************************************************
【函数名】 u8 *motorCommandGenerate(u8 dir, float distance, float speed)
【功  能】 电机控制命令生成
【参数值】 dir：方向；  distance：运动距离； speed：速度；
【返回值】 命令数组, 这个使用方法，对bit位有浪费。
****************************************************************/
u8* motorCommandGenerate(u8 dir, float distance, float speed){
	// 格式为 方向buf[1]  距离和速度 buf[2] buf[3] buf[4].... 整数和小数后2为分别对应
	static u8 buf[8] = {0};
		if(dir == 1)
			buf[0] = 1;
		else if (dir == 0)
			buf[0] = 0;
		else if(dir > 1)
		{
			buf[0] = dir;
		}

		//get disance
		buf[1] = distance; 
		buf[2] = (int)((distance - buf[1]) * 10);
		buf[3] = (distance - buf[1] - buf[2] * 0.1) * 100;
		
		buf[4] = speed;
		buf[5] = (int)((speed - buf[4]) * 10);
		buf[6] = (speed - buf[4] - buf[5] * 0.1) * 100;
	
		return buf;
}

// 是否接收到，接收到了，接收方传回一个应答，如果在0.1秒内没有应答信号，那么再次发送，累计次以上，
// 直接报错，开始记录error类型log，且打入时间信息。
/*
   500KB 的can 大概每秒500000bits ， 那么1秒也就是 3900 帧， 这里直接采用一般那么也有进2000帧，
		1ms 能够有3帧， 那么我们采取0.01秒的校验时间，也能够证系统足够的稳定。
*/
/***************************************************************
【函数名】 u8 isReceived(void)
【功  能】 判断挂载节点是否接收到了数据帧
【参数值】 无
【返回值】 是否
****************************************************************/
u8 isReceived(u8* buff){
	
	u8 receBuf[8];
	u8 cnt = 0;
	u8 sum = 0;
	u8 i = 0;
	
	cnt = Can_Receive_Msg(receBuf); // 控制器接收到后返回所有字节的高三位之和。 最大也就是9 * 8 = 72  暂时没想到好的校验算法
	if(cnt) { //接收到了数据
		for(i = 0; i < cnt / 2; i++){
			sum += buff[i] & 0xE0;
		}
		 if( sum == receBuf[0]){
			 	printf("The returned data was successfully received!! \r\n");
			 return 1;  //成功
		 }
	}

	return 0; //失败

}


/***************************************************************
【函数名】 u8 sendMotorCommand(u8 dir, float distance, float speed)
【功  能】 发送控制信息，控制电机运动
【参数值】 dir：方向；  distance：运动距离； speed：速度； id：设备id号1.2.3.4.....
【返回值】 是否发送成功，耗时多少,    如果发送失败，就一直尝试重发，直到成功为止。
					 1.有一个弊端，可能一个非常不重要的数据发送失败，导致系统直接瘫痪。
					 但是一直接收不到，也可能是线路真的出问题了，所以这种情况不好去解决。
					 2.移植为HAL后有个bug, 将线断开后会不断尝试重新发送，但是不会printf打印。 暂时这样， 
****************************************************************/
#define debug 0 // 是否打印发送的信息
u8 sendMotorCommand(u8 dir, float distance, float speed, uint32_t id){
	u8 i = 0;
	u8 errorFlag = 0;
	u8 errorCnt = 0;
	u8 issucess = 0;
	u8 timecnt = 0;
	u8 *commandBuf = motorCommandGenerate(dir,distance,speed);
	
//	printf("Can\r\n");
	//printf("my\r\n");
	_q1:
	issucess = Can_Send_Msg(commandBuf,8,id);//发送8个字节
	  //goto从这块重新执行
	TIM3->CNT = 0;
	startTimerInterrupt();		//发送数据成功开始定时器计数
	// 这块很占时间
	#if debug
	printf("This time the data is：");
	for(i = 0;  i < 8; i++){
		 printf(" %d",commandBuf[i]);
	}
	printf("\r\n");
	#endif
	
	if (!issucess){
		printf("The control command was sent successfully ! \r\n"); 	//最后把所有的printf函数封装位以太网打Log
		while(1){
			// 收到了验证数据
			if( isReceived(commandBuf) )  // 如果接收到了返回的数据，且通过加密验证
			{
				stopTimerInterrupt(); //停止定时器计数
				timecnt = getTimerCountValue(TIM3);   // 得到时间计时，留作函数返回用
				printf("timecnt is %d \r\n", timecnt);
				if (timecnt < 200){
					printf("This time, the data is sent \r\n");
					errorFlag = 0; // 发送成功了，那么解开这个限制。
					return timecnt;  //直接返回本次的事件
				}
			}
			// 失败了， 没收到验证数据
			else
			{
				// 还没到出错的等级
					stopTimerInterrupt(); // 先关闭定时器
					errorCnt++; // 错误计数
					errorFlag = 0;
					if(errorCnt < ERRORRANGE){  //错误计数
						printf("The time is greater than 5ms or the data received is incorrect %dth time\r\n", errorCnt); //一直打log之后在主机那边写一个脚本检测log日志是否正常。
						goto _q1;   //使用goto简化代码， 调试时注意这块是跳到了重新进行检测这次的事件，也就是跳到了开始计时的上一行代码。
					}
					else{
						errorFlag = 1;
					}
				// 尝试了10次还是失败了，
					// 仍然不断尝试重新发送，但是log也在一直打印
					while(errorFlag == 1){
						if(errorCnt > 50){
							// 重启设备 错误次数太多 尝试重启解决问题
							HAL_NVIC_SystemReset();          	/* 重启 */
						}
						//在这一直重复这个过程， 但是log等级变成了 ERROR 等级
						printf("ERROR Restart the device %d times........\r\n", 50 - errorCnt); //一直打log之后在主机那边写一个脚本检测log日志是否正常。
						goto _q1;
					}
				break;
			}
		}
	}
	// 发送失败 尝试重发
	else
	{
		printf("The control command was sent fail ! \r\n"); //发送是否成功
		printf("Retry sending  \r\n");
		goto _q1;
	}
	return timecnt;
}


/***************************************************************
【函数名】void test()
【功  能】功能单元测试 最低标准要求
【参数值】无
【返回值】无
****************************************************************/
void test(void){

	u8 dir = 1;
	float dis = 3.125f;
	float speed = 4.043f;

	if (motorValueVerify(dir, dis, speed)) { //参数值校验

		sendMotorCommand(dir, dis, speed, 0);

	}
}
