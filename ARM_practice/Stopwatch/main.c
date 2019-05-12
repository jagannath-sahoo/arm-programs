#include "LPC17xx.h"
#include <string.h>
#include <stdio.h>

/********TIMER*********/
//Initialize timer
void initTIMER(void);
volatile int millis = 0;

/**********************/
int ss = 0;
int mm = 0;
int hh = 0;
char buf[64] = { 0 };


/*********UART*********/
// Initialize uart
void initUART(void);
//Send Char type data
void sendChar(char data);
//Receive char
char receiveChar(void);
//Send string
void sendString(char *str);

char* receiveString(char* strBuf, int len);
/***********************/

int main()
{
	initUART();
	
	//sendChar('A');
	sendString("Hello UART\r\n");
	//char buf[64] = { 0 };
	
	/*GPIO*/
	LPC_GPIO2->FIODIR |= (1<<9);
	LPC_GPIO2->FIOSET |= (1<<9);
	/******/
	initTIMER();
	while(1)
	{
			if(millis >= 1000)
			{
				ss++;
				millis = 0;
			}
			if(ss>= 60)
			{
				mm++;
				ss = 0;
			}
			if(mm>= 60)
			{
				hh++;
				mm = 0;
			}
		sprintf(buf,"%d:%d:%d:%d\r\n",hh,mm,ss,millis);
		sendString(buf); 
		memset(buf,0,64);
		//sprintf(buf,"%d:%d:%d:%d\r\n",hh,mm,ss,millis);
		//sendString(buf); 
		//memset(buf,0,64);
		//LPC_TIM0->TCR |= (1<<0);//Enable Timer 
		/*
		receiveString(buf,10);
		sendString(buf);
		if(strcmp("cdac",buf) == 0)
		{
			sendString("Matched");
			LPC_GPIO2->FIOCLR |= (1<<9);
		}
		else
		{
			LPC_GPIO2->FIOSET |= (1<<9);
		}
		memset(buf,0,32);
		*/
		
		
	}
}

/****************UART*************/
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

void sendString(char *str)
{
	while(*str != 0)
	{
		sendChar(*str++);
	}
}

char receiveChar(void)
{
	//U3LSR: Wgeb set the U3RBR contains some data
	while(!(LPC_UART3->LSR & (1<<0)));//Trap
	return(LPC_UART3->RBR);
}

char* receiveString(char* strBuf, int len)
{
	char temp = 0;
	while(((temp = receiveChar()) != 0x0D) && (len>0))
	{
		sendChar('*');
		*strBuf++ = temp;
		len--;
	}
	*strBuf = 0;
	return strBuf;
}
/*********************************/

/******TIMER***********/
void initTIMER(void)
{
	//Power On the TIMER
	// By default TIM0 is Activated
	//Configure Timer Peripheal clock
	//By Default its set to 25MHz
	//By default its in Timer Mode
	LPC_TIM0->PC = 0x00;
	LPC_TIM0->TC = 0x00;
	
	LPC_TIM0->MR0 = 1;
	LPC_TIM0->PR = 24999;
	/*
	Timer Control Register
	BIT 1		TC will reset on match to MR0
	BIT 0		Interupt on MR0 Match with TC
	*/
	LPC_TIM0->MCR |= (1<<1) | (1<<0);
	
	LPC_TIM0->TCR = 0x00;
	LPC_TIM0->TCR |= (1<<1);//Reset TIMER0
	LPC_TIM0->TCR &= ~(1<<1);//Clear Reset TIMER0 Bit
	LPC_TIM0->TCR |= (1<<0);//Enable Timer 
	NVIC_EnableIRQ(TIMER0_IRQn);
}

void TIMER0_IRQHandler()
{
	
	if(LPC_TIM0->IR & (1<<0))
	{
		//LPC_TIM0->IR |= (1<<0);
		millis++;
	}
}
/**********************/
