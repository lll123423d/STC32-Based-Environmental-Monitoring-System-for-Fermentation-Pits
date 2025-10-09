#include "../User/main.h"


void OLED_First_UI(void)
{
	static unsigned int lastReadTime = 0;
    unsigned int currentTime; 
	float temp, humi;
    unsigned char result;
	result = DHT11_Read(&temp, &humi);
	currentTime = GetSystemTime(); // 需要实现获取系统时间的函数
	
	OLED_ShowCHinese(0,2,10);
	OLED_ShowCHinese(16,2,12);
	OLED_ShowChar(32,2,':',16);  //第二页
	
	OLED_ShowCHinese(0,4,10);
	OLED_ShowCHinese(16,4,12);
	OLED_ShowChar(32,4,':',16);  //第三页
	
	OLED_ShowString(0,6,"Co2:");            //第四页
	
	 // 每2秒读取一次DHT11（DHT11需要间隔至少1秒读取）
    if (currentTime - lastReadTime >= 2000) {
        lastReadTime = currentTime;

        if (result == 0) {
            // 清空显示区域
            //OLED_Clear();
            
            // 显示温度标签和值
            OLED_ShowNum_Float(48,2, temp);
            //OLED_ShowString(80, 0, "C");
            
            // 显示湿度标签和值
            OLED_ShowString(0, 2, "Humi:");
            OLED_ShowNum_Float(48, 4, humi);
            OLED_ShowString(80, 2, "%");
        } else {
            // 显示错误信息
            OLED_ShowString(48, 2, "DHT11 Error");
            if (result == 1) {
                OLED_ShowString(48, 4, "No Response");
            } else {
                OLED_ShowString(48, 6, "Checksum Error");
            }
        }
    }
	
	

	
	OLED_ScrollMessage();        //第一页放到最后因为他会清除其他列的显示  显示错位的问题可以通过修正oledchar()来解决
	
	
	

	
}
void main()
{
	WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
	EAXFR = 1; //扩展寄存器(XFR)访问使能
	CKCON = 0; //提高访问XRAM速度
	P0M0 = 0x00; P0M1 = 0x00; 
    P1M0 = 0x00; P1M1 = 0x00; 
    P2M0 = 0x00; P2M1 = 0x00; 
    P3M0 = 0x00; P3M1 = 0x00; 
    P4M0 = 0x00; P4M1 = 0x00; 
    P5M0 = 0x00; P5M1 = 0x00; 
    P6M0 = 0x00; P6M1 = 0x00; 
    P7M0 = 0x00; P7M1 = 0x00; 

	Timer0_Init();
	Uart1Init();
	UartSend(0x00);
	OLED_Init();
	OLED_Clear();
	delay_ms(1000);
	ES = 1;											// 使能串口1中断
	EA = 1;											// 使能EA总中断
	OLED_ShowChar(0,0,'A',16);
	while(1)
	{
//		OLED_First_UI();
		// 其他任务...
//        delay_ms(100); // 适当延时
	}
}



