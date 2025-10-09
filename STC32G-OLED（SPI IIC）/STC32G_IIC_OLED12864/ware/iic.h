#ifndef __IIC_H
#define __IIC_H

#include "stc.h"

//------------------------------------------引脚选择------------------------------------------
#define  SCL P25
#define  SDA P24


//------------------------------------------函数声明------------------------------------------
void IIC_DELAY();				                            //等待结束
void IIC_START(void);			                            //IIC开始
void IIC_STOP(void);			                            //IIC结束
void IIC_SENDACK(void);			                            //发送ACK
void IIC_SENDNACK(void);		                            //发送NACK
void IIC_WAITACK(void);			                            //等到从机ACK
void IIC_SENDBYTE(u8 dat);		                            //发送一个字节
u8 IIC_READBYTE(void);			                            //读取一个字节
void IIC_Init();				                            //IIC初始化

void IIC_Write_NByte( u8 slave,u8 addr,u8 *p,u8 number );	//iic写入连续的几个字节
void IIC_Read_NByte( u8 slave,u8 addr,u8 *p,u8 number );	//iic读取连续的几个字节

#endif



