#include "SPI.h"

/*--- 全局变量和状态标志，用于主循环与中断服务程序(ISR)通信 ---*/
// volatile 关键字确保编译器不对这些变量进行优化，因为它们在中断中被修改
sbit LED = P1^1; 

void SPI_Init(void)				//初始化
	{
		LED = 1; 
		SS = 1; 
		P_SW1 &= 0XFC;		//???? ??00		引脚选择将低两位写为0
							//SS MOSI MISO SCLK  P5.4 P1.3 P1.4 P1.5 
		
		SPCTL = 0X50;		//模式配置 SSIG==0,MSTR==0固定为主机模式
		SPCTL |= 0X40;      //最后再使能SPI防止数据未使能读写就已经结束
		
		SCLK = 0;			//引脚初始化
		
		SPSTAT = 0xC0;  // 写1清除SPIF和WCOL标志位
		ESPI = 1;       // 使能SPI中断 (STC32G中SPI中断的使能位)
		//EA = 1;         // 开启全局中断 总中断会再主函数打开
	}
//	
//void SPI_Isr() interrupt 9  
//{ 
// busy = 0; 
// SS = 1;  //拉高从机的SS管脚 
// SPSTAT = 0xC0; //清中断标志
// LED = !LED; //测试端口 
//} 

void SPI_WriteByte(unsigned char dat)		//写入一个字节
	{
	SS = 0; //拉低从机SS管脚 
	SPDAT = dat; //发送测试数据 
	while (!(SPSTAT & 0x80)); //查询完成标志 
	SPSTAT = 0xc0; //清中断标志 
	SS = 1; //拉高从机的SS管脚 
	LED = !LED; //测试端口
	}
	
/**
  * @brief  通过SPI读取一个字节的数据
  * @param  无
  * @retval 读取到的字节数据
  * @note   此函数使用查询方式，会阻塞直到数据传输完成
  **/
unsigned char SPI_ReadByte(void)
{
   unsigned char received_data;
    
    SS = 0;              // 拉低从机SS管脚，开始传输
    SPDAT = 0xFF;        // 发送虚拟数据0xFF以产生时钟信号
    while (!(SPSTAT & 0x80)); // 等待传输完成
    received_data = SPDAT; // 读取接收到的数据
    SPSTAT = 0xC0;       // 清除SPIF和WCOL标志位
    SS = 1;              // 拉高从机的SS管脚，结束传输
    
    return received_data; // 返回读取到的数据
}

