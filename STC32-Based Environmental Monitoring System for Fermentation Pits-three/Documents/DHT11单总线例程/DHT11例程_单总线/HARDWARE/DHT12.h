#ifndef _DHT12_H_
#define _DHT12_H_
#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "stdio.h"

//IO方向设置
#define SDA_IN()  {GPIOB->CRH&=0XF0FFFFFF;GPIOB->CRH|=0x08000000;}//上下拉输入
#define SDA_OUT() {GPIOB->CRH&=0XF0FFFFFF;GPIOB->CRH|=0x03000000;}//通用推挽输出


//IO操作函数
#define IIC_SCL    PBout(13) //SCL输出	
#define SEND_SDA   PBout(14) //SDA输出	 
#define READ_SDA   PBin(14)  //SDA输入 



void DHT12_Init(void);//初始化SDA(PB14),CLK(PB13)
u8 DHT12_Rdata(void);
u8 DHT12_READ(void);




#endif	//_DHT12_H_
