#include "LPC214x.h"
#include <inttypes.h>
/*
(External Crystal) --> PLL --> CCLK --> APBDIV() --> PCLK
																|					|						|->15MHz
																|					|
																|					|->APBDIV register (APBDIV)
																|					|->To Divide Core CLK By default 00 so 1/4 of CCLK
																|
																|->By Default 60MHz from Startup File
																|-> 60MHz from Startup file
*/
/* ------From Datasheet------
Master operation
The following sequence describes how one should process a data transfer with the SPI
block when it is set up to be the master. This process assumes that any prior data transfer
has already completed.
1. Set the SPI clock counter register to the desired clock rate.
2. Set the SPI control register to the desired settings.
3. Write the data to transmitted to the SPI data register. This write starts the SPI data
transfer.
4. Wait for the SPIF bit in the SPI status register to be set to 1. The SPIF bit will be set
after the last cycle of the SPI data transfer.
5. Read the SPI status register.
6. Read the received data from the SPI data register (optional).
7. Go to step 3 if more data is required to transmit.
*/

/*
Initialize SPI0
*/
void initSPI0();
/*
Send Char 
param: data
return: void
*/
void sendData(char data);
/*
Random delay Loop
param: time
return: void
*/
void delay(unsigned long time);

int main()
{
	initSPI0();
	while(1)
	{
		for(uint8_t i = 0; i <= 8; ++i)
		{
			sendData(1<<i);
			delay(2000);
		}
		sendData(0x00);
		/*sendData(0x0F);
		delay(2000);
		sendData(0xF0);
		delay(2000);*/
	}
	return 0;
}

void initSPI0()
{
	/*
	PIN SELECT REGISTER
	P0.4 9:8 	 	SCK0  = 01
	P0.5 11:10 	MISO0 = 01
	P0.6 13:12	MOSI0	= 01
	*/
	PINSEL0 |= (01<<8) | (01<<10) | (01<<12);
	//PINSEL0 |= (1<<8) | (1<<10) | (1<<12);
	/*
	P0.7	SSEL0 as OUTPUT 
	*/
	IO0DIR |= (1<<7);
	IO0SET |= (1<<7);//Set High 
	/*
	SPI Control Register
	MSTR	Bit(5)	Set as Master
	LSBF  Bit(6)  Set as SPI transfer LSB first
	*/
	//S0SPCR=0x00;
	S0SPCR |= (1<<5) | (1<<6);
	/*
	SPI Clock Counter Register
	SPI Clock = 1MHz
	PCLK = 15MHz
	value = PCLK / SPCCR0 = 15(Doubt)
	*/
	S0SPCCR = 0x0f;
}

void sendData(char data)
{
	IO0SET |= (1<<7);//Set SSEL0 as HIGH
	/*
	SPI Data Register (S0SPDR)
	*/
	S0SPDR = data;//Send data
	/*
	SPI Status Register (S0SPSR)
	SPIF Bit(7) SPI transfer complete flag
	1: Data transmit complete
	0: Data transmit is going on
	*/
	while(!(S0SPSR & (1<<7)));
	IO0CLR |=(1<<7);//SSEL0 as Low
}

void delay(unsigned long time)
{
 	int i, j;
	for(i=0;i<1000;i++)
		for(j=0;j<time;j++);
}