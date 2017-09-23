#include<P18F4550.h>
#include"uart1.c"

#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config FOSC=HS


#define rs PORTEbits.RE0 
#define rw PORTEbits.RE1
#define en PORTEbits.RE2
#define tur PORTBbits.RB2
#define led PORTBbits.RB5
#define data PORTD

unsigned char msg21[]="AT+CMGF=1\r";
unsigned char msg31[]="AT+CFUN=1\r";
char u4[]= "temperature out of range";
char u5[]="pH out of range";

char u6[]="turbidity out of range";

void delay_ms(unsigned int time)
{unsigned int i,j;
for(i=0; i<time; i++)
for(j=0; j<274; j++);}

void LCD_cmd(unsigned char cmd)
{data=cmd;
rs=0;
rw=0;
en=1;
delay_ms(50);
en=0;}

void LCD_write(unsigned char DATA)
{data=DATA;
rs=1;
rw=0;
en=1;
delay_ms(50);
en=0;}

void LCD_init(void)
{LCD_cmd(0x38);
LCD_cmd(0x01);
LCD_cmd(0x80);
LCD_cmd(0x0F);}

void write_string(static char*str)
{int i=0;
while(str[i]!=0)
	{LCD_write(str[i]);
	delay_ms(40);
	i++;}
}
//counting frequency
void delayMs(char ch)
{
int i;
T0CON = 0x83;
for (i = 0; i < ch; i++) 
{
TMR0H = 0x0B;
TMR0L=0xDC;

INTCONbits.TMR0IF = 0;
while(!(INTCONbits.TMR0IF)); 
}
}

unsigned int i=0, tovf,id3,id2,id1,id0;
unsigned int freq,R,G,B,R1,G1,B1,ch;
unsigned int f1, f2, f3, f4, f5;
unsigned int f1_mod, f2_mod, f3_mod, f4_mod;

void high_ISR(void);
#pragma code high_vector = 0x08
void high_interrupt(void)
{
	_asm
	goto high_ISR
	_endasm
	}
	
#pragma code  //return to the default code section //return to the default code section
#pragma interrupt high_ISR
void high_ISR (void)
{
if(PIR1bits.TMR1IF){ 
PIR1bits.TMR1IF = 0;
tovf ++;
}
}
char temp, temp1;
unsigned char d3,d2,d1,d0;

int convert(char val)
{
int v;
if(val==0b00000000)
v=0;
if(val==0b00000001)
v=1;
if(val==0b00000010)
v=2;
if(val==0b00000011)
v=3;
if(val==0b00000100)
v=4;
if(val==0b00000101)
v=5;
if(val==0b00000110)
v=6;
if(val==0b00000111)
v=7;
if(val==0b00001000)
v=8;
if(val==0b00001001)
v=9;
if(val==0b00001010)
v=10;
if(val==0b00001011)
v=11;
if(val==0b00001100)
v=12;
if(val==0b00001101)
v=13;
if(val==0b00001110)
v=14;
if(val==0b00001111)
v=15;
return v;
}

void count()
{
T1CON=0x83;  //start counter
INTCON=0xC0; //global interrupts enable
delayMs(5);
T1CON=0x82;
INTCONbits.GIE=0;
temp=TMR1L;
temp1=TMR1H;
//PORTB=temp1;
//lcd_data(tovf);

d3=temp1 & 0xF0;  //higher 4 bits of TMRH
d3=d3>>4;
d2=temp1 & 0x0F;   //lower 4 bits of TMRH
d1=temp & 0xF0;    //higher 4 bits of TMRL
d1=d1>>4;
d0=temp & 0x0F;   //lower 4 bits of TMRL

id3=convert(d3);
id2=convert(d2);
id1=convert(d1);
id0=convert(d0);

freq=(id3*4096)+(id2*256)+(id1*16)+id0;
TMR1H=0;
TMR1L=0;


}

void Display(unsigned int ascii)
 {
f5=(ascii%10)+48;
f4_mod=(ascii/10);
f4=(f4_mod%10)+48;
f3_mod=(f4_mod/10);
f3=(f3_mod%10)+48;

f2_mod=(f3_mod/10);
f2=(f2_mod%10)+48;
f1_mod=(f2_mod/10);
f1=(f1_mod)+48;

LCD_write(f1);
LCD_write(f2);
LCD_write(f3);
LCD_write(f4);
LCD_write(f5);
}

void main()
{
char u1[]="AT+CMGS=\"";
char u2[]="7038930822";
char u3[]="\"\r";
char msg1[]="pH  Temp Turb";
char msg2[]="Water Usage";
char msg3[]="Lit/hr";
char msg4[]="";
unsigned int i;
float Q=0;
unsigned char m=0,mm=0;
unsigned int adc_result1=0,adc_result=0;
unsigned int adc_result1t=0,flagt=0,flagh=0,flagT=0;
unsigned char temp[3],temp1[3],temp1t[3];


TRISD=0X00;
TRISE=0X00;
TRISC=0X00;
TRISB=0x00;
ADCON1=0x0F;
LCD_init();

while(1)

{
//PORTB=0b00101100;
//pH

ADCON0=0b00000100;
ADCON1=0b00001100;
ADCON2=0b10001110;
ADCON0bits.ADON=1;


ADCON0bits.GO=1;
while(ADCON0bits.GO);

adc_result=ADRESL;
adc_result|= ((unsigned int)ADRESH)<<8;


//control logic for project
if(adc_result<430)
{
flagh=2; //increase
}

else if(614<adc_result)
{
flagh=2;
}

else{
flagh=1;
}

temp[0]=(unsigned char)((adc_result>>8)&0X000F);
temp[1]=(unsigned char)((adc_result>>4)&0X000F);
temp[2]=(unsigned char)(adc_result&0X000F);

write_string(msg1);

LCD_cmd(0xC0);

for(m=0;m<3;m++)
{
if(temp[m]<10)
LCD_write(temp[m]+0x30);
else
LCD_write(temp[m]+0x37);
}

delay_ms(100);


//TEMPERATURE

ADRESH=0;
ADRESL=0;
ADCON0=0b00000000;
ADCON0bits.ADON=1;
ADCON0bits.GO=1;
while(ADCON0bits.GO);

adc_result1=ADRESL;
adc_result1|= ((unsigned int)ADRESH)<<8;

if(adc_result1>90)
{
flagt=2; //cool
}

else if(adc_result1<57)
{
flagt=2; //heat
}

else 
{
flagt=3; //heat
}


temp1[0]=(unsigned char)((adc_result1>>8)&0X000F);
temp1[1]=(unsigned char)((adc_result1>>4)&0X000F);
temp1[2]=(unsigned char)(adc_result1&0X000F);

LCD_cmd(0xC4);

for(mm=0;mm<3;mm++)
{
if(temp1[mm]<10)
LCD_write(temp1[mm]+0x30);
else
LCD_write(temp1[mm]+0x37);
}


delay_ms(100);

//turbidity

ADRESH=0;
ADRESL=0;
ADCON0=0b00001000;
ADCON0bits.ADON=1;
ADCON0bits.GO=1;
while(ADCON0bits.GO);

adc_result1t=ADRESL;
adc_result1t|= ((unsigned int)ADRESH)<<8;

if(adc_result1t<511)
{
flagT=1; //Turbid
}
else
flagT=0;

temp1t[0]=(unsigned char)((adc_result1t>>8)&0X000F);
temp1t[1]=(unsigned char)((adc_result1t>>4)&0X000F);
temp1t[2]=(unsigned char)(adc_result1t&0X000F);

LCD_cmd(0xC9);

for(mm=0;mm<3;mm++)
{
if(temp1t[mm]<10)
LCD_write(temp1t[mm]+0x30);
else
LCD_write(temp1t[mm]+0x37);
}

if(flagt==2)
{
PORTC=0x04;  
}
if(flagt==3)
{
PORTC=0x02;
}
//if(flagt==1)
//{
//PORTC=0x01;
//}
//pH Flags
if(flagh==1)
{
PORTCbits.RC4=1;  //green
}

if(flagh==2)
{
PORTCbits.RC5=1;  //red
}

//Turbidity flags
if(flagT==1)
PORTBbits.RB6=1;
if(flagT==0)
PORTBbits.RB6=0;

delay_ms(100);


TRISCbits.RC0=1;

TMR1H=0;
TMR1L=0;
LCD_cmd(0x01);
LCD_cmd(0X80);
write_string(msg2);
LCD_cmd(0XC0);

PIR1bits.TMR1IF=0;
RCONbits.IPEN=1;
IPR1bits.TMR1IP=1;
PIE1bits.TMR1IE=1;
count();
Q=(freq*8); //Quantity in Litres per hour
msg4[]=(char)Q;
Display(Q);
LCD_cmd(0XC6);
write_string(msg3);
delay_ms(800);
LCD_cmd(0x01);
LCD_cmd(0X80);

UART_init();

if(flagt==2)
{
//PORTB=0b00101100;

	
UART_Write_Text(&u1);
		UART_Write_Text(&u2);
		UART_Write_Text(&u3);
	delay_ms(100);
		UART_Write_Text(&u4);
	delay_ms(100);
		UART_Write(0x1a);
	delay_ms(100);
}

if(flagh==2)
{

//PORTB=0b00101100;


	
UART_Write_Text(&u1);
		UART_Write_Text(&u2);
		UART_Write_Text(&u3);
	delay_ms(100);
		UART_Write_Text(&u5);
	delay_ms(100);
		UART_Write(0x1a);
	delay_ms(100);
}

if(flagT==1)
{

	
UART_Write_Text(&u1);
		UART_Write_Text(&u2);
		UART_Write_Text(&u3);
	delay_ms(100);
		UART_Write_Text(&u6);
	delay_ms(100);
		UART_Write(0x1a);
	delay_ms(100);
}


	
UART_Write_Text(&u1);
		UART_Write_Text(&u2);
		UART_Write_Text(&u3);
	delay_ms(100);
		UART_Write_Text(&msg4);
	delay_ms(100);
		UART_Write(0x1a);
	delay_ms(100);


}
}

