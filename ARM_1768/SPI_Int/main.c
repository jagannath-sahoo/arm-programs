#include "LPC17xx.h"

/*************FROM USER MANNUAL*******************
1. Set the SPI Clock Counter Register to the desired clock rate.
2. Set the SPI Control Register to the desired settings for master mode.

1. Optionally, verify the SPI setup before starting the transfer.
2. Write the data to transmitted to the SPI Data Register. This write starts the SPI data
transfer.
3. Wait for the SPIF bit in the SPI Status Register to be set to 1. The SPIF bit will be set
after the last cycle of the SPI data transfer.
4. Read the SPI Status Register.
5. Read the received data from the SPI Data Register (optional).
6. Go to step 2 if more data is to be transmitted.
**************************************************/


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
Initialize SPI
*/
void initSPI(void);

void sendData(char data);

void delay(unsigned long time)
{
 	int i, j;
	for(i=0;i<1000;i++)
		for(j=0;j<time;j++);
}

//IRQ variable
volatile int val = 0;

int main()
{
	initSPI();
	NVIC_EnableIRQ(SPI_IRQn);//Enable Spi interrupt in NVIC
	while(1)
	{
		LPC_GPIO0->FIOSET|=(1<<16);
		LPC_SPI->SPDR= val++;
		LPC_GPIO0->FIOCLR|=(1<<16);
		if(val>=7)
		{
			val = 0;
		}
		/*
		for(uint8_t i = 0; i <= 8; ++i)
		{
			sendData(1<<i);
			delay(2000);
		}
		sendData(0x00);
		*/
		/*
		sendData(0xf0);
		for(int i=0;i<10000;i++)
			for(int j=0;j<50;j++);
		
		sendData(0x0f);
		for(int i=0;i<10000;i++)
			for(int j=0;j<50;j++);
		*/
	}
	return 0;
}

void initSPI(void)
{
	/*
	Power Control for Peripherals register (PCONP)
	BIT[8] PCSPI The SPI interface power/clock control bit.
	*/
	LPC_SC->PCONP |= (1<<8);
	/*
	Peripheral Clock Selection registers 0 and 1 (PCLKSEL0)
	BIT[17:16] PCLK_SPI Peripheral clock selection for SPI.
	*/
	/*
	Pin Function Select register 0 (PINSEL0)
	31:30 P0.15	11 SCK
	Pin Function Select register 1 (PINSEL1)
	1:0 	P0.16	10 SSEL0(Avoid)
	3:2 	P0.17 11 MISO0
	5:4   P0.18	11 MOSI0
	*/
	LPC_PINCON->PINSEL0 |= (1<<31) | (1<<30);
	LPC_PINCON->PINSEL1 |= (1<<2) | (1<<3) | (1<<5) | (1<<4);
	
	LPC_GPIO0->FIODIR2 |= (1<<0);//SSEL As Output
	/*
	SPI Control Register (S0SPCR)
	BIT[5]	Set for Master Operations
	BIT[6]  Set for SPI data is transferred LSB (bit 0) first
	BIT[7]	Set for SPI interrupt
	*/
	LPC_SPI->SPCR = 0x00;
	LPC_SPI->SPCR |= (1<<5) | (1<<6) | (1<<7);
	/*
	SPI Clock Counter Register (S0SPCCR)
	BIT[7:0] For SPI Counter Setting Setting
	PClk = 25MHz
	SPI Clk = 2.5MHz
	So Val = PClk / SPICLK
	*/
	LPC_SPI->SPCCR = 0x10;
}

void sendData(char data)
{
	//P0.16 as HIGH by FIO2SET
	LPC_GPIO0->FIOSET2 |= (1<<0);
	LPC_SPI->SPDR = data;
	/*
	SPI Status Register (S0SPSR)
	BIT[7] SPIF SPI transfer complete flag
	*/
	while(!(LPC_SPI->SPSR & (1<<7)));//Trap
	//P0.16 as HIGH by FIO2SET
	LPC_GPIO0->FIOCLR2 |= (1<<0);
}

void SPI_IRQHandler()
{
	LPC_SPI->SPINT |= (1<<0);
	val = LPC_SPI->SPDR;
}