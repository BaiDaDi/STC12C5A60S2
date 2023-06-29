#include <STC12C5A60S2.H>
/*=================================================
*自定义数据类型
=================================================*/
typedef unsigned char uchar;
typedef unsigned int uint;

#define LCD1602_DB P0	//LCD1602数据总线

sbit LED= P1^7;

//定义LCD1602
sbit LCD1602_RS = P4^4;	 //RS端
sbit LCD1602_RW = P4^5;	 //RW端
sbit LCD1602_EN = P4^6;	 //EN端

//TB6612
sbit STBY= P1^2;	
sbit PWMA = P1^3;	 
sbit PWMB = P1^4;	
sbit AIN1 = P1^0;	
sbit AIN2 = P1^1;	
sbit BIN1 = P1^5;	
sbit BIN2 = P1^6;	

sbit OUT1= P2^7;
sbit OUT2= P2^6;
sbit OUT3= P2^5;
sbit OUT4= P2^4;
sbit OUT5= P2^3;

//定义PWM标志位
uchar num_i=0;
uchar num_door=10;
uchar num_flag=100;

/*=================================================
*函数名称：Read_Busy
*函数功能：判断1602液晶忙，并等待
=================================================*/
void Read_Busy()
{
	uchar busy;
	LCD1602_DB = 0xff;//复位数据总线
	LCD1602_RS = 0;	  //拉低RS
	LCD1602_RW = 1;	  //拉高RW读
	do
	{
		LCD1602_EN = 1;//使能EN
		busy = LCD1602_DB;//读回数据
		LCD1602_EN = 0;	 //拉低使能以便于下一次产生上升沿
	}while(busy & 0x80); //判断状态字BIT7位是否为1，为1则表示忙，程序等待
}
/*=================================================
*函数名称：LCD1602_Write_Cmd
*函数功能：写LCD1602命令
*调用：Read_Busy();
*输入：cmd:要写的命令
=================================================*/
void LCD1602_Write_Cmd(uchar cmd)
{
	Read_Busy();	 //判断忙，忙则等待
	LCD1602_RS = 0;
	LCD1602_RW = 0;	//拉低RS、RW操作时序情况1602课件下中文使用说明基本操作时序章节
	LCD1602_DB = cmd;//写入命令
	LCD1602_EN = 1;	 //拉高使能端 数据被传输到LCD1602内
	LCD1602_EN = 0;	 //拉低使能以便于下一次产生上升沿
}
/*=================================================
*函数名称：LCD1602_Write_Dat
*函数功能：写LCD1602数据
*调用：Read_Busy();
*输入：dat：需要写入的数据
=================================================*/
void LCD1602_Write_Dat(uchar dat)
{
	Read_Busy();
	LCD1602_RS = 1;
	LCD1602_RW = 0;
	LCD1602_DB = dat;
	LCD1602_EN = 1;
	LCD1602_EN = 0;
}
/*=================================================
*函数名称：LCD1602_Dis_OneChar
*函数功能：在指定位置显示一个字符
*调用：LCD1602_Write_Cmd(); LCD1602_Write_Dat();	
*输入：x：要显示的横坐标取值0-40，y：要显示的行坐标取值0-1（0为第一行,1为第二行）
		dat：需要显示的数据以ASCLL形式显示
=================================================*/
void LCD1602_Dis_OneChar(uchar x, uchar y,uchar dat)
{
	if(y)	x |= 0x40;
	x |= 0x80;
	LCD1602_Write_Cmd(x);
	LCD1602_Write_Dat(dat);		
}
/*=================================================
*函数名称：LCD1602_Dis_Str
*函数功能：在指定位置显示字符串
*调用：LCD1602_Write_Cmd(); LCD1602_Write_Dat();
*输入：x：要显示的横坐标取值0-40，y：要显示的行坐标取值0-1（0为第一行,1为第二行）
		*str：需要显示的字符串
=================================================*/
void LCD1602_Dis_Str(uchar x, uchar y, uchar *str)
{
	if(y) x |= 0x40;
	x |= 0x80;
	LCD1602_Write_Cmd(x);
	while(*str != '\0')
	{
		LCD1602_Write_Dat(*str++);
	}
}
/*=================================================
*函数名称：Init_LCD1602
*函数功能：1602初始化
*调用：	LCD1602_Write_Cmd();
=================================================*/
void Init_LCD1602()
{
	LCD1602_Write_Cmd(0x38); //	设置16*2显示，5*7点阵，8位数据接口
	LCD1602_Write_Cmd(0x0c); //开显示
	LCD1602_Write_Cmd(0x06); //读写一字节后地址指针加1
	LCD1602_Write_Cmd(0x01); //清除显示
}

void run()//具体可以看TB6612的驱动数据表
{
	AIN1 = 0;
	AIN2 = 1;
	BIN1 = 0;
	BIN2 = 1;
}

void stop()
{
	AIN1 = 0;
	AIN2 = 0;
	BIN1 = 0;
	BIN2 = 0;
}

void right()
{
	AIN1 = 0;
	AIN2 = 1;
	BIN1 = 0;
	BIN2 = 0;
}

void left()
{
	AIN1 = 0;
	AIN2 = 0;
	BIN1 = 0;
	BIN2 = 1;
}

void Timer0_Init(void)		//1毫秒@12.000MHz
{
	AUXR |= 0x80;			//定时器时钟1T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0x20;				//设置定时初始值
	TH0 = 0xD1;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
	ET0 = 1;				//使能定时器0中断
}


void main()
{
	uchar TestStr[] = {"Hello Car!"};
	uchar str[] =     {"Speed - 10%"};
	
	P4SW = 0xFB;
	P1M0 = 0x01;
	P1M1 = 0x00;
	P0M0 = 0x00;
	P0M1 = 0x00;

	Init_LCD1602();//1602初始化
	LCD1602_Dis_Str(0, 0, &TestStr);	//显示字符串
	LCD1602_Dis_Str(0, 1, &str);	//显示字符串
	
	Timer0_Init();
		
	EA=1;//开中断
			
	while(1)
	{
			if( OUT2 == 1 && OUT4 == 1 )//2和4都为高，说明线在2和4之间，前进就行
				run();
			else
			{
				if( OUT2 == 0 && OUT4 == 1 )//黑色部分不亮灯，低电平，2撞线，该左转
				{
					left();
				}
				
				if( OUT2 == 1 && OUT4 == 0 )//黑色部分不亮灯，低电平，4撞线，该右转
				{
					right();
				}
			}
	}
}
void Timer0_Isr(void) interrupt 1
{	
	if( num_i <= num_door )//0-10时，开电机
	{
		num_i++;
		
		STBY = 1;
		PWMA = 1;
		PWMB = 1;
	}
	if( (num_door < num_i) && (num_i < num_flag) )//10-100时，关电机，那么电机占空比就是10%了
	{
		num_i++;
		
		STBY = 0;
		PWMA = 0;
		PWMB = 0;
	}
	if( num_i >= num_flag )
	{
		num_i=0;
	}
}