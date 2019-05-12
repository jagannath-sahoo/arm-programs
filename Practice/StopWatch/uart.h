#include "LPC17xx.h"


// Initialize uart
void initUART(void);
//Send Char type data
void sendChar(char data);
//Receive char
char receiveChar();
//Send string
void sendString(char *str);

void initUART()
{
	//No need to do any thing with the clock
	
	//Power on the uart
	LPC_SC->PCONP |= (1<<25);
	//UART configuration
	/*
	Line control register
	bit[1:0]	11 	8 Bit word length
	bit[2]		0	 	1 STOP BIT
	bit[3]		0		Parity disabled
	bit[7]		1		Latch control register
	*/	
	LPC_UART3->LCR |= (1<<0) | (1<<1) | (1<<7);
	//Setup Baud rate by Pclck/(16*9600) = 162.72 in HEx 0xA2
	LPC_UART3->DLL |= 0xA2;
	//clear latch
	LPC_UART3->LCR &= ~(1<<7);
	//Enable TX and RX
	LPC_PINCON->PINSEL0 |= (1<<1) | (1<<3);
	//Enable Transmission
	LPC_UART3->TER |=(1<<7);
}

void sendChar(char data)
{
	//Transmision Holding Register
	while(!(LPC_UART3->LSR & (1<<5)));//Trap
	LPC_UART3->THR = data;
}