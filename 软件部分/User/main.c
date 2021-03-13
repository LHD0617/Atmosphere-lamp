/*---------------------------------------------------------------------
                            ����Ƽ�Э��

��ƽ    ̨��STC8G1K08A
����    д������������ѧԺ�������Ļ�ϲ
����ϵ��ʽ��QQ��320388825 ΢�ţ�LHD0617_
�������¡�2021.03.13
������ƽ̨��MDK 5.28.0
����    �ܡ���Χ�ƹ���
��ע�����������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;��
---------------------------------------------------------------------*/


//����ͷ�ļ�
#include "reg51.h"
#include "intrins.h"
#include <stdlib.h>

//������������
typedef unsigned char 					uint8;
typedef unsigned short int 			uint16;
typedef unsigned int				 		uint32;



//������ɫ
#define R	0
#define G 1
#define B 2
//�Ƶĸ���
#define LED_num 50
//���������
#define MAX_Mode 8
//�ʵ���������
unsigned char Light_data[3][LED_num];
//��λ���ɼ�
uint16 AD;
//�ʵ�ģʽ
uint8 mode;
//MyFlash��ַ
uint8 address=0x0300;




//С����ؼĴ���
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
 




//��������
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
//���������ơ�main
//����    �ܡ�������
//------------------------------------------
void main(void)
{
	//ϵͳ��ʼ��
	System_Init();
	//Flash��ȡģʽ
	mode=Byte_Read(address);
	while(1)
	{
		switch(mode)
		{
			//����ģʽ
			case 0:		Everbright_mode();										break;
			//��ɫģʽ
			case 1: 	Solid_color_mode(); 									break;
			//��ɫ��Ͳ
			case 2:		Kaleidoscope1_mode();									break;
			//������Ͳ
			case 3:		Kaleidoscope2_mode();									break;
			//�λ�ģʽ
			case 4:		Dream_mode();													break;
			//�����ɫ��ˮ��
			case 5:		Randomcolor_water_lamp_Mode();				break;
			//������β��
			case 6:		Meteor_taillight_Mode();							break;
			//�����ɫ������
			case 7:		Random_color_breathing_lamp_Mode();		break;
			//�����ɫģʽ
			case 8:		Random_color_Mode();									break;
			default:	mode = 0;
		}
	}
}

/////////////////////////////////////////////////////ģʽ����/////////////////////////////////////////////////////

//------------------------------------------
//���������ơ�Everbright_mode
//����    �ܡ�����ģʽ
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
//���������ơ�Solid_color_mode
//����    �ܡ���ɫģʽ
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
//���������ơ�Kaleidoscope1_mode
//����    �ܡ���ɫ��Ͳ
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
//���������ơ�Kaleidoscope2_mode
//����    �ܡ�������Ͳ
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
//���������ơ�Dream_mode
//����    �ܡ��λ�ģʽ
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
//���������ơ�Randomcolor_water_lamp_Mode
//����    �ܡ������ɫ��ˮ��ģʽ
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
//���������ơ�Meteor_taillight_Mode
//����    �ܡ�������β��ģʽ
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
//���������ơ�Random_color_breathing_lamp_Mode
//����    �ܡ������ɫ������ģʽ
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
//���������ơ�Random_color_Mode
//����    �ܡ������ɫģʽ
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
//���������ơ�Pattern_recongnition
//����    �ܡ����ģʽ�Ƿ��л�
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

/////////////////////////////////////////////////////ϵͳ����/////////////////////////////////////////////////////

//------------------------------------------
//���������ơ�ADC_Once
//����    �ܡ���ȡ��λ����ѹ
//------------------------------------------
uint16 ADC_Once(void)
{
	ADC_CONTR=0xC4;
	_nop_(); _nop_();
	while(!(ADC_CONTR&0x20));
	return (ADC_RES<<2)+(ADC_RESL>>6);
}

//------------------------------------------
//���������ơ�System_Init
//����    �ܡ�ϵͳ��ʼ��
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

/////////////////////////////////////////////////////WS2812����/////////////////////////////////////////////////////

//------------------------------------------
//���������ơ�Write_1
//����    �ܡ�д1��
//1�룬�ߵ�ƽ850ns �͵�ƽ400ns �������150ns
//------------------------------------------
void Write_1(void)
{
	WS2812 = 1;
	_nop_(); _nop_(); _nop_();
	WS2812 = 0;
	_nop_();
}

//------------------------------------------
//���������ơ�Write_0
//����    �ܡ�д0��
//0�룬�ߵ�ƽ400ns �͵�ƽ850ns �������150ns
//------------------------------------------
void Write_0(void)
{
	WS2812 = 1;
	_nop_();
	WS2812 = 0;
	_nop_(); _nop_(); _nop_();  
}

//------------------------------------------
//���������ơ�Write_RST
//����    �ܡ�д������
//����������50us
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
//���������ơ�Write_Byte
//����    �ܡ�дһ���ֽ�
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
//���������ơ�Write_24Bits
//����    �ܡ�д24������
//------------------------------------------
void Write_24Bits(uint8 green,uint8 red,uint8 blue)
{
	Write_Byte(green);
	Write_Byte(red);
	Write_Byte(blue);
}

//------------------------------------------
//���������ơ�Display
//����    �ܡ���ʾС������
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
//���������ơ�Clean_data
//����    �ܡ����С������
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


/////////////////////////////////////////////////////��ʱ����/////////////////////////////////////////////////////

//------------------------------------------
//���������ơ�Delay1ms
//����    �ܡ���ʱ1ms
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
//���������ơ�Delay_ms
//����    �ܡ��ӳٵ�λ����
//------------------------------------------
void Delay_ms(uint16 ms)
{
	uint16 i;
	for(i=0;i<ms;i++)
		Delay1ms();
}

////////////////////////////////////////////////////Flash����////////////////////////////////////////////////////

//------------------------------------------
//���������ơ�IAP_Disable
//����    �ܡ��ر�IAP����
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
//���������ơ�Byte_Read
//����    �ܡ�EEPROM��һ�ֽ��ӳ���
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
//���������ơ�Byte_Program
//����    �ܡ�EEPROM�ֽڱ���ӳ���
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
//���������ơ�Sector_Erase
//����    �ܡ�EEPROM���������ӳ���
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

