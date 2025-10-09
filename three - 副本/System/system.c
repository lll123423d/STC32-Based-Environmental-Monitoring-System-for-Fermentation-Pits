#include "../User/main.h"

volatile unsigned int timer_ms = 0;       // 毫秒计时器

/***********************
 * 定时器0初始化
 * 配置1ms定时中断
 ***********************/
void Timer0_Init(void) {
	AUXR &= 0x7F;			//定时器时钟12T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0x18;				//设置定时初始值
	TH0 = 0xFC;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
	ET0 = 1;				//使能定时器0中断
}

/***********************
 * 定时器0中断服务函数
 * 每1ms更新毫秒计数器
 ***********************/
void Timer0_ISR() interrupt 1 {
    TH0 = 0xFC;                 // 重装高8位
    TL0 = 0x18;                 // 重装低8位
    timer_ms++;                 // 更新毫秒计数器
}

/***********************
 * 毫秒延时函数
 * 基于定时器0的毫秒计数器
 ***********************/
void delay_ms(unsigned int ms) {
unsigned int start = timer_ms; //start 是 局部变量，仅在 delayus() 函数的作用域内有效，且在初始化后 不再被修改
    while (timer_ms- start < ms); // 等待指定毫秒数

}

// 微秒级延时函数（基于STC32G12K128的IRC频率调整）
void Delay_us(unsigned int us) {
    while (us--) {
        _nop_(); _nop_(); _nop_(); // 根据主频调整指令周期
    }
}

// 获取系统时间函数
unsigned long GetSystemTime(void)
{
    unsigned long time;
    
    // 禁止中断以确保读取时间的原子性
    EA = 0;
    time = timer_ms;
    EA = 1;
    
    return time;
}