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

char ch = 0;//1 byte char Buffer
void uartReceiveISR() __irq
{
	/*
	UART1 Interrupt Identification Register (U1IIR)
	BIT[3:1] Interrupt Identification
	011 1 - Receive Line Status (RLS).
	010 2a - Receive Data Available (RDA).
	110 2b - Character Time-out Indicator (CTI).
	001 3 - THRE Interrupt.
	000 4 - Modem Interrupt.
	*/
	if(U1IIR == 0x04){//RX
		ch = U1RBR;//Read the content from U1RBR register
	}
	if(U1IIR == 0x02){//TX
		U1THR = ch;
	}
	//Add Dummy value to exit from ISR
	VICVectAddr = 0x00;
}


int main()
{
	//Initailize UART1
	initUART();
	VPBDIV = 0x02; //Divide Clock by 2
	char *ptr = "Hello World\r\n";
	sendString(ptr);
	while(1)
	{
		if(ch != 0)//check if not null
		{
			sendChar(ch);
			ch = 0;//Make it NULL
		}
		//U1THR = 'Z';
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
	
	/*******INTERRUPT*******/
	
	/*
	UART1 Interrupt Enable Register
	BIT[0]	1 Enable RDA Interrupt
					0 Disable RDA Interrupt
	*/
	U1IER |= (1<<0);
	//Add the ISR Function with VICVect Address table
	VICVectAddr0=(unsigned long) uartReceiveISR;
	//VIC Vect Interrupt control
	VICVectCntl0 =	0x20 | 7;
	//Enable VIC interrupt table
	VICIntEnable |= (1<<7);
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

	