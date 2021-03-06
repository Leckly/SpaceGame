/******************************************************************
 *****                                                        *****
 *****  Name: display.c                                       *****
 *****  modified version of Andreas Dannenberg                *****
 *****  Includes only display specific code.		      *****                                                        *****
 ******************************************************************/


#include <msp430x14x.h>
#include "lcd.h"
#include "portyLcd.h"



#define bitset(var,bitno) ((var) |= 1 << (bitno))
#define bitclr(var,bitno) ((var) &= ~(1 << (bitno)))



void Delay (unsigned int a);
void Delayx100us(unsigned char b);
void _E(void);


void putc(char c) {
    SEND_CHAR(c);
}
void clearDisplay() {
    SEND_CMD(CLR_DISP);
    Delayx100us(10);
}
void gotoSecondLine() {
    SEND_CMD(DD_RAM_ADDR2);
}
void printString(char *String) {
  while(*String)
    putc(*String++);
}
char HexDigit(int digitvalue) {
  if (digitvalue < 10)
    return(digitvalue + '0');
  else
    return(digitvalue + 'A' - 10);
}

void printHex(unsigned int Number) {
  char HexBuffer[5];
  unsigned char counter=0,i;
  
  while(Number !=0)
  {  HexBuffer[counter]=HexDigit(Number & 0x000f);
     Number = Number >> 4;
     ++counter;
  }
     HexBuffer[counter]='0'; 
     ++counter;
  for(i=0;i<counter;i++)
   {
    putc(HexBuffer[counter -i-1]);
   }
}
void printDecDigit(int Number){
 if (Number <0)
 {
   putc('-');
   putc(Number + '0');
 }
 else 
  putc(Number + '0'); 
}
void printDecimal(int Number) {

  char DecimalBuffer[7];
  unsigned char counter=0,i;

  if (Number < 0) {
     Number = -Number;
    putc('-');
  } 
  while(Number !=0)
  {
  DecimalBuffer[counter]=(Number %10)+'0';
  Number=Number /10;
  ++counter;
  }
  
  for(i=0;i<counter;i++)
   {
    putc(DecimalBuffer[counter -i-1]);
   }
}



void Delay (unsigned int a)
{
  int k;
  for (k=0 ; k != a; ++k) {
    _NOP();
    _NOP();
    _NOP();
    _NOP();
  }
}

void Delayx100us(unsigned char b)
{
  int j;
  for (j=0; j!=b; ++j) Delay (_100us);
}


void _E(void)
{
        bitset(P2OUT,E);		//toggle E for LCD
	Delay(_10us);
	bitclr(P2OUT,E);
}


void SEND_CHAR (unsigned char d)
{
        int temp;
	Delayx100us(5);                 //.5ms	
	temp = d & 0xf0;		//get upper nibble	
	LCD_Data &= 0x0f;
	LCD_Data |= temp;
	bitset(P2OUT,RS);     	        //set LCD to data mode
	_E();                           //toggle E for LCD
	temp = d & 0x0f;
	temp = temp << 4;               //get down nibble
	LCD_Data &= 0x0f;
	LCD_Data |= temp;
	bitset(P2OUT,RS);   	        //set LCD to data mode
	_E();                           //toggle E for LCD
}

void SEND_CMD (unsigned char e)
{
        int temp;
	Delayx100us(10);                //10ms
	temp = e & 0xf0;		//get upper nibble	
	LCD_Data &= 0x0f;
	LCD_Data |= temp;               //send CMD to LCD
	bitclr(P2OUT,RS);     	        //set LCD to CMD mode
	_E();                           //toggle E for LCD
	temp = e & 0x0f;
	temp = temp << 4;               //get down nibble
	LCD_Data &= 0x0f;
	LCD_Data |= temp;
	bitclr(P2OUT,RS);   	        //set LCD to CMD mode
	_E();                           //toggle E for LCD
}

void InitLCD(void)
{
    bitclr(P2OUT,RS);
    Delayx100us(250);                   //Delay 100ms
    Delayx100us(250);
    Delayx100us(250);
    Delayx100us(250);
    LCD_Data |= BIT4 | BIT5;            //D7-D4 = 0011
    LCD_Data &= ~BIT6 & ~BIT7;
    _E();                               //toggle E for LCD
    Delayx100us(100);                   //10ms
    _E();                               //toggle E for LCD
    Delayx100us(100);                   //10ms
    _E();                               //toggle E for LCD
    Delayx100us(100);                   //10ms
    LCD_Data &= ~BIT4;
    _E();                               //toggle E for LCD

    SEND_CMD(DISP_ON);
    SEND_CMD(CLR_DISP);
    Delayx100us(250);
    Delayx100us(250);
    Delayx100us(250);
    Delayx100us(250);
}




