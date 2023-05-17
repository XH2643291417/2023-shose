#include <reg51.h> 
#define uint unsigned int
#define uchar unsigned char   	//�궨��
#include <intrins.h>
#include "eeprom52.h"
#define LCD1602 P0

sbit ADC_CS   = P1^3; 				// ADC0832��CS����
sbit ADC_CLK  = P1^4; 				// ADC0832��CLK����
sbit ADC_DAT  = P1^5; 				// ADC0832��DI/DO����

//LCD����
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
uchar num0,num1;			// ��������ֵ
/*-------------------------------------
			   ��������
---------------------------------------*/
void LCD_init(void);                          //��ʼ������ 
void LCD_write_command(uchar command);        //дָ��� 
void LCD_write_data(uchar dat);               //д���ݺ���  
void LCD_disp_char(uchar x,uchar y,uchar dat);//��ĳ����Ļλ������ʾһ���ַ�,X��0-15),y(1-2)  
void LCD_disp_str(uchar x,uchar y,uchar *str); //LCD1602��ʾ�ַ������� 
void delay_n10us(uint n);                     //��ʱ����

sbit SET=P3^1;    			//���������
sbit DEC=P3^2;    			//������ټ�
sbit ADD=P3^3;    			//�������Ӽ�
sbit BUZZ=P3^6;    			//���������
sbit ALAM=P1^2;				  //����ƹⱨ��
sbit DQ=P3^7;     			//����DS18B20����I/O	
sbit RS = P2^5;
sbit RW = P2^6;
sbit EN = P2^7;
sbit LED1 = P1^6;
sbit LED2 = P1^7;
bit jiemian = 0;
bit shanshuo_st;    			//��˸�����־
bit beep_st;     				//�����������־
uchar x=0;      				//������

uchar code tab1[]={"Now Tem:   .  C "};
uchar code tab2[]={"TH:   C  TL:   C"};
uint c;
uchar Mode = 0;     	//״̬��־
signed char TH;  		  //���ޱ����¶�
signed char TL;   		//���ޱ����¶�

/*-------------------------------------
			   LCD1602�ӳ���
---------------------------------------*/
void LCD_init(void) 
{  
	delay_n10us(10);  
	LCD_write_command(0x38);//����8λ��ʽ��2�У�5x7 
	delay_n10us(10);  LCD_write_command(0x0c);//����ʾ���ع�꣬����˸ 
	delay_n10us(10);  LCD_write_command(0x06);//�趨���뷽ʽ����������λ 
	delay_n10us(10);  LCD_write_command(0x01);//�����Ļ��ʾ  
	delay_n10us(100);       //��ʱ��������ʱ��������ʱԼn��10us 
}

void LCD_write_command(uchar dat) 
{  
	delay_n10us(10);  
	LCD_RS=0;         //ָ�� 
	LCD_RW=0;         //д�� 
	LCD_E=1;          //���� 
	LCD_DB=dat;  delay_n10us(10);  //ʵ��֤�����ҵ�LCD1602�ϣ���forѭ��1�ξ��������ͨдָ� 
	LCD_E=0;  delay_n10us(10);  //ʵ��֤�����ҵ�LCD1602�ϣ���forѭ��1�ξ��������ͨдָ� 
}

void LCD_write_data(uchar dat) 
{  
	delay_n10us(10);  
	LCD_RS=1;          //���� 
	LCD_RW=0;		   //д��
	LCD_E=1;           //���� 
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

void delay_n10us(uint n)  //��ʱ 
{                 
	uint i;                    
	for(i=n;i>0;i--)            
	 {         
	 nop_();_nop_();_nop_();_nop_();_nop_();_nop_();    
	 } 
}






/*********************************************************/
// ADC0832��ʱ������
/*********************************************************/
void WavePlus()
{
	_nop_();
	ADC_CLK = 1;
	_nop_();
	ADC_CLK = 0;
}



/*********************************************************/
// ��ȡָ��ͨ����A/Dת�����
/*********************************************************/
uchar Get_ADC0832(uchar chanel)
{ 
	uchar i;
	uchar dat1=0;
	uchar dat2=0;
	
	ADC_CLK = 0;				// ��ƽ��ʼ��
	ADC_DAT = 1;
	_nop_();
	ADC_CS = 0;
	WavePlus();					// ��ʼ�ź� 
	ADC_DAT = 1;
	WavePlus();					// ͨ��ѡ��ĵ�һλ
	ADC_DAT = chanel;      
	WavePlus();					// ͨ��ѡ��ĵڶ�λ
	ADC_DAT = 1;
	
	for(i=0;i<8;i++)		// ��һ�ζ�ȡ
	{
		dat1<<=1;
		WavePlus();
		if(ADC_DAT)
			dat1=dat1|0x01;
		else
			dat1=dat1|0x00;
	}
	
	for(i=0;i<8;i++)		// �ڶ��ζ�ȡ
	{
		dat2>>= 1;
		if(ADC_DAT)
			dat2=dat2|0x80;
		else
			dat2=dat2|0x00;
		WavePlus();
	}
	
	_nop_();						// �����˴δ���
	ADC_DAT = 1;
	ADC_CLK = 1;
	ADC_CS  = 1;   

	if(dat1==dat2)			// ���زɼ����
		return dat1;
	else
		return 0;
} 



void delay_ms(unsigned int ms)// ��ʱ�ӳ���
{   
  unsigned int a,b;
  for(a=ms;a>0;a--)
  for(b=123;b>0;b--);
}

/*********************************************************/
// ����������
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
	LCD_disp_char(3,1,num0/100+'0'); //��ʾ 
	LCD_disp_char(4,1,num0/10%10+'0'); //��ʾ 
	LCD_disp_char(5,1,num0%10+'0'); //��ʾ 

	if(Mode1==0)
	{
		LCD_disp_str(13,1,"OFF"); //��ʾ ��
	}
	else if(Mode1==1)
	{
		LCD_disp_str(13,1," ON"); //��ʾ ��  
	}
}



//////////////////////////////////////////////////////////////////////////////////

/******************�����ݱ��浽��Ƭ���ڲ�eeprom��******************/
void write_eeprom()
{
	SectorErase(0x2000);
	byte_write(0x2000, TH);
	byte_write(0x2001, TL);
	byte_write(0x2060, a_a);	
}

/******************�����ݴӵ�Ƭ���ڲ�eeprom�ж�����*****************/
void read_eeprom()
{
	TH = byte_read(0x2000);
	TL = byte_read(0x2001);
	a_a      = byte_read(0x2060);
}

/**************�����Լ�eeprom��ʼ��*****************/
void init_eeprom() 
{
	read_eeprom();		//�ȶ�
	if(a_a != 1)		//�µĵ�Ƭ����ʼ��Ƭ������eeprom
	{
		TH = 40;
		TL = 10;
		a_a = 1;
		write_eeprom();	   //��������
	}	
}

//============================================================================================
//====================================DS18B20=================================================
//============================================================================================
/*****��ʱ�ӳ���*****/
void Delay_DS18B20(int num)
{
  while(num--) ;
}
void delay(uint xms)//��ʱ�������вκ���
{
	uint x,y;
	for(x=xms;x>0;x--)
	 for(y=110;y>0;y--);
}
/*****��ʼ��DS18B20*****/
void Init_DS18B20(void)
{
  unsigned char x=0;
  DQ = 1;         //DQ��λ
  Delay_DS18B20(8);    //������ʱ
  DQ = 0;         //��Ƭ����DQ����
  Delay_DS18B20(80);   //��ȷ��ʱ������480us
  DQ = 1;         //��������
  Delay_DS18B20(14);
  x = DQ;           //������ʱ�����x=0���ʼ���ɹ���x=1���ʼ��ʧ��
  Delay_DS18B20(20);
}
/*****��һ���ֽ�*****/
unsigned char ReadOneChar(void)
{
  unsigned char i=0;
  unsigned char dat = 0;
  for (i=8;i>0;i--)
  {
    DQ = 0;     // �������ź�
    dat>>=1;
    DQ = 1;     // �������ź�
    if(DQ)
    dat|=0x80;
    Delay_DS18B20(4);
  }
  return(dat);
}
/*****дһ���ֽ�*****/
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
/*****��ȡ�¶�*****/
unsigned int ReadTemperature(void)
{
  unsigned char a=0;
  unsigned char b=0;
  unsigned int t=0;
  float tt=0;
  Init_DS18B20();
  WriteOneChar(0xCC);  //����������кŵĲ���
  WriteOneChar(0x44);  //�����¶�ת��
  Init_DS18B20();
  WriteOneChar(0xCC);  //����������кŵĲ���
  WriteOneChar(0xBE);  //��ȡ�¶ȼĴ���
  a=ReadOneChar();     //����8λ
  b=ReadOneChar();    //����8λ
  t=b;
  t<<=8;
  t=t|a;
  tt=t*0.0625;
 // t= tt*10+0.5;     //�Ŵ�10���������������
  t= tt*10+0.5; 
  return(t);
}

/*****��ȡ�¶�*****/
void check_wendu(void)
{
	c=ReadTemperature();  			//��ȡ�¶�ֵ����ȥDS18B20����Ư���
	if(c>1200)
	c=1200;
}

/********Һ��д��ָ�����д�����ݺ������Ժ�ɵ���**************/

void write_1602com(uchar com)//****Һ��д��ָ���****
{
	RS=0;//����/ָ��ѡ����Ϊָ��
	RW=0; //��дѡ����Ϊд
	LCD1602=com;//��������
	delay(1);
	EN=1;//����ʹ�ܶˣ�Ϊ������Ч���½�����׼��
	delay(1);
	EN=0;//en�ɸ߱�ͣ������½��أ�Һ��ִ������
}


void write_1602dat(uchar dat)//***Һ��д�����ݺ���****
{
	RS=1;//����/ָ��ѡ����Ϊ����
	RW=0; //��дѡ����Ϊд
	LCD1602=dat;//��������
	delay(1);
	EN=1; //en�øߵ�ƽ��Ϊ�����½�����׼��
	delay(1);
	EN=0; //en�ɸ߱�ͣ������½��أ�Һ��ִ������
}


void lcd_init()//***Һ����ʼ������****
{
	uchar a;
	write_1602com(0x38);//����Һ������ģʽ����˼��16*2����ʾ��5*7����8λ����
	write_1602com(0x0c);//����ʾ����ʾ���
	write_1602com(0x06);//�������ƶ�������Զ�����
	write_1602com(0x01);//����ʾ

	write_1602com(0x80);//������ʾ�̶����Ŵӵ�һ�е�1��λ��֮��ʼ��ʾ
	for(a=0;a<16;a++)
	{
		write_1602dat(tab1[a]);//��Һ����д������ʾ�Ĺ̶����Ų���
		delay(3);
	}
	write_1602com(0x80+0x40);//ʱ����ʾ�̶�����д��λ�ã��ӵ�2��λ�ú�ʼ��ʾ
	for(a=0;a<16;a++)
	{
		write_1602dat(tab2[a]);//д��ʾʱ��̶����ţ�����ð��
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

/*****��ʼ����ʱ��0*****/
void InitTimer(void)
{
	TMOD=0x1;
	TH0=0x3c;
	TL0=0xb0;     //50ms������12M��
	EA=1;      //ȫ���жϿ���
	TR0=1;
	ET0=1;      //������ʱ��0
}

void KEY()
{
			//���ܼ�
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
	//����
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
	//����
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
	//����
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

/*****�����ӳ���*****/
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
	LCD_init();//LCD��ʼ��              
	LCD_disp_str(0,1,"XH:000       OFF"); //��ʾ 
	
	while(1)
	{
		num0=Get_ADC0832(0);				// ��ȡ����Ƿ�����
		
		LCD();
		AlarmJudge();			// ����������
	}
}
/*****������*****/
void main(void)
{
	uint z; // �������
	hongwai();
	delay(1);
	lcd_init();
	delay(1);
	init_eeprom();
	InitTimer();    //��ʼ����ʱ��
	
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
			LCD();//�ж��л���ʾ����
		}
		Alarm(); 
		check_wendu();		
	}
}

/*****��ʱ��0�жϷ������*****/
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