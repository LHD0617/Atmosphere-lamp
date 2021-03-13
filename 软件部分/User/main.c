/*---------------------------------------------------------------------
                            睿龙科技协会

【平    台】STC8G1K08A
【编    写】黑龙江工程学院——满心欢喜
【联系方式】QQ：320388825 微信：LHD0617_
【最后更新】2021.03.13
【编译平台】MDK 5.28.0
【功    能】氛围灯工程
【注意事项】本程序只供学习使用，未经作者许可，不得用于其它任何用途。
---------------------------------------------------------------------*/


//引入头文件
#include "reg51.h"
#include "intrins.h"
#include <stdlib.h>

//定义数据类型
typedef unsigned char 					uint8;
typedef unsigned short int 			uint16;
typedef unsigned int				 		uint32;



//定义颜色
#define R	0
#define G 1
#define B 2
//灯的个数
#define LED_num 50
//最大功能数量
#define MAX_Mode 8
//彩灯数据数组
unsigned char Light_data[3][LED_num];
//电位器采集
uint16 AD;
//彩灯模式
uint8 mode;
//MyFlash地址
uint8 address=0x0300;




//小灯相关寄存器
sfr 	P3M0 				= 0xb2;
sfr 	P3M1 				= 0xb1;
sfr 	P5M0 				= 0xca;
sfr 	P5M1 				= 0xc9;
sfr 	ADC_CONTR		=	0xbc;
sfr 	ADC_RES			=	0xbd;
sfr 	ADC_RESL		=	0xbe;
sfr 	ADCCFG			=	0xde;
sfr 	P_SW2				=	0xba;
sfr 	P5					=	0xc8;
sfr	 	IAP_DATA    = 0xc2;
sfr 	IAP_ADDRH   = 0xc3;
sfr 	IAP_ADDRL  	= 0xc4;
sfr 	IAP_CMD    	= 0xc5;
sfr 	IAP_TRIG    = 0xc6;
sfr 	IAP_CONTR   = 0xc7;
sfr 	IAP_TPS		  = 0xF5;
sbit WS2812 = P3^3;
sbit Button = P5^5;
#define IAP_OFFSET	0x2000
#define P5PU      	(*(uint8 volatile xdata *)0xfe15)
#define ADCTIM			(*(uint8 volatile xdata *)0xfea8)
 




//函数声明
void Write_1();
void Write_0();
void Write_RST(void);
void System_Init(void);
void Write_Byte(uint8 byte);
void IAP_Disable(void);
uint8 Byte_Read(unsigned int add);
void Byte_Program(unsigned int add,unsigned char ch);
void Sector_Erase(unsigned int add);
void Write_24Bits(uint8 green,uint8 red,uint8 blue);
void theaterChase(unsigned long c, unsigned int wait) ;
void Display(void);
void Delay1ms(void);
void Delay_ms(uint16 ms);
uint16 ADC_Once(void);
void Clean_data(void);
void Solid_color_mode(void);
void Everbright_mode(void);
void Kaleidoscope1_mode(void);
void Kaleidoscope2_mode(void);
void Dream_mode(void);
uint8 Pattern_recongnition(void);
void Randomcolor_water_lamp_Mode(void);
void Meteor_taillight_Mode(void);
void Random_color_breathing_lamp_Mode(void);
void Random_color_Mode(void);

 
//------------------------------------------
//【函数名称】main
//【功    能】主函数
//------------------------------------------
void main(void)
{
	//系统初始化
	System_Init();
	//Flash读取模式
	mode=Byte_Read(address);
	while(1)
	{
		switch(mode)
		{
			//常亮模式
			case 0:		Everbright_mode();										break;
			//纯色模式
			case 1: 	Solid_color_mode(); 									break;
			//单色万花筒
			case 2:		Kaleidoscope1_mode();									break;
			//流动万花筒
			case 3:		Kaleidoscope2_mode();									break;
			//梦幻模式
			case 4:		Dream_mode();													break;
			//随机颜色流水灯
			case 5:		Randomcolor_water_lamp_Mode();				break;
			//流星拖尾灯
			case 6:		Meteor_taillight_Mode();							break;
			//随机颜色呼吸灯
			case 7:		Random_color_breathing_lamp_Mode();		break;
			//随机颜色模式
			case 8:		Random_color_Mode();									break;
			default:	mode = 0;
		}
	}
}

/////////////////////////////////////////////////////模式操作/////////////////////////////////////////////////////

//------------------------------------------
//【函数名称】Everbright_mode
//【功    能】常亮模式
//------------------------------------------
void Everbright_mode(void)
{
	uint8 i;
	Clean_data();
	while(1)
	{
		AD=ADC_Once();
		for(i=0;i<LED_num;i++)
		{
			Light_data[R][i]=AD/4;
			Light_data[G][i]=AD/4;
			Light_data[B][i]=AD/4;
		}
		Display();
		if(Pattern_recongnition()==1)	break;
	}
}

//------------------------------------------
//【函数名称】Solid_color_mode
//【功    能】纯色模式
//------------------------------------------
void Solid_color_mode(void)
{
	uint8 i;
	Clean_data();
	while(1)
	{
		AD=ADC_Once();
		if(AD<342)
			for(i=0;i<LED_num;i++)
			{
				Light_data[R][i]=255-AD*0.74;
				Light_data[G][i]=AD*0.74;
				Light_data[B][i]=0;
			}
		if(342<=AD&&AD<684)
			for(i=0;i<LED_num;i++)
			{
				Light_data[R][i]=0;
				Light_data[G][i]=255-(AD-342)*0.74;
				Light_data[B][i]=(AD-342)*0.74;
			}
		if(684<=AD&&AD<1024)
			for(i=0;i<LED_num;i++)
			{
				Light_data[R][i]=(AD-684)*0.74;
				Light_data[G][i]=0;
				Light_data[B][i]=255-(AD-684)*0.74;
			}
		Display();
		if(Pattern_recongnition()==1)	break;
	}
}

//------------------------------------------
//【函数名称】Kaleidoscope1_mode
//【功    能】单色万花筒
//------------------------------------------
void Kaleidoscope1_mode(void)
{
	uint8 i,flag=0;
	uint16 color;
	Clean_data();
	while(1)
	{
		for(color=0;color<768;color++)
		{
			AD=ADC_Once();
			if(color<256)
				for(i=0;i<LED_num;i++)
				{
					Light_data[R][i]=255-color;
					Light_data[G][i]=color;
					Light_data[B][i]=0;
				}
			if(256<=color&&color<512)
				for(i=0;i<LED_num;i++)
				{
					Light_data[R][i]=0;
					Light_data[G][i]=255-(color-256);
					Light_data[B][i]=color-256;
				}
			if(512<=color)
				for(i=0;i<LED_num;i++)
				{
					Light_data[R][i]=color-512;
					Light_data[G][i]=0;
					Light_data[B][i]=255-(color-512);
				}
			Delay_ms(AD/100);
			Display();
			if(Pattern_recongnition()==1)
			{
				flag=1;
				break;
			}
		}
		if(flag==1)
			break;
	}
}

//------------------------------------------
//【函数名称】Kaleidoscope2_mode
//【功    能】流动万花筒
//------------------------------------------
void Kaleidoscope2_mode(void)
{
	uint8 i;
	uint16 color=0;
	Clean_data();
	for(i=0;i<LED_num;i++)
	{
		Light_data[R][i]=255-i*8;
		Light_data[G][i]=i*8;
		Light_data[B][i]=0;
	}
	while(1)
	{
		AD=ADC_Once();
		color+=8;
		if(color>767)
			color=0;
		if(color<256)
		{
			Light_data[R][0]=255-color;
			Light_data[G][0]=color;
			Light_data[B][0]=0;
		}
		if(256<=color&&color<512)
		{
			Light_data[R][0]=0;
			Light_data[G][0]=255-(color-256);
			Light_data[B][0]=color-256;
		}
		if(512<=color)
		{
			Light_data[R][0]=color-512;
			Light_data[G][0]=0;
			Light_data[B][0]=255-(color-512);
		}
		for(i=LED_num-1;i>0;i--)
		{
			Light_data[R][i]=Light_data[R][i-1];
			Light_data[G][i]=Light_data[G][i-1];
			Light_data[B][i]=Light_data[B][i-1];
		}
		Delay_ms((AD/32)+10);
		Display();
		if(Pattern_recongnition()==1)	break;
	}
}

//------------------------------------------
//【函数名称】Dream_mode
//【功    能】梦幻模式
//------------------------------------------
void Dream_mode(void)
{
	uint8 i,level;
	Clean_data();
	while(1)
	{
		AD=ADC_Once();
		level = AD/100;
		for(i=0;i<LED_num;i++)
		{
			Light_data[R][i]=0;
			Light_data[G][i]=0;
			Light_data[B][i]=0;
		}
		for(i=0;i<level+5;i++)
		{
			Light_data[R][i]=0;
			Light_data[G][i]=0;
			Light_data[B][i]=255-(255/(level+5))*i;
			Light_data[R][LED_num-i-1]=255-(255/(level+5))*i;
			Light_data[G][LED_num-i-1]=0;
			Light_data[B][LED_num-i-1]=255-(255/(level+5))*i;
		}
		Display();
		if(Pattern_recongnition()==1)	break;
	}
}


//------------------------------------------
//【函数名称】Randomcolor_water_lamp_Mode
//【功    能】随机颜色流水灯模式
//------------------------------------------
void Randomcolor_water_lamp_Mode(void)
{
	uint8 i,j;
	Clean_data();
	while(1)
	{
		AD=ADC_Once();
		if(j==0)
		{
			Light_data[R][0]=(rand()&0xff);
			Light_data[G][0]=(rand()&0xff);
			Light_data[B][0]=(rand()&0xff);
		}
		else 
		{
			Light_data[R][0]=Light_data[R][1];
			Light_data[G][0]=Light_data[G][1];
			Light_data[B][0]=Light_data[B][1];
		}
		j++;
		j%=10;
		for(i=LED_num-1;i>0;i--)
		{
			Light_data[R][i]=Light_data[R][i-1];
			Light_data[G][i]=Light_data[G][i-1];
			Light_data[B][i]=Light_data[B][i-1];
		}
		Delay_ms((AD/100)+5);
		Display();
		if(Pattern_recongnition()==1)	break;
	}
}

//------------------------------------------
//【函数名称】Meteor_taillight_Mode
//【功    能】流星拖尾灯模式
//------------------------------------------
void Meteor_taillight_Mode(void)
{
	uint8 i,j;
	Clean_data();
	while(1)
	{
		AD=ADC_Once();
		if(j==0)
		{
			Light_data[R][0]=(rand()&0xff);
			Light_data[G][0]=(rand()&0xff);
			Light_data[B][0]=(rand()&0xff);
		}
		else if(j<16)
		{
			if(j%2==0)
			{
				Light_data[R][0]>>=1;
				Light_data[G][0]>>=1;
				Light_data[B][0]>>=1;
			}
		}
		else
		{
			Light_data[R][0]=0;
			Light_data[G][0]=0;
			Light_data[B][0]=0;
		}
		for(i=LED_num-1;i>0;i--)
		{
			Light_data[R][i]=Light_data[R][i-1];
			Light_data[G][i]=Light_data[G][i-1];
			Light_data[B][i]=Light_data[B][i-1];
		}
		Delay_ms((AD/100)+5);
		Display();
		if(Pattern_recongnition()==1)	break;
		j++;
		j%=50;
	}
}

//------------------------------------------
//【函数名称】Random_color_breathing_lamp_Mode
//【功    能】随机颜色呼吸灯模式
//------------------------------------------
void Random_color_breathing_lamp_Mode(void)
{
	uint8 i=0,j,randnum;
	Clean_data();
	randnum = (rand()&0xff);
	while(1)
	{
		AD=ADC_Once();
		if(i>=0xfe)
		{
			randnum = (rand()&0xff);
			i=0;
		}
		if((randnum&0x07)==0)	randnum=1;
		if(i<127)
		{
			if(randnum&0x01)	for(j=0;j<LED_num;j++)	Light_data[R][j]+=2;
			if(randnum&0x02)	for(j=0;j<LED_num;j++)	Light_data[G][j]+=2;
			if(randnum&0x04)	for(j=0;j<LED_num;j++)	Light_data[B][j]+=2;
		}
		else
		{
			if(randnum&0x01)	for(j=0;j<LED_num;j++)	Light_data[R][j]-=2;
			if(randnum&0x02)	for(j=0;j<LED_num;j++)	Light_data[G][j]-=2;
			if(randnum&0x04)	for(j=0;j<LED_num;j++)	Light_data[B][j]-=2;
		}
		Delay_ms((AD/100)+1);
		Display();
		i++;
		if(Pattern_recongnition()==1)	break;
	}
}

//------------------------------------------
//【函数名称】Random_color_Mode
//【功    能】随机颜色模式
//------------------------------------------
void Random_color_Mode(void)
{
	uint8 i;
	uint32 color;
	Clean_data();
	while(1)
	{
		AD=ADC_Once();
		for(i=0;i<LED_num;i++)
		{
			color=(rand()&0xffffff);
			Light_data[R][i]=color>>4;
			Light_data[G][i]=color>>2;
			Light_data[B][i]=color;
		}
		Delay_ms((AD/100)+5);
		Display();
		if(Pattern_recongnition()==1)
			break;
	}
}



//------------------------------------------
//【函数名称】Pattern_recongnition
//【功    能】检测模式是否切换
//------------------------------------------
uint8 Pattern_recongnition(void)
{
	if(Button==0)
	{
		mode++;
		if(mode>MAX_Mode)
		{
			mode = 0;
		}
		Sector_Erase(address);
		Byte_Program(address,mode);
		Delay_ms(120);
		return 1;
	}
	else
		return 0;
}	

/////////////////////////////////////////////////////系统操作/////////////////////////////////////////////////////

//------------------------------------------
//【函数名称】ADC_Once
//【功    能】读取电位器电压
//------------------------------------------
uint16 ADC_Once(void)
{
	ADC_CONTR=0xC4;
	_nop_(); _nop_();
	while(!(ADC_CONTR&0x20));
	return (ADC_RES<<2)+(ADC_RESL>>6);
}

//------------------------------------------
//【函数名称】System_Init
//【功    能】系统初始化
//------------------------------------------
void System_Init(void)
{
	uint8 i;
	for(i=0;i<LED_num;i++)
	{
		Light_data[R][i]=0;
		Light_data[G][i]=0;
		Light_data[B][i]=0;
	}
	P_SW2|=0x80;
	P3M0|=0x01<<3;
	P3M1&=~(0x01<<3);
	P5M0&=~(0x01<<5);
	P5M1|=0x01<<5;
	P5M0&=~(0x01<<4);
	P5M1|=0x01<<4;
	P5PU|=0x01<<5;
	P_SW2|=0x80;
	ADCTIM=0x3f;
	P_SW2&=0x7f;
	ADCCFG=0x0f;
	ADC_CONTR=0x80;
}

/////////////////////////////////////////////////////WS2812操作/////////////////////////////////////////////////////

//------------------------------------------
//【函数名称】Write_1
//【功    能】写1码
//1码，高电平850ns 低电平400ns 误差正负150ns
//------------------------------------------
void Write_1(void)
{
	WS2812 = 1;
	_nop_(); _nop_(); _nop_();
	WS2812 = 0;
	_nop_();
}

//------------------------------------------
//【函数名称】Write_0
//【功    能】写0码
//0码，高电平400ns 低电平850ns 误差正负150ns
//------------------------------------------
void Write_0(void)
{
	WS2812 = 1;
	_nop_();
	WS2812 = 0;
	_nop_(); _nop_(); _nop_();  
}

//------------------------------------------
//【函数名称】Write_RST
//【功    能】写重置码
//数据线拉低50us
//------------------------------------------
void Write_RST(void)
{
	uint8 i=2,j=15;
	WS2812 = 0;
	_nop_();
	_nop_();
	while (--i)
		while (--j);
}

//------------------------------------------
//【函数名称】Write_Byte
//【功    能】写一个字节
//------------------------------------------
void Write_Byte(uint8 byte)
{
	uint8 i;
	for(i=0;i<8;i++)
	{
		if(byte&0x80)
			Write_1();
		else
			Write_0();
		byte <<= 1;
	}
}

//------------------------------------------
//【函数名称】Write_24Bits
//【功    能】写24个比特
//------------------------------------------
void Write_24Bits(uint8 green,uint8 red,uint8 blue)
{
	Write_Byte(green);
	Write_Byte(red);
	Write_Byte(blue);
}

//------------------------------------------
//【函数名称】Display
//【功    能】显示小灯数据
//------------------------------------------
void Display(void)
{
	uint8 i;
	for(i=0;i<LED_num;i++)
	{
		Write_24Bits(Light_data[G][i],Light_data[R][i],Light_data[B][i]);
	}
	Write_RST();
}

//------------------------------------------
//【函数名称】Clean_data
//【功    能】清空小灯数据
//------------------------------------------
void Clean_data(void)
{
	uint8 i;
	for(i=0;i<LED_num;i++)
	{
		Light_data[R][i]=0;
		Light_data[G][i]=0;
		Light_data[B][i]=0;
	}
}


/////////////////////////////////////////////////////延时操作/////////////////////////////////////////////////////

//------------------------------------------
//【函数名称】Delay1ms
//【功    能】延时1ms
//------------------------------------------
void Delay1ms(void)
{
	unsigned char i, j;
	_nop_();
	_nop_();
	i = 22;
	j = 128;
	do
	{
		while (--j);
	} while (--i);
}

//------------------------------------------
//【函数名称】Delay_ms
//【功    能】延迟单位毫秒
//------------------------------------------
void Delay_ms(uint16 ms)
{
	uint16 i;
	for(i=0;i<ms;i++)
		Delay1ms();
}

////////////////////////////////////////////////////Flash操作////////////////////////////////////////////////////

//------------------------------------------
//【函数名称】IAP_Disable
//【功    能】关闭IAP功能
//------------------------------------------
void IAP_Disable(void)
{
	IAP_CONTR = 0;
	IAP_CMD   = 0;
	IAP_TRIG 	= 0;
	IAP_ADDRH = 0x80;
	IAP_ADDRL = 0;
}

//------------------------------------------
//【函数名称】Byte_Read
//【功    能】EEPROM读一字节子程序
//------------------------------------------
uint8 Byte_Read(uint32 add)
{
	uint8 da;
	IAP_CONTR = 0x80;
	IAP_TPS	= 12;
	IAP_CMD = 0x01;
	IAP_ADDRH = (uint8)(add>>8);
	IAP_ADDRL = (uint8)(add&0xff);
	IAP_TRIG = 0x5a;
	IAP_TRIG = 0xa5;
	_nop_();
	da = IAP_DATA;
	IAP_Disable();
	return da;
}

//------------------------------------------
//【函数名称】Byte_Program
//【功    能】EEPROM字节编程子程序
//------------------------------------------
void Byte_Program(uint32 add,uint8 ch)
{
	IAP_CONTR = 0x80;
	IAP_TPS	= 12;
	IAP_CMD = 0x02;
	IAP_ADDRH = (uint8)(add>>8);
	IAP_ADDRL = (uint8)(add&0xff);
	IAP_DATA = ch;
	IAP_TRIG = 0x5a;
	IAP_TRIG = 0xa5;
	_nop_();
	IAP_Disable();
}

//------------------------------------------
//【函数名称】Sector_Erase
//【功    能】EEPROM擦除扇区子程序
//------------------------------------------  
void Sector_Erase(uint32 add)
{
	IAP_CONTR = 0x80;
	IAP_TPS	= 12;
	IAP_CMD = 0x03;
	IAP_ADDRH = (uint8)(add>>8);
	IAP_ADDRL = (uint8)(add&0xff);
	IAP_TRIG = 0x5a;
	IAP_TRIG = 0xa5;
	_nop_();
	IAP_Disable();
}

