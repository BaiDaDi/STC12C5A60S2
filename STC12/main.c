#include <STC12C5A60S2.H>
/*=================================================
*�Զ�����������
=================================================*/
typedef unsigned char uchar;
typedef unsigned int uint;

#define LCD1602_DB P0	//LCD1602��������

sbit LED= P1^7;

//����LCD1602
sbit LCD1602_RS = P4^4;	 //RS��
sbit LCD1602_RW = P4^5;	 //RW��
sbit LCD1602_EN = P4^6;	 //EN��

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

//����PWM��־λ
uchar num_i=0;
uchar num_door=10;
uchar num_flag=100;

/*=================================================
*�������ƣ�Read_Busy
*�������ܣ��ж�1602Һ��æ�����ȴ�
=================================================*/
void Read_Busy()
{
	uchar busy;
	LCD1602_DB = 0xff;//��λ��������
	LCD1602_RS = 0;	  //����RS
	LCD1602_RW = 1;	  //����RW��
	do
	{
		LCD1602_EN = 1;//ʹ��EN
		busy = LCD1602_DB;//��������
		LCD1602_EN = 0;	 //����ʹ���Ա�����һ�β���������
	}while(busy & 0x80); //�ж�״̬��BIT7λ�Ƿ�Ϊ1��Ϊ1���ʾæ������ȴ�
}
/*=================================================
*�������ƣ�LCD1602_Write_Cmd
*�������ܣ�дLCD1602����
*���ã�Read_Busy();
*���룺cmd:Ҫд������
=================================================*/
void LCD1602_Write_Cmd(uchar cmd)
{
	Read_Busy();	 //�ж�æ��æ��ȴ�
	LCD1602_RS = 0;
	LCD1602_RW = 0;	//����RS��RW����ʱ�����1602�μ�������ʹ��˵����������ʱ���½�
	LCD1602_DB = cmd;//д������
	LCD1602_EN = 1;	 //����ʹ�ܶ� ���ݱ����䵽LCD1602��
	LCD1602_EN = 0;	 //����ʹ���Ա�����һ�β���������
}
/*=================================================
*�������ƣ�LCD1602_Write_Dat
*�������ܣ�дLCD1602����
*���ã�Read_Busy();
*���룺dat����Ҫд�������
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
*�������ƣ�LCD1602_Dis_OneChar
*�������ܣ���ָ��λ����ʾһ���ַ�
*���ã�LCD1602_Write_Cmd(); LCD1602_Write_Dat();	
*���룺x��Ҫ��ʾ�ĺ�����ȡֵ0-40��y��Ҫ��ʾ��������ȡֵ0-1��0Ϊ��һ��,1Ϊ�ڶ��У�
		dat����Ҫ��ʾ��������ASCLL��ʽ��ʾ
=================================================*/
void LCD1602_Dis_OneChar(uchar x, uchar y,uchar dat)
{
	if(y)	x |= 0x40;
	x |= 0x80;
	LCD1602_Write_Cmd(x);
	LCD1602_Write_Dat(dat);		
}
/*=================================================
*�������ƣ�LCD1602_Dis_Str
*�������ܣ���ָ��λ����ʾ�ַ���
*���ã�LCD1602_Write_Cmd(); LCD1602_Write_Dat();
*���룺x��Ҫ��ʾ�ĺ�����ȡֵ0-40��y��Ҫ��ʾ��������ȡֵ0-1��0Ϊ��һ��,1Ϊ�ڶ��У�
		*str����Ҫ��ʾ���ַ���
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
*�������ƣ�Init_LCD1602
*�������ܣ�1602��ʼ��
*���ã�	LCD1602_Write_Cmd();
=================================================*/
void Init_LCD1602()
{
	LCD1602_Write_Cmd(0x38); //	����16*2��ʾ��5*7����8λ���ݽӿ�
	LCD1602_Write_Cmd(0x0c); //����ʾ
	LCD1602_Write_Cmd(0x06); //��дһ�ֽں��ַָ���1
	LCD1602_Write_Cmd(0x01); //�����ʾ
}

void run()//������Կ�TB6612���������ݱ�
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

void Timer0_Init(void)		//1����@12.000MHz
{
	AUXR |= 0x80;			//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;			//���ö�ʱ��ģʽ
	TL0 = 0x20;				//���ö�ʱ��ʼֵ
	TH0 = 0xD1;				//���ö�ʱ��ʼֵ
	TF0 = 0;				//���TF0��־
	TR0 = 1;				//��ʱ��0��ʼ��ʱ
	ET0 = 1;				//ʹ�ܶ�ʱ��0�ж�
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

	Init_LCD1602();//1602��ʼ��
	LCD1602_Dis_Str(0, 0, &TestStr);	//��ʾ�ַ���
	LCD1602_Dis_Str(0, 1, &str);	//��ʾ�ַ���
	
	Timer0_Init();
		
	EA=1;//���ж�
			
	while(1)
	{
			if( OUT2 == 1 && OUT4 == 1 )//2��4��Ϊ�ߣ�˵������2��4֮�䣬ǰ������
				run();
			else
			{
				if( OUT2 == 0 && OUT4 == 1 )//��ɫ���ֲ����ƣ��͵�ƽ��2ײ�ߣ�����ת
				{
					left();
				}
				
				if( OUT2 == 1 && OUT4 == 0 )//��ɫ���ֲ����ƣ��͵�ƽ��4ײ�ߣ�����ת
				{
					right();
				}
			}
	}
}
void Timer0_Isr(void) interrupt 1
{	
	if( num_i <= num_door )//0-10ʱ�������
	{
		num_i++;
		
		STBY = 1;
		PWMA = 1;
		PWMB = 1;
	}
	if( (num_door < num_i) && (num_i < num_flag) )//10-100ʱ���ص������ô���ռ�ձȾ���10%��
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