#ifndef __SPI_H
#define __SPI_H

#include <STC32G.H>

#define SS 		P54
#define MISO	P14
#define MOSI	P13
#define SCLK	P15 



//------------------------------函数声明------------------------------
void SPI_Init(void);						//初始化
void SPI_WriteByte(unsigned char dat);					//写入一个字节
unsigned char SPI_ReadByte(void);			//读取一个字节	


#endif