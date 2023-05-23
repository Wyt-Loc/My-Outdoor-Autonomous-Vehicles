#define GPSSerial  Serial
#define DEBUGSerial Serial


void setup()	//初始化内容
{
  GPSSerial.begin(9600);			//定义波特率9600
  DEBUGSerial.begin(9600);  
}

void loop()		//主循环
{
  while (GPSSerial.available()) {   
     DEBUGSerial.write(GPSSerial.read());//收到GPS数据则通过Serial输出
  }
  

}
