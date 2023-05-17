#include <reg51.h> 
#define uint unsigned int
#define uchar unsigned char   	//宏定义
#include <intrins.h>
#include "eeprom52.h"
#define LCD1602 P0

sbit ADC_CS   = P1^3; 				// ADC0832的CS引脚
sbit ADC_CLK  = P1^4; 				// ADC0832的CLK引脚
sbit ADC_DAT  = P1^5; 				// ADC0832的DI/DO引脚

//LCD定义
#define LCD_DB P0 
sbit LCD_RS=P2^5;   
sbit LCD_RW=P2^6;  
sbit LCD_E=P2^7;  

sbit DJ=P1^1;  


uint Time50ms=0;

sbit SW=P3^0; 
bit flag=1;
uchar Mode1=0;
sbit DJ1=P3^4; 
sbit DJ2=P3^5; 
uchar num0,num1;			// 保存亮度值
/*-------------------------------------
			   函数申明
---------------------------------------*/
void LCD_init(void);                          //初始化函数 
void LCD_write_command(uchar command);        //写指令函数 
void LCD_write_data(uchar dat);               //写数据函数  
void LCD_disp_char(uchar x,uchar y,uchar dat);//在某个屏幕位置上显示一个字符,X（0-15),y(1-2)  
void LCD_disp_str(uchar x,uchar y,uchar *str); //LCD1602显示字符串函数 
void delay_n10us(uint n);                     //延时函数

sbit SET=P3^1;    			//定义调整键
sbit DEC=P3^2;    			//定义减少键
sbit ADD=P3^3;    			//定义增加键
sbit BUZZ=P3^6;    			//定义蜂鸣器
sbit ALAM=P1^2;				  //定义灯光报警
sbit DQ=P3^7;     			//定义DS18B20总线I/O	
sbit RS = P2^5;
sbit RW = P2^6;
sbit EN = P2^7;
sbit LED1 = P1^6;
sbit LED2 = P1^7;
bit jiemian = 0;
bit shanshuo_st;    			//闪烁间隔标志
bit beep_st;     				//蜂鸣器间隔标志
uchar x=0;      				//计数器

uchar code tab1[]={"Now Tem:   .  C "};
uchar code tab2[]={"TH:   C  TL:   C"};
uint c;
uchar Mode = 0;     	//状态标志
signed char TH;  		  //上限报警温度
signed char TL;   		//下限报警温度

/*-------------------------------------
			   LCD1602子程序
---------------------------------------*/
void LCD_init(void) 
{  
	delay_n10us(10);  
	LCD_write_command(0x38);//设置8位格式，2行，5x7 
	delay_n10us(10);  LCD_write_command(0x0c);//开显示，关光标，不闪烁 
	delay_n10us(10);  LCD_write_command(0x06);//设定输入方式，增量不移位 
	delay_n10us(10);  LCD_write_command(0x01);//清除屏幕显示  
	delay_n10us(100);       //延时清屏，延时函数，延时约n个10us 
}

void LCD_write_command(uchar dat) 
{  
	delay_n10us(10);  
	LCD_RS=0;         //指令 
	LCD_RW=0;         //写入 
	LCD_E=1;          //允许 
	LCD_DB=dat;  delay_n10us(10);  //实践证明，我的LCD1602上，用for循环1次就能完成普通写指令。 
	LCD_E=0;  delay_n10us(10);  //实践证明，我的LCD1602上，用for循环1次就能完成普通写指令。 
}

void LCD_write_data(uchar dat) 
{  
	delay_n10us(10);  
	LCD_RS=1;          //数据 
	LCD_RW=0;		   //写入
	LCD_E=1;           //允许 
	LCD_DB=dat; delay_n10us(10);
	 LCD_E=0;  delay_n10us(10); 
}

void LCD_disp_char(uchar x,uchar y,uchar dat) 
{    
	uchar address;   
	if(y==1)           
	address=0x80+x;  
	else           
	address=0xc0+x;    
	LCD_write_command(address);   
	LCD_write_data(dat); 
}

void LCD_disp_str(uchar x,uchar y,uchar *str) 
{    
	uchar address;   
	if(y==1)           
	address=0x80+x;   
	else           
	address=0xc0+x;    
	LCD_write_command(address);   
	while(*str!='\0')   
		{       
		LCD_write_data(*str);
		str++;
		}
}

void delay_n10us(uint n)  //延时 
{                 
	uint i;                    
	for(i=n;i>0;i--)            
	 {         
	 nop_();_nop_();_nop_();_nop_();_nop_();_nop_();    
	 } 
}






/*********************************************************/
// ADC0832的时钟脉冲
/*********************************************************/
void WavePlus()
{
	_nop_();
	ADC_CLK = 1;
	_nop_();
	ADC_CLK = 0;
}



/*********************************************************/
// 获取指定通道的A/D转换结果
/*********************************************************/
uchar Get_ADC0832(uchar chanel)
{ 
	uchar i;
	uchar dat1=0;
	uchar dat2=0;
	
	ADC_CLK = 0;				// 电平初始化
	ADC_DAT = 1;
	_nop_();
	ADC_CS = 0;
	WavePlus();					// 起始信号 
	ADC_DAT = 1;
	WavePlus();					// 通道选择的第一位
	ADC_DAT = chanel;      
	WavePlus();					// 通道选择的第二位
	ADC_DAT = 1;
	
	for(i=0;i<8;i++)		// 第一次读取
	{
		dat1<<=1;
		WavePlus();
		if(ADC_DAT)
			dat1=dat1|0x01;
		else
			dat1=dat1|0x00;
	}
	
	for(i=0;i<8;i++)		// 第二次读取
	{
		dat2>>= 1;
		if(ADC_DAT)
			dat2=dat2|0x80;
		else
			dat2=dat2|0x00;
		WavePlus();
	}
	
	_nop_();						// 结束此次传输
	ADC_DAT = 1;
	ADC_CLK = 1;
	ADC_CS  = 1;   

	if(dat1==dat2)			// 返回采集结果
		return dat1;
	else
		return 0;
} 



void delay_ms(unsigned int ms)// 延时子程序
{   
  unsigned int a,b;
  for(a=ms;a>0;a--)
  for(b=123;b>0;b--);
}

/*********************************************************/
// 卷帘门驱动
/*********************************************************/
void AlarmJudge()
{
	if(Mode1==0)
	{
		if(num0>100)
		{ 
			DJ1=0;
			delay_ms(1500);
			DJ1=1;
			Mode1=1;
		}
	}
	else if(Mode1==1)
	{
		if(num0<=100)
		{
			DJ2=0;
			delay_ms(1500);
			DJ2=1;
			Mode1=0;
		}
	}
}


void LCD()
{
	LCD_disp_char(3,1,num0/100+'0'); //显示 
	LCD_disp_char(4,1,num0/10%10+'0'); //显示 
	LCD_disp_char(5,1,num0%10+'0'); //显示 

	if(Mode1==0)
	{
		LCD_disp_str(13,1,"OFF"); //显示 关
	}
	else if(Mode1==1)
	{
		LCD_disp_str(13,1," ON"); //显示 开  
	}
}



//////////////////////////////////////////////////////////////////////////////////

/******************把数据保存到单片机内部eeprom中******************/
void write_eeprom()
{
	SectorErase(0x2000);
	byte_write(0x2000, TH);
	byte_write(0x2001, TL);
	byte_write(0x2060, a_a);	
}

/******************把数据从单片机内部eeprom中读出来*****************/
void read_eeprom()
{
	TH = byte_read(0x2000);
	TL = byte_read(0x2001);
	a_a      = byte_read(0x2060);
}

/**************开机自检eeprom初始化*****************/
void init_eeprom() 
{
	read_eeprom();		//先读
	if(a_a != 1)		//新的单片机初始单片机内问eeprom
	{
		TH = 40;
		TL = 10;
		a_a = 1;
		write_eeprom();	   //保存数据
	}	
}

//============================================================================================
//====================================DS18B20=================================================
//============================================================================================
/*****延时子程序*****/
void Delay_DS18B20(int num)
{
  while(num--) ;
}
void delay(uint xms)//延时函数，有参函数
{
	uint x,y;
	for(x=xms;x>0;x--)
	 for(y=110;y>0;y--);
}
/*****初始化DS18B20*****/
void Init_DS18B20(void)
{
  unsigned char x=0;
  DQ = 1;         //DQ复位
  Delay_DS18B20(8);    //稍做延时
  DQ = 0;         //单片机将DQ拉低
  Delay_DS18B20(80);   //精确延时，大于480us
  DQ = 1;         //拉高总线
  Delay_DS18B20(14);
  x = DQ;           //稍做延时后，如果x=0则初始化成功，x=1则初始化失败
  Delay_DS18B20(20);
}
/*****读一个字节*****/
unsigned char ReadOneChar(void)
{
  unsigned char i=0;
  unsigned char dat = 0;
  for (i=8;i>0;i--)
  {
    DQ = 0;     // 给脉冲信号
    dat>>=1;
    DQ = 1;     // 给脉冲信号
    if(DQ)
    dat|=0x80;
    Delay_DS18B20(4);
  }
  return(dat);
}
/*****写一个字节*****/
void WriteOneChar(unsigned char dat)
{
  unsigned char i=0;
  for (i=8; i>0; i--)
  {
    DQ = 0;
    DQ = dat&0x01;
    Delay_DS18B20(5);
    DQ = 1;
    dat>>=1;
  }
}
/*****读取温度*****/
unsigned int ReadTemperature(void)
{
  unsigned char a=0;
  unsigned char b=0;
  unsigned int t=0;
  float tt=0;
  Init_DS18B20();
  WriteOneChar(0xCC);  //跳过读序号列号的操作
  WriteOneChar(0x44);  //启动温度转换
  Init_DS18B20();
  WriteOneChar(0xCC);  //跳过读序号列号的操作
  WriteOneChar(0xBE);  //读取温度寄存器
  a=ReadOneChar();     //读低8位
  b=ReadOneChar();    //读高8位
  t=b;
  t<<=8;
  t=t|a;
  tt=t*0.0625;
 // t= tt*10+0.5;     //放大10倍输出并四舍五入
  t= tt*10+0.5; 
  return(t);
}

/*****读取温度*****/
void check_wendu(void)
{
	c=ReadTemperature();  			//获取温度值并减去DS18B20的温漂误差
	if(c>1200)
	c=1200;
}

/********液晶写入指令函数与写入数据函数，以后可调用**************/

void write_1602com(uchar com)//****液晶写入指令函数****
{
	RS=0;//数据/指令选择置为指令
	RW=0; //读写选择置为写
	LCD1602=com;//送入数据
	delay(1);
	EN=1;//拉高使能端，为制造有效的下降沿做准备
	delay(1);
	EN=0;//en由高变低，产生下降沿，液晶执行命令
}


void write_1602dat(uchar dat)//***液晶写入数据函数****
{
	RS=1;//数据/指令选择置为数据
	RW=0; //读写选择置为写
	LCD1602=dat;//送入数据
	delay(1);
	EN=1; //en置高电平，为制造下降沿做准备
	delay(1);
	EN=0; //en由高变低，产生下降沿，液晶执行命令
}


void lcd_init()//***液晶初始化函数****
{
	uchar a;
	write_1602com(0x38);//设置液晶工作模式，意思：16*2行显示，5*7点阵，8位数据
	write_1602com(0x0c);//开显示不显示光标
	write_1602com(0x06);//整屏不移动，光标自动右移
	write_1602com(0x01);//清显示

	write_1602com(0x80);//日历显示固定符号从第一行第1个位置之后开始显示
	for(a=0;a<16;a++)
	{
		write_1602dat(tab1[a]);//向液晶屏写日历显示的固定符号部分
		delay(3);
	}
	write_1602com(0x80+0x40);//时间显示固定符号写入位置，从第2个位置后开始显示
	for(a=0;a<16;a++)
	{
		write_1602dat(tab2[a]);//写显示时间固定符号，两个冒号
		delay(3);
	}

}

void display()
{
	if(Mode==0)
	{
		write_1602com(0x80+8);
		write_1602dat(c/1000+0x30);
		write_1602dat((c%1000)/100+0x30);
		write_1602dat(((c%1000)%100)/10+0x30);
		write_1602com(0x80+12);
		write_1602dat(((c%1000)%100)%10+0x30);
		write_1602com(0x80+13);
		write_1602dat(0xdf);
		write_1602com(0x80+0x40+3);
		write_1602dat(TH/10+0x30);
		write_1602dat(TH%10+0x30);
		write_1602dat(0xdf);
		write_1602com(0x80+0x40+12);
		write_1602dat(TL/10+0x30);
		write_1602dat(TL%10+0x30);
		write_1602dat(0xdf);			
	}								  
}
//=====================================================================================

/*****初始化定时器0*****/
void InitTimer(void)
{
	TMOD=0x1;
	TH0=0x3c;
	TL0=0xb0;     //50ms（晶振12M）
	EA=1;      //全局中断开关
	TR0=1;
	ET0=1;      //开启定时器0
}

void KEY()
{
			//功能键
	if(SET==0)
	{
		delay(10);
		if(SET==0)
		{	
			BUZZ=0;
			Mode++;
			if(Mode>=3)
			{
				Mode=0;
				write_eeprom();
			}
			delay(50);
			BUZZ=1;
		}
		while(SET==0)
		{
			if(Mode==0)
				{
				  write_1602com(0x80+0x40+6);
					write_1602com(0x0c);
				}	
			else if(Mode==1)
				{
					write_1602com(0x80+0x40+4);
					write_1602com(0x0f);
				}	
			else
				{
					write_1602com(0x80+0x40+13);
					write_1602com(0x0f);
				}							
		}
	}
	//增加
	if(ADD==0&&Mode==1)
	{
		delay(10);
		if(ADD==0)	
		{
			TH++;
			if(TH>=99)	
			TH=99;
			write_1602com(0x80+0x40+3);
			write_1602dat(TH/10+0x30);
			write_1602dat(TH%10+0x30);
			write_1602com(0x80+0x40+4);	
			BUZZ=0;
			delay(50);
			BUZZ=1;
		}
		while(ADD==0);
	}
	//减少
	if(DEC==0&&Mode==1)
	{
		delay(10);
		if(DEC==0)
		{
			TH--;
			if(TH==TL)	
			TH=TL+1;
			write_1602com(0x80+0x40+3);
			write_1602dat(TH/10+0x30);
			write_1602dat(TH%10+0x30);
			write_1602com(0x80+0x40+4);	
			BUZZ=0;
			delay(50);
			BUZZ=1;
		}
		while(DEC==0);
	}
	if(ADD==0&&Mode==2)
	{
		delay(10);
		if(ADD==0)	
		{
			TL++;
			if(TL==TH)	
			TL=TH-1;
			write_1602com(0x80+0x40+12);
			write_1602dat(TL/10+0x30);
			write_1602dat(TL%10+0x30);
			write_1602com(0x80+0x40+13);	
			BUZZ=0;
			delay(50);
			BUZZ=1;
		}
		while(ADD==0);
		
	}
	//减少
	if(DEC==0&&Mode==2)
	{
		delay(10);
		if(DEC==0)
		{
			TL--;
			if(TL<=0)	
			TL=0;
			write_1602com(0x80+0x40+12);
			write_1602dat(TL/10+0x30);
			write_1602dat(TL%10+0x30);
			write_1602com(0x80+0x40+13);	
			BUZZ=0;
			delay(50);
			BUZZ=1;
		}
		while(DEC==0);		
	}
}

/*****报警子程序*****/
void Alarm()
{
	if(x>=10){beep_st=~beep_st;x=0;}
	if(Mode==0)
	{
		if((c/10)>=TH||(c/10)<TL)
		{
			ALAM=0;
			if(beep_st==1)
			BUZZ=0;
			else
			BUZZ=1;
			
		}
		else
		{
			BUZZ=1;
			ALAM=1;	
		}
		if((c/10)>=TH)
			{LED1=0;}
		    else
		    {LED1=1;}
		    if((c/10)<=TL)
		    {LED2=0;}
		    else
		    {LED2=1;}
		
	}
	else
	{
		BUZZ=1;
		ALAM=1;
	}
}

////////////////////////////////////////////////////////////////////////
void hongwai()
{
	LCD_init();//LCD初始化              
	LCD_disp_str(0,1,"XH:000       OFF"); //显示 
	
	while(1)
	{
		num0=Get_ADC0832(0);				// 获取检测是否有人
		
		LCD();
		AlarmJudge();			// 卷帘门驱动
	}
}
/*****主函数*****/
void main(void)
{
	uint z; // 定义变量
	hongwai();
	delay(1);
	lcd_init();
	delay(1);
	init_eeprom();
	InitTimer();    //初始化定时器
	
	for(z=0;z<100;z++)
	{
		check_wendu();
		delay(1);        
	} 

	while(1)
	{		
		display();
		if(jiemian==1)
		{
			KEY();
		}
		else
		{
			LCD();//中断切换显示界面
		}
		Alarm(); 
		check_wendu();		
	}
}

/*****定时器0中断服务程序*****/
void timer0(void) interrupt 1
{
 TH0=0x3c;
 TL0=0xb0;
 x++;
}
void X0(void) interrupt 0
{
 jiemian=~jiemian;
}