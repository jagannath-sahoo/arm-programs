#include "LPC214x.h"

/*
(External Crystal) --> PLL --> CCLK --> APBDIV() --> PCLK
																|					|						|->30MHz
																|					|
																|					|->APBDIV register (APBDIV)
																|					|->To Divide Core 10(VPBDIV Reg) => 1/2 of CCLK
																|
																|->By Default 60MHz from Startup File
																|-> 60MHz from Startup file
*/

/*
	Initialize UART
	param: None
	return: void
*/
void initUART();
/*
Send Charactor on UART1
param: data
return: void
*/
void sendChar(unsigned char data);

/*
Recevice Charactor on UART1
param: None
return: unsigned char
*/
unsigned char getChar();

/*
Send String
param:	str: String to Send
return: void
*/
void sendString(unsigned char *str);

int main()
{
	//Initailize UART1
	initUART();
	VPBDIV = 0x02; //Divide Clock by 2
	char *ptr = "Hello World\r\n";
	sendString(ptr);
	while(1)
	{
		sendChar(getChar());
	}
	return 0;
}

void initUART()
{	
	/*
	Pin function Select register 0
	P0.8 Bit(17:16)= TXD UART1
	P0.9 Bit(19:18)= RXD UART1
	*/
	//PINSEL0 = 0x50000;
	PINSEL0 |= (01<<16) | (01<<18);
	/*
	UART1 Line Control Register
	Bit(1:0) 8 Bit Length = 11
	Bit(2)   1 Stop bit = 0
	Bit(3)   Disable Parity = 0
	Bit(7)   Enable access Divisor Latches = 1
	*/
	//U1LCR = 0x83;
	U1LCR |= (11<<0) | (1<<7);
	/*
	UART1 Divisor Latch Registers 0
	Peripheral Clock PCLK = 30MHz(or 60MHz Doubt)
	Desired_BaudRate = 9600
	U1DLL = PCLK in Hertz/(16 x Desired_BaudRate) = 195
	*/
	U1DLL = 0x000000C3;
	/*
	Bit(7)   = Disable access Divisor Latches
	*/
	//U1LCR =0x00000003;
	U1LCR &= ~(1<<7);	
}

void sendChar(unsigned char data)
{
	/*
	UART1 Line Status Register(U1LSR)
	Transmitte Holding Register Empty(THRE) Bit 5
	0: U1THR contains data
	1: U1THR Empty
	*/
	while(!(U1LSR & (1<<5)));//Check for Empty U1THR
	//UART1 Transmitter Holding Register (U1THR)
	U1THR = data;
}

unsigned char getChar()
{
	/*
	UART1 Line Status Register(U1LSR)
	Receiver Data Ready(RDR) Bit 0
	0: U1RBR is empty
	1: U1RBR contains data
	*/
	while(!(U1LSR & (1<<0)));//Check for Empty U1THR
	//UART1 Transmitter Holding Register (U1THR)
	return (U1RBR);
}

void sendString(unsigned char *str)
{
	while(*str)//Check Null 
	{
		sendChar(*str++);
	}
}
	