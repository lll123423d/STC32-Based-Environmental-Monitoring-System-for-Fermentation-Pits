#include <STC32G.H>
#include <intrins.h>
#include <stdio.h>   // 用于printf
sbit DHT11 = P1^1;    //dth11数字信号输入脚

#define BRT (65536 - (11059200 / 115200+2) / 4)		// 加 2 操作是为了让 Keil 编译器，自动实现四舍五入运算// 波特率115200																										   																								
bit busy;											// 串口忙标志
unsigned char wptr;										// 写指针
unsigned char rptr;										// 读指针
unsigned char buffer[16];							// 接收缓存，长度16
void SendServoPositionCmd(unsigned char cmd_id, float angle_x, unsigned int time);

// 微秒级延时函数（基于STC32G12K128的IRC频率调整）
void Delay_us(unsigned int us) {
    while (us--) {
        _nop_(); _nop_(); _nop_(); // 根据主频调整指令周期
    }
}

// 毫秒级延时函数
void Delay_ms(unsigned int ms) {
    while (ms--) {
        Delay_us(1000);
    }
}

// 读取DHT11数据函数
// 返回值：0-成功，1-无响应，2-校验错误
//temp 温度 humi 湿度
unsigned char DHT11_Read(float *temp, float *humi) {
    unsigned char buf[5] = {0};
    unsigned char i, j;
    
    // 主机启动信号
    DHT11 = 0;
    Delay_ms(20);    // 拉低至少18ms
    DHT11 = 1;
    Delay_us(30);    // 拉高等待20-40μs
    
    // 检测从机响应
    if (!DHT11) {
        // 等待83μs低电平响应
        while (!DHT11);
        // 等待87μs高电平
        while (DHT11);
        
        // 接收40位数据
        for (i = 0; i < 5; i++) {
            for (j = 0; j < 8; j++) {
                // 等待54μs低电平结束
                while (!DHT11);
                // 延时40μs（判断高电平持续时间）
                Delay_us(40);
                // 判断位值（高电平持续时间>40μs为逻辑"1"）
                buf[i] <<= 1;     //将整个字节左移一位
                if (DHT11) {
                    buf[i] |= 1;   //如果读到高电平 将buf[i]字节的对应位设置为1//这个操作是对buf[i]的最低位或运算
                    // 等待高电平结束
                    while (DHT11);
                }
            }
        }
        
        // 校验数据
        if (buf[4] == (buf[0] + buf[1] + buf[2] + buf[3])) {
            // 处理温度负数（手册说明：温度<0时，小数部分最高位为1）
            if (buf[3] & 0x80) {
                *temp = -(float)(buf[2] + (buf[3] & 0x7F) * 0.1);
            } else {
                *temp = (float)buf[2] + (float)buf[3] * 0.1;
            }
            *humi = (float)buf[0] + (float)buf[1] * 0.1;
            return 0; // 成功
        } else {
            return 2; // 校验错误
        }
    }
    return 1; // 无响应
}








void Uart1Isr() interrupt 4
{
	if (TI)											// 发送中断标志置1时
	{
		TI = 0;										// 清零发送中断标志
		busy = 0;									// 清零忙状态
	}
	if (RI)											// 接收中断标志置1时
	{
		RI = 0;										// 清零接收中断标志
		buffer[wptr] = SBUF;	// 接收长度16
//		wptr &= 0x0f;						// 接收长度16
		wptr = (wptr + 1) % 16;            // 存入环形缓冲区
		
//		buffer[0] = SBUF;					// 接收长度1
	}
}
void Uart1Init()							// UART1初始化
{
	S1_S1 = 0;	
	S1_S0 = 0;									// 切换串口1引脚到P3.6(RxD) P3.7(TxD)
	
	SCON = 0x50;								// 模式1（8位数据）、接收使能
	T2L = BRT;						
	T2H = BRT >> 8;							// 波特率对应的重装载值
	S1BRT = 1;									// 定时器2做波特率发生器
	T2x12 = 1;									// 1T模式
	T2R = 1;										// 启动定时器2
	wptr = 0x00;								// 清零写指针
	rptr = 0x00;								// 清零读指针
	busy = 0;										// 清零忙标志
}
void UartSend(unsigned char dat)				// 发送字符函数
{
	while (busy);								// 当串口忙时，等待
	busy = 1;										// 忙标志置1
	SBUF = dat;									// 发送数据dat
}


void dhtll_print() {
    unsigned char result;
    float temperature, humidity;
    char temp_str[10], humi_str[10];
    int i;
    result = DHT11_Read(&temperature, &humidity);
    
    if(result == 0) {
        // 发送温度
        sprintf(temp_str, "%.1f", temperature);
        UartSend('T');
        UartSend('e');
        UartSend('m');
        UartSend('p');
        UartSend(':');
        for( i = 0; temp_str[i] != '\0'; i++) {
            UartSend(temp_str[i]);
        }
        UartSend(' ');
        UartSend('C');
        UartSend(',');
        UartSend(' ');
        
        // 发送湿度
        sprintf(humi_str, "%.1f", humidity);
        UartSend('H');
        UartSend('u');
        UartSend('m');
        UartSend('i');
        UartSend(':');
        for( i = 0; humi_str[i] != '\0'; i++) {
            UartSend(humi_str[i]);
        }
        UartSend(' ');
        UartSend('%');
        UartSend('\r');
        UartSend('\n');
    } else if (result == 1) {
        // 发送错误信息
        const char *err = "DHT11 No Response!\r\n";
        for( i = 0; err[i] != '\0'; i++) {
            UartSend(err[i]);
        }
    } else {
        const char *err = "DHT11 Checksum Error!\r\n";
        for( i = 0; err[i] != '\0'; i++) {
            UartSend(err[i]);
        }
    }
}
void main()
{
			WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
			EAXFR = 1; //扩展寄存器(XFR)访问使能
			CKCON = 0; //提高访问XRAM速度
	        P1M0 = 0x00; P1M1 = 0x00;   //设置为双向口
//			P1PU &= ~0x80;    			//设置4.1k上拉电阻，硬件外接4.7k时注释
//			P1IE |= 0x80; 				//使能数字信号输入
			
			Uart1Init();
			ES = 1;											// 使能串口1中断
			EA = 1;											// 使能EA总中断
// 上电等待传感器稳定
    Delay_ms(1000);
    
    while(1) {
        dhtll_print();
        Delay_ms(2000);  // 遵守2秒读取间隔
    }

}