#include "LPC17xx.h"
#include <stdio.h>
#include <string.h>
/***********************************************************************************************************************
Externel Clk ==> System Clk (CCLK)==> PCLK_ADC	==> ADC CLKDIV(Clk/1)
12MHz							|-> 100MHz					|-> 25MHz					
************************************************************************************************************************/

/********FROM USER MANUAL*********
The ADC is configured using the following registers:
1. Power: In the PCONP register (Table 46), set the PCADC bit.
Remark: On reset, the ADC is disabled. To enable the ADC, first set the PCADC bit,
and then enable the ADC in the AD0CR register (bit PDN Table 532). To disable the
ADC, first clear the PDN bit, and then clear the PCADC bit.
2. Clock: In the PCLKSEL0 register (Table 40), select PCLK_ADC. To scale the clock for
the ADC, see bits CLKDIV in Table 532.
3. Pins: Enable ADC0 pins through PINSEL registers. Select the pin modes for the port
pins with ADC0 functions through the PINMODE registers (Section 8.5).
4. Interrupts: To enable interrupts in the ADC, see Table 536. Interrupts are enabled in
the NVIC using the appropriate Interrupt Set Enable register. Disable the ADC
interrupt in the NVIC using the appropriate Interrupt Set Enable register.
5. DMA: See Section 29.6.4. For GPDMA system connections, see Table 544.
**********************************/


/****************************UART******************************************/
/*========================================================================*/
/*
(External Clock) --> PLL CLOCK --> Clock Div --> PCLOCK 	--> PCLCK UART()
											|							|-> CLK/4			|-> CLK/4			|-> 25MHz
											|							|->100MHz
											|
											|-> 400MHz 

PLL0 is configured for Core Clock
PLL0CON 	BIT[14:0]		MSEL(M) For cal M = M + 1
					BIT[23:16]	Pre-Div(N)	For Cal N = N + 1

Fcco = (2 * M * Fin)/N			
"From startup file"
M = 0x63 => 99 in DEC so M = 99 + 1 = 100
N = 0x05 => 5 in DEC so N = 5 + 1 = 6
Fin = external clock = 12MHz
Fcco = 400MHz

CPU Clock Configuration register (CCLKCFG)
BIT[7:0] 		CCLKSEL 	Selects the divide value for creating the CPU clock (CCLK)
											from the PLL0 output.
		3 								pllclk is divided by 4 to produce the CPU clock

*/

/*
Initialize UART
param: void
return void
*/
void uartInit3(void);

/*
Send Char
param: char type data
return: void
*/
void sendChar3(char data);

/*
Receive Char
param: void
return: char type data
*/
char getChar3(void);

/*
Send String
param:	str: String to Send
return: void
*/
void sendString(char *str);
/********************************************************/

/*
ADC initialize
*/
void initADC(void);

/*
Get adc results
param: void
return : int result of adc
*/
int getAdcResult(void);

volatile int adcIntBuf = 0;

int main()
{
	int adc = 0;
	initADC();
	uartInit3();
	sendString("ADC\r\n");
	char buff[32] = { 0 };
	while(1)
	{
		adc =  adcIntBuf;
		sprintf(buff,"ADC Result: %d\r\n",adc);
		sendString(buff);
		memset(buff,0,32);
		for(long int i = 0; i < 600000; ++i);
	}
}

void initADC()
{
	/*
	Power Control for Peripherals register (PCONP)
	BIT 12 PCADC A/D converter (ADC) power/clock control bit.
	*/
	LPC_SC->PCONP |= (1<<12);
	
	/*
	Configure PIN as ADC
	Pin Function Select Register 1 (PINSEL1)
	19:18 P0.25 GPIO Port 0.25 AD0.2	01
	*/
	LPC_PINCON->PINSEL1 |= (1<<18);
	
	//RESET ADC CONTROL REGISTER
	LPC_ADC->ADCR = 0x00;

	/*
	A/D Control Register
	BIT 15:8 CLKDIV The APB clock (PCLK_ADC0) is divided by (this value plus one) to produce the clock for
									the A/D converter

	*/
	LPC_ADC->ADCR |= (0x01<<8);
	
	
	
	/*
	A/D Control Register
	BIT 16 BURST 1 The AD converter does repeated conversions
			21 PDN 1 The A/D converter is operational.
			7:0 SEL Selects which of the AD0.2 for BIT 2
	*/
	LPC_ADC->ADCR |= (1<<16) | (1<<21) | (1<<2);
	/*
	A/D Control Register
	26:24 START When the BURST bit is 0, these bits control whether and when an A/D conversion is
				started:
	*/
	//Stop ADC connvertion
	LPC_ADC->ADCR |= (0x000<<24);
	
	/****INTERRUPT****/
	/*
	A/D Interrupt Enable register (AD0INTEN)
	BIT[2] 		ADINTEN2 		Completion of a conversion on ADC channel 2 will generate an interrupt
	*/
	LPC_ADC->ADINTEN |= (1<<2);
	NVIC_EnableIRQ(ADC_IRQn);
	/*****************/
}

int getAdcResult()
{
	int adcRes = 0;
	//Start Conversion
	//001 Start conversion now.
	LPC_ADC->ADCR |= (1<<24);
	/*
	A/D Status register (ADSTAT)
	BIT 2 DONE2 This bit mirrors the DONE status flag from the result register for A/D channel 2.
	*/
	while(!(LPC_ADC->ADSTAT & (1<<2)));//Trap
	/*
	A/D Data Registers (AD0DR2)
	BIT 15:4 RESULT When DONE is 1, this field contains a binary fraction representing the voltage on the AD0[n]
	*/
	adcRes = ((LPC_ADC->ADDR2) >> 4) & (0xfff);
	return adcRes;
}

void ADC_IRQHandler()
{
	/*
	A/D Status register (ADSTAT)
	BIT 16 		ADINT This bit is the A/D interrupt flag
	*/
	adcIntBuf = ((LPC_ADC->ADDR2) >> 4) & (0xfff);
	//Stop ADC connvertion
	//LPC_ADC->ADCR |= (0x000<<24);
	
}
/*****************************UART**************************/
/*=========================================================*/
void uartInit3()
{
	/*
	Power Control for Peripherals register (PCONP)
	BIT[25]		PCUART3 	UART 3 power/clock control bit
	*/
	LPC_SC->PCONP |= (1<<25);
	/*
	Pin function select register 0 (PINSEL0)
	BIT[1:0]		P0.0		10 			TXD3
	BIT[3:2] 		P0.1 		10			RXD3
	*/
	LPC_PINCON->PINSEL0 |= (1<<3)|(1<<1);
	//LPC_PINCON->PINSEL0 |= (0x10<<0) | (0x10<<2);
	/*
	Pin Mode select register 0 (PINMODE0)
	BIT[1:0]	P0.0		10		pin has neither pull-up nor pull-down
	BIT[3:2] 	P0.1		10		pin has neither pull-up nor pull-down
	(Optional)
	*/
	LPC_PINCON->PINMODE0 |= (0x10<<0) | (0x10<<2);
	
	/*
	Peripheral Clock Selection register 1 (PCLKSEL1)
	BIT[19:18] 	PCLK_UART3 		Peripheral clock selection for UART3
														00 PCLK_peripheral = CCLK/4 
	*/
	LPC_SC->PCLKSEL1 &= ~(0x11<<18);
	/*
	UARTn Line Control Register (U0LCR)
	BIT[1:0]		Word Length Select		11 For 8-bit char length
	BIT[2]			Stop Bit Select				0  For 1 Stop Bit
	BIT[3] 			Parity Enable 				0  Disable parity generation and checking.
	BIT[7]		  Divisor Latch					1	 Enable access to Divisor Latches.
							Access Bit (DLAB)
	*/
	LPC_UART3->LCR |= /*(0x11<<0) */  (1<<1)|(1<<0) | (1<<7);
	//LPC_UART3->LCR |= (0x11<<0) | (1<<7);
	/*
	BR = PCLK / (16 * 9600) = 162.72 in Hex 0xA2
	*/
	LPC_UART3->DLL = 0xA2;
	LPC_UART3->LCR &= ~(1<<7);//Clear Latch
	/*
	BIT[7]		U3TER			Enable Tramission of Tx 
	*/
	LPC_UART3->TER|=(1<<7);
}

void sendChar3(char data)
{
	/*
	UARTn Line Status Register (U2LSR)
	BIT[5]		Transmitter Holding Register Empty (THRE))
						THRE is set immediately upon detection of an empty UARTn THR and is cleared	
						on a UnTHR write.
						0 UnTHR contains valid data.
						1 UnTHR is empty.
	*/
	while(!(LPC_UART3->LSR & (1<<5)));//Trap
	//THR = Transmission Hold Register
	LPC_UART3->THR = data;
}

char getChar3(void)
{
	/*
	UARTn Line Status Register (U2LSR)
	BIT[0] Receiver Data Ready (RDR)
								UnLSR0 is set when the UnRBR holds an unread character and is cleared when
								the UARTn RBR FIFO is empty.
								0 The UARTn receiver FIFO is empty.
								1 The UARTn receiver FIFO is not empty.
	*/
	while(!(LPC_UART3->LSR & (1<<0)));//TRAP
	
	//RBR = Receiver Buffer Register
	return(LPC_UART3->RBR);
}

void sendString(char *str)
{
	while(*str)
	{
		sendChar3(*str++);
	}
}
/*==========================================================*/

