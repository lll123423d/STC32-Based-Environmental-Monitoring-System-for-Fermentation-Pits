#include <STC32G.H>
#include <CONFIGOLED.H>
#include <intrins.h>
#include <stdio.h>   // 用于printf

sbit DHT11 = P1^1;    //dth11数字信号输入脚

sbit    SPI_MISO = P2^4;    //PIN2
sbit    SPI_MOSI = P2^3;    //PIN5
sbit    SPI_SCK = P2^5;     //PIN6

sbit    OLED_CS     = P2^2;                                 //???? 
sbit    OLED_DC     = P2^1;                                 //????/??????
sbit    OLED_REST   = P2^0;                                 //??λ??

#define OLED_CMD  0	                                        //д????
#define OLED_DATA 1	                                        //д????
#ifndef NULL
#define NULL ((void*)0)
#endif

extern unsigned char xdata OLED_Buffer[8][128]; // 显示缓冲区
unsigned char abc[]="hell world";

// 假设OLED宽度为128像素，每个汉字占16像素，英文字符占8像素
#define OLED_WIDTH 128

// 字符串分解后的元素结构
typedef struct {
    unsigned char type; // 0:英文, 1:汉字
    unsigned char value; // 英文字符的ASCII或汉字的字库索引
} CharUnit;

// 将字符串转换为CharUnit数组
// 字符串："基于STC32G12K128的窖池环境监测系统"
// 汉字对应的字库索引：基(0), 于(1), 的(14), 窖(2), 池(3), 环(4), 境(5), 监(6), 测(7), 系(8), 统(9)
// 英文和数字直接使用ASCII码
const CharUnit message[] = {
    {1, 0},  // 基
    {1, 1},  // 于
    {0, 'S'}, {0, 'T'}, {0, 'C'}, {0, '3'}, {0, '2'}, {0, 'G'}, {0, '1'}, {0, '2'}, {0, 'K'}, {0, '1'}, {0, '2'}, {0, '8'},
    {1, 14}, // 的
    {1, 2},  // 窖
    {1, 3},  // 池
    {1, 4},  // 环
    {1, 5},  // 境
    {1, 6},  // 监
    {1, 7},  // 测
    {1, 8},  // 系
    {1, 9}   // 统
};

#define MESSAGE_LENGTH (sizeof(message) / sizeof(message[0]))

// 滚动显示状态变量
int scroll_x = OLED_WIDTH; // 起始x坐标（从屏幕右侧开始）
int total_width = 0;       // 消息总宽度
int scroll_delay = 0;      // 滚动延迟计数器

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

// 微秒级延时函数（基于STC32G12K128的IRC频率调整）
void Delay_us(unsigned int us) {
    while (us--) {
        _nop_(); _nop_(); _nop_(); // 根据主频调整指令周期
    }
}

// 毫秒级延时函数
void Delay_ms(unsigned int ms) {
    while (ms--) {
        Delay_us(1000);
    }
}

void OLED_WR_Byte(unsigned char dat,unsigned char cmd)
{
    if(cmd)
    {
        OLED_DC = 1;
    }
    else
    {
        OLED_DC = 0;
    }
    OLED_CS = 0;
    SPI_WriteByte(dat);
    OLED_CS = 1;
}

void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WR_Byte((unsigned char)(0xb0+y),OLED_CMD);    //设置其实页的位置（0到7页）
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);     //设置列（低四位）
	OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD);          //高四位
}

//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	unsigned char i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(0x00,OLED_DATA); 
	} //更新显示
}

/*@显示字符函数
 *@x--横坐标  选择F8X16[]，x占用8个宽度 
 *@y--纵坐标  选择F8X16[]，y占用2个宽度（两行）
 *@char--要显示的字库里的字符
 *@SIZE--选择configoled.h里的字库，16表示选择F8X16[]
 */
void OLED_ShowChar(unsigned char x,unsigned char y,unsigned char  chr,unsigned char SIZE)
{      	
	unsigned char c=0,i=0;	
		c=chr-' ';//根据空格字符得到偏移后的值 chr-32			
		if(x>128-1){x=0;y=y;}   //如果起点横坐标超过128（屏幕最右侧），则行清零列下移动两行
		if(SIZE ==16)             //根据字号大小选择字库
			{
			OLED_Set_Pos(x,y);	
			for(i=0;i<8;i++)       //两行高度的上一行
			OLED_WR_Byte( F8X16[c*16+i],OLED_DATA);
			OLED_Set_Pos(x,y+1);
			for(i=0;i<8;i++)       //下一行
			OLED_WR_Byte( F8X16[c*16+i+8],OLED_DATA);
			}
			else {	
				OLED_Set_Pos(x,y+1);
				for(i=0;i<6;i++)
				OLED_WR_Byte( F6x8[c][i],OLED_DATA);
				
			}
}

void OLED_Init(void)
{
	SPI_Init();
    Delay_ms(200);          //延时等待单片机稳定

    OLED_WR_Byte(0xAE,OLED_CMD);//关闭显示，准备配置
	//开始配置
	OLED_WR_Byte(0x20,OLED_CMD);//设置内存地址模式
    OLED_WR_Byte(0x02,OLED_CMD);//00 水平   01 垂直  02 页地址模式
	
    OLED_WR_Byte(0x00,OLED_CMD);//设置列起始地位
    OLED_WR_Byte(0x10,OLED_CMD);//设置列起始高位
	
    OLED_WR_Byte(0x40,OLED_CMD);//设置起始行：显存中的第几行作为屏幕的第一行
	OLED_WR_Byte(0xb0,OLED_CMD);//设置起始页：0到7页
	
    OLED_WR_Byte(0x81,OLED_CMD);//设置对比度
    OLED_WR_Byte(0xFf,OLED_CMD);// 0到256个级别  00h-ffh
	
	OLED_WR_Byte(0xa4,OLED_CMD);// 0xa4显示显存内容 0xa5无视显纯内容点亮全屏
	
    OLED_WR_Byte(0xA1,OLED_CMD);//设置镜像    0xa0左右反置 0xa1正常
    OLED_WR_Byte(0xC8,OLED_CMD);//设置镜像   0xc0上下反置 0xc8正常
	
    OLED_WR_Byte(0xA6,OLED_CMD);//设置为普通显示
	
    OLED_WR_Byte(0xA8,OLED_CMD);//设置复用率（0-63）
    OLED_WR_Byte(0x3f,OLED_CMD);//1/64 （0x00-0x3f）
	
    OLED_WR_Byte(0xD3,OLED_CMD);//设置屏幕上下偏移
    OLED_WR_Byte(0x00,OLED_CMD);//0x00-0xff 不偏移
	
    OLED_WR_Byte(0xd5,OLED_CMD);//设置时钟显示分频频率 刷新率
    OLED_WR_Byte(0x80,OLED_CMD);//0x10-0xf0(越大速度越快)
	
    OLED_WR_Byte(0xD9,OLED_CMD);//设置预充电时间
    OLED_WR_Byte(0xF1,OLED_CMD);//将预充电设置为15个时钟周期，放电设置为1个时钟周期
	
    OLED_WR_Byte(0xDA,OLED_CMD);//设置列引脚硬件配置
    OLED_WR_Byte(0x12,OLED_CMD);//12864--12 12832--02
	
    OLED_WR_Byte(0xDB,OLED_CMD);//--设置vcomh反压值
    OLED_WR_Byte(0x40,OLED_CMD);//设置 VCOM 取消选择电平
	
    OLED_WR_Byte(0x8D,OLED_CMD);//设置电荷泵开启
    OLED_WR_Byte(0x14,OLED_CMD);//禁用
	
    OLED_WR_Byte(0xAF,OLED_CMD);  //开启显示
    OLED_Clear();
	OLED_Set_Pos(0,0); 
}

//显示一个字符号串
void OLED_ShowString(unsigned char x,unsigned char y,unsigned  char  *chr)
{ 
	unsigned char j=0;
	while (chr[j]!='\0')
	{		OLED_ShowChar(x,y,chr[j],16);
			x+=8;
		if(x>120){x=0;y+=2;}
			j++;
	}
}

//m的n次方函数，这里用于计算10的n次方
unsigned int oled_pow(unsigned char m,unsigned char n)
{
	unsigned int result=1;	 
	while(n--)result*=m;    
	return result;
}				  
/*显示2个数字
 *x,y :起点坐标	 
 *len :数字的位数   最多5位
 *size:字体大小
 *mode:模式	0,填充模式;1,叠加模式
 *num:数值(0~65535);	       */ 		  
void OLED_ShowNum(unsigned char x,unsigned char y,unsigned int num,unsigned char len,unsigned char size2)
{         	
	unsigned char t,temp;
	unsigned char enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ',size2);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2); 
	}
}

//========================================================================
// 函数名称: OLED_ShowNum_Float
// 函数功能: OLED显示到小数点后两位
// 入口参数: @x：x点  @y：y点  @num：小数变量   
// 函数返回: 无
//========================================================================
// 手动实现浮点数到字符串转换（两位小数）
void OLED_ShowNum_Float(unsigned char x, unsigned char y, float num) {
    unsigned char str[12];
    unsigned char i = 0, j = 0;
    long integer_part;
    int fractional_part;
    unsigned char neg = 0;
    
    // 处理负数
    if (num < 0) {
        neg = 1;
        num = -num;
    }
    
    // 分离整数和小数部分
    integer_part = (long)num;
    fractional_part = (int)((num - integer_part) * 100 + 0.5); // 四舍五入到两位小数
    
    // 处理小数部分进位
    if (fractional_part >= 100) {
        fractional_part -= 100;
        integer_part++;
    }
    
    // 转换整数部分
    if (integer_part == 0) {
        str[i++] = '0';
    } else {
        long temp = integer_part;
        unsigned char digits = 0;
        
        // 计算位数
        while (temp > 0) {
            temp /= 10;
            digits++;
        }
        
        // 转换整数部分
        temp = integer_part;
        for (j = 0; j < digits; j++) {
            str[i + digits - 1 - j] = (temp % 10) + '0';
            temp /= 10;
        }
        i += digits;
    }
    
    // 添加小数点
    str[i++] = '.';
    
    // 添加小数部分（确保两位）
    if (fractional_part < 10) {
        str[i++] = '0';
        str[i++] = fractional_part + '0';
    } else {
        str[i++] = (fractional_part / 10) + '0';
        str[i++] = (fractional_part % 10) + '0';
    }
    
    // 添加负号（如果需要）
    if (neg) {
        // 将整个字符串向右移动一位，给负号腾出位置
        for (j = i; j > 0; j--) {
            str[j] = str[j - 1];
        }
        str[0] = '-';
        i++;
    }
    
    str[i] = '\0'; // 字符串结束符
    
    OLED_ShowString(x, y, str);
}

/*@x 列号 128****
 *@y 页号 8
 *@no 汉字库索引
 *注意汉字是16x16的字库
 ***************/
void OLED_ShowCHinese(unsigned char x,unsigned char y,unsigned char no)
{      			    
	unsigned char t,adder=0;
	OLED_Set_Pos(x,y);	
    for(t=0;t<16;t++)
		{
				OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
				adder+=1;
     }	
		OLED_Set_Pos(x,y+1);	
    for(t=0;t<16;t++)
			{	
				OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
				adder+=1;
      }					
}

// 清除单行内容的函数
void OLED_ClearLine(unsigned char line) {
    unsigned char x;
    OLED_Set_Pos(0, line);
    for(x = 0; x < OLED_WIDTH; x++) {
        OLED_WR_Byte(0x00, OLED_DATA);
    }
}

// 滚动显示步骤函数（非阻塞）
void OLED_ScrollStep(void) {
    int i, char_width, x;
    
    // 计算整个字符串的总宽度（仅第一次）
    if(total_width == 0) {
        for(i = 0; i < MESSAGE_LENGTH; i++) {
            if(message[i].type == 1) {
                total_width += 16; // 汉字宽度
            } else {
                total_width += 8;  // 英文字符宽度
            }
        }
    }
    
    // 只清除滚动区域（第一页和第二页）
    OLED_ClearLine(0);
    OLED_ClearLine(1);
    
    x = scroll_x;
    for(i = 0; i < MESSAGE_LENGTH; i++) {
        char_width = (message[i].type == 1) ? 16 : 8;
        
        // 只绘制在屏幕可见区域内的字符
        if(x < OLED_WIDTH && x + char_width > 0) {
            if(message[i].type == 1) {
                OLED_ShowCHinese((unsigned char)x, 0, message[i].value);
            } else {
                OLED_ShowChar((unsigned char)x, 0, message[i].value, 16);
            }
        }
        
        x += char_width;
        
        // 如果已经超出屏幕右侧，提前结束循环
        if(x >= OLED_WIDTH) {
            break;
        }
    }
    
    // 更新起始位置
    scroll_x--;
    
    // 如果整个字符串已经完全滚出屏幕，则重置起始位置
    if(scroll_x < -total_width) {
        scroll_x = OLED_WIDTH;
    }
}

// 读取DHT11数据函数
// 返回值：0-成功，1-无响应，2-校验错误
//temp 温度 humi 湿度
unsigned char DHT11_Read(float *temp, float *humi) {
    unsigned char buf[5] = {0};
    unsigned char i, j;
    unsigned int timeout;
    
    // 主机启动信号
    DHT11 = 0;
    Delay_ms(20);    // 拉低至少18ms
    DHT11 = 1;
    Delay_us(30);    // 拉高等待20-40μs
    
    // 检测从机响应
    timeout = 10000;
    while (!DHT11 && timeout--); // 等待低电平响应
    if (timeout == 0) return 1; // 无响应
    
    timeout = 10000;
    while (DHT11 && timeout--); // 等待高电平
    if (timeout == 0) return 1;
    
    // 接收40位数据
    for (i = 0; i < 5; i++) {
        for (j = 0; j < 8; j++) {
            // 等待54μs低电平结束
            timeout = 10000;
            while (!DHT11 && timeout--);
            if (timeout == 0) return 1;
            
            // 延时40μs（判断高电平持续时间）
            Delay_us(40);
            
            // 判断位值（高电平持续时间>40μs为逻辑"1"）
            buf[i] <<= 1;     //将整个字节左移一位
            if (DHT11) {
                buf[i] |= 1;   //如果读到高电平 将buf[i]字节的对应位设置为1
                
                // 等待高电平结束
                timeout = 10000;
                while (DHT11 && timeout--);
                if (timeout == 0) return 1;
            }
        }
    }
    
    // 校验数据
    if (buf[4] == (buf[0] + buf[1] + buf[2] + buf[3])) {
        // 处理温度负数（手册说明：温度<0时，小数部分最高位为1）
        if (buf[3] & 0x80) {
            *temp = -(float)(buf[2] + (buf[3] & 0x7F) * 0.1);
        } else {
            *temp = (float)buf[2] + (float)buf[3] * 0.1;
        }
        *humi = (float)buf[0] + (float)buf[1] * 0.1;
        return 0; // 成功
    } else {
        return 2; // 校验错误
    }
}

void OLED_First_UI(void)
{
    float temperature, humidity;
    unsigned char result;
    
    // 读取温湿度
    result = DHT11_Read(&temperature, &humidity);
    
    // 清除显示区域
    OLED_ClearLine(2);
    OLED_ClearLine(3);
    OLED_ClearLine(4);
    OLED_ClearLine(5);
    OLED_ClearLine(6);
    OLED_ClearLine(7);
    
    // 显示温度
    OLED_ShowCHinese(0, 2, 10);  // 温
    OLED_ShowCHinese(16, 2, 12); // 度
    OLED_ShowChar(32, 2, ':', 16);
    
    if(result == 0) {
        OLED_ShowNum_Float(48, 2, temperature);
        
    } else {
        OLED_ShowString(48, 2, "Error");
    }
    
    // 显示湿度
    OLED_ShowCHinese(0, 4, 10);  // 湿
    OLED_ShowCHinese(16, 4, 12); // 度
    OLED_ShowChar(32, 4, ':', 16);
    
    if(result == 0) {
        OLED_ShowNum_Float(40, 4, humidity);
        OLED_ShowChar(100, 4, '%', 16);
    } else {
        OLED_ShowString(40, 4, "Error");
    }
    
    // 显示CO2（假设值）
    OLED_ShowString(0, 6, "Co2:");
    OLED_ShowNum(32, 6, 450, 3, 16);
    OLED_ShowString(56, 6, "ppm");
    
    // 执行一次滚动步骤
    OLED_ScrollStep();
}

void main()
{
    static unsigned char arr[]="hello world atc32g12k128";

    EAXFR = 1; 									// 使能访问 XFR
    CKCON = 0x00; 							// 设置外部数据总线速度为最快
    WTST = 0x00;								// 设置程序代码等待参数，等待时间为0个时钟，CPU执行程序速度最快

    P0M1 = 0x00;P0M0 = 0x00;		// 设置P0口为准双向口模式
    P1M1 = 0x00;P1M0 = 0x00;		// 设置P1口为准双向口模式
    P2M1 = 0x00;P2M0 = 0x00;		// 设置P2口为准双向口模式
    P3M1 = 0x00;P3M0 = 0x00;		// 设置P3口为准双向口模式
    
    OLED_Init();
    OLED_Clear();
    
    while(1) {
        OLED_First_UI();      // 更新UI和传感器数据
        Delay_ms(100);        // 控制滚动速度
    }
}