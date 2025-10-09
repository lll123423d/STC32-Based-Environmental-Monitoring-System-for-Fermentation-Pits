#ifndef __OLED_H
#define __OLED_H

//------------------------------------------º¯ÊýÉùÃ÷------------------------------------------
void OLED_WR_Byte(unsigned char dat,unsigned char cmd);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_Clear(void);
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char  chr,unsigned char SIZE);
void OLED_Init(void);
void OLED_ShowString(unsigned char x,unsigned char y,unsigned  char  *chr);
void OLED_ShowNum_Float(unsigned char x, unsigned char y, float num);
void OLED_ShowCHinese(unsigned char x,unsigned char y,unsigned char no);
void OLED_ScrollMessage(void);
    
#endif



