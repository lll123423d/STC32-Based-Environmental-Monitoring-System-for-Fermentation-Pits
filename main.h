#ifndef __MAIN_H__
#define __MAIN_H__

#include <STC32G.H>
#include <intrins.h>
#include <stdio.h>   // 用于printf
#include "../System/system.h"
#include "../System/USTAR1.h"
#include "../System/SPI.h"
#include "../Hardware/oled.h"
#include "../Hardware/fontoled.h"
#include "../Hardware/dth11.h"



#define MAIN_Fosc 11059200UL		//定义主频11.0592MHz，请根据实际使用频率修改


typedef bit BOOL;
typedef unsigned char BYTE;
typedef unsigned int WORD;
typedef unsigned long DWORD;

typedef unsigned char u8;				//unsigned char			 	类型，用u8表示
typedef unsigned int u16;				//unsigned int 				类型，用u16表示
typedef unsigned long int u32;	//unsigned long int 	类型，用u32表示

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned int ushort;
typedef unsigned long ulong;

typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long uint32_t;

#endif
