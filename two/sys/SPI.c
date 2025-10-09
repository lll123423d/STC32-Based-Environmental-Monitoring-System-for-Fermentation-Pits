#include "SPI.h"

void SPI_Init(void)				//初始化
{
	P_SW1 = (P_SW1 & ~(3<<2)) | (1<<2);     //IO口切换. 0: P1.2/P5.4 P1.3 P1.4 P1.5, 1: P2.2 P2.3 P2.4 P2.5, 2: P5.4 P4.0 P4.1 P4.3, 3: P3.5 P3.4 P3.3 P3.2
	SPCTL = 0X90;		//模式配置 ssig==1 忽略SS
	SPCTL |= 0X40;
	SPI_SCK = 0;			//引脚初始化
	SPI_MOSI=1;
	SPSTAT = 0xC0;  // 写1清除SPIF和WCOL标志位
}

void SPI_WriteByte(unsigned char dat)		//写入一个字节
	{
		SPDAT = dat;
		while(SPIF == 0);
		SPSTAT = 0xC0;  // 写1清除SPIF和WCOL标志位
	}

