#include "../User/main.h"

/*------------------------------------------引脚选择----------------------*/
    //注意：这里SPI的引脚选择要通过P_SW1寄存器切换，在修改这里的引脚
sbit    SPI_MISO = P2^4;    //PIN2
sbit    SPI_MOSI = P2^3;    //PIN5
sbit    SPI_SCK = P2^5;     //PIN6

void SPI_Init(void)				//初始化
{
	P_SW1 = (P_SW1 & ~(3<<2)) | (1<<2);     //IO口切换
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