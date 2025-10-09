#include "../User/main.h"
sbit DHT11 = P1^1;    //dth11数字信号输入脚

// 读取DHT11数据函数
// 返回值：0-成功，1-无响应，2-校验错误
//temp 温度 humi 湿度
	unsigned char DHT11_Read(float *temp, float *humi)
{
    unsigned char buf[5] = {0};
    unsigned char i, j;
    
    // 主机启动信号
    DHT11 = 0;
    delay_ms(20);    // 拉低至少18ms
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












