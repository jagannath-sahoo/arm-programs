#include "LPC17xx.h"
/*******************FROM DATASHEET***************************/
/*
The UART0/2/3 peripherals are configured using the following registers:
1. Power: In the PCONP register (Table 46), set bits PCUART0/2/3.
Remark: On reset, UART0 is enabled (PCUART0 = 1), and UART2/3 are disabled
(PCUART2/3 = 0).
2. Peripheral clock: In the PCLKSEL0 register (Table 40), select PCLK_UART0; in the
PCLKSEL1 register (Table 41), select PCLK_UART2/3.
3. Baud rate: In register U0/2/3LCR (Table 280), set bit DLAB =1. This enables access
to registers DLL (Table 274) and DLM (Table 275) for setting the baud rate. Also, if
needed, set the fractional baud rate in the fractional divider register (Table 286).
4. UART FIFO: Use bit FIFO enable (bit 0) in register U0/2/3FCR (Table 279) to enable
FIFO.
5. Pins: Select UART pins through the PINSEL registers and pin modes through the
PINMODE registers (Section 8.5).
Remark: UART receive pins should not have pull-down resistors enabled.
6. Interrupts: To enable UART interrupts set bit DLAB =0 in register U0/2/3LCR
(Table 280). This enables access to U0/2/3IER (Table 276). Interrupts are enabled in
the NVIC using the appropriate Interrupt Set Enable register.
7. DMA: UART0/2/3 transmit and receive functions can operate with the GPDMA
controller (see Table 544).
*/
/************************************************************/

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

int main()
{
	uartInit3();
	sendString("Hello UART3\r\n");
	while(1)
	{
		sendChar3(getChar3());
	}
	return 0;
}

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

