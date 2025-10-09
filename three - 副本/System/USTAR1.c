#include "../User/main.h"


#define BRT (65536 - (MAIN_Fosc / 115200+2) / 4)		// 加 2 操作是为了让 Keil 编译器，自动实现四舍五入运算// 波特率115200																										   																								
bit busy;											// 串口忙标志
unsigned char wptr;										// 写指针
unsigned char rptr;										// 读指针
unsigned char buffer[16];							// 接收缓存，长度16


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
	S1_S0 = 0;									// 设置为默认引脚P3.0 P3.1
	
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
