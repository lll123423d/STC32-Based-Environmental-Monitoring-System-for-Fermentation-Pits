#include <SYS.H>
#include "SPI.h"
#include "OLED.h"
#include "pic.h"
#define  DELAY_TIME   2000
void main()
{
	unsigned char i;
    float dat= -3.145;
    sysini();
	Timer0_Init();

	OLED_Init();			//初始化OLED 
	EA=1;
	
 while (1)
    {
//-----------------------------------------显示图片-----------------------------------------   
        OLED_BuffClear();                                                       //清除全部缓存
        OLED_BuffShowBMP(0,0,128,8,OLED12864_IMG);                              //显示LOGO图片
		OLED_BuffShow();                                                        //将缓存写入显示屏显示
        delay_ms(DELAY_TIME);                                                   //延时	

//-----------------------------------------显示字符-----------------------------------------   
		OLED_BuffClear();                                                       //清除全部缓存
		OLED_BuffShowString(0,0,"HEELO ,WELCOME TO USE STC32&OLED12864!",0);      //显示数据
		OLED_BuffShow();                                                        //将缓存写入显示屏显示
        delay_ms(DELAY_TIME);                                                   //延时	
        
//-----------------------------------------显示汉字-----------------------------------------   
		OLED_BuffClear();                                                       //清除全部缓存
		OLED_BuffShowString(0,0,"你好,欢迎使用STC 驱\xfd动OLED12864的历程",0);      //显示数据
		OLED_BuffShow();                                                        //将缓存写入显示屏显示
        delay_ms(DELAY_TIME);                                                   //延时	       
        
//-----------------------------------------显示点-----------------------------------------   
		OLED_BuffClear();                                                       //清除全部缓存
		OLED_BuffShowString(32,0,"画点测试");                                    //显示数据
		OLED_BuffShow();                                                        //将缓存写入显示屏显示	
        for( i=0;i<64;i++ )                                                     //循环64次
        {
            OLED_BuffShowPoint(i*2,25);                                         //显示一个点
            OLED_BuffShow();                                                    //刷新显示
            delay_ms(50);                                                       //延时
        }
        
//-----------------------------------------显示线-----------------------------------------   
		OLED_BuffClear();                                                       //清除全部缓存
		OLED_BuffShowString(32,0,"画线测试");                                    //显示数据
		OLED_BuffShow();                                                        //将缓存写入显示屏显示	
        for( i=0;i<6;i++ )                                                      //循环6次
        {
            OLED_BuffShowLine(4,16+8*i,124,16+8*i);                             //显示横着6条线
            OLED_BuffShow();                                                    //刷新显示
            delay_ms(100);
        }        
        for( i=0;i<16;i+=1 )                                                    //循环16次
        {
            OLED_BuffShowLine(8*i+4,16,8*i+4,56);                               //显示16条数线
            OLED_BuffShow();                                                    //刷新显示
            delay_ms(100);
        }  

//-----------------------------------------显示圆-----------------------------------------   
		OLED_BuffClear();                                                       //清除全部缓存
		OLED_BuffShowString(32,0,"画圆测试");                                    //显示数据
		OLED_BuffShow();                                                        //将缓存写入显示屏显示	
        for( i=0;i<6;i++ )                                                      //循环6次
        {
           OLED_BuffShowCircle(64,40,5+i*3);                                    //画一个圆    
            OLED_BuffShow();                                                    //刷新显示
            delay_ms(250);
        }         

//-----------------------------------------显示32大字-----------------------------------------   
		OLED_BuffClear();                                                       //清除全部缓存
		OLED_BuffShowString(32,0,"大字测试");                                    //显示数据
        OLED_BuffShowChar32(24,3,1,0);                                          //显示数字1
        OLED_BuffShowChar32(40,3,0,0);                                          //显示数字0
        OLED_BuffShowChar32(56,3,10,0);                                         //显示：
        OLED_BuffShowChar32(72,3,5,0);                                          //显示数字5
        OLED_BuffShowChar32(88,3,5,0);                                          //显示数字5
		OLED_BuffShow();                                                        //将缓存写入显示屏显示	
        delay_ms(DELAY_TIME);                                                   //延时	 

//-----------------------------------------反显测试-----------------------------------------   
		OLED_BuffClear();                                                       //清除全部缓存
		OLED_BuffShowString(32,0,"反显测试",0);                                  //显示数据
        OLED_BuffShowString(16,2,"  这是正\xfd显  ",0);                          //显示数据
        OLED_BuffShowString(16,4,"  这是反显  ",1);                              //显示数据
		OLED_BuffShow();                                                        //将缓存写入显示屏显示
        delay_ms(DELAY_TIME); 

//-----------------------------------------变量显示-----------------------------------------   
        OLED_BuffClear();                                                       //清除全部缓存
        OLED_BuffShowString(32,0,"变量显示",0);                                  //显示数据
        OLED_BuffShowNum(0,2,-25169,0) ;                                        //显示数据
        OLED_BuffShowNum02F(0,4,3.14159,0);                                     //显示数据
        OLED_BuffShowNum02F(0,6,dat,1);                                         //显示数据
        OLED_BuffShow();                                                        //将缓存写入显示屏显示
        delay_ms(DELAY_TIME);   
                
    }
}
