#include<LPC214x.h>

//Initialize SPI0
void spiInit();
/*
spi send data
param: data: to send
return: void
*/
void spiData(unsigned char data);

int main()
{
	spiInit();
	while(1)
	{
		
		spiData((char)0x5);
		/*//Delay using for loop
		for(int i = 0; i <= 1000000 ; ++i){}
		spiData((char)0x0);
		//Delay using for loop
		for(int i = 0; i <= 1000000 ; ++i){}
			*/
	}
	return 0;
}

void spiInit()
{
	PINSEL0 = 0x00000500;
	/*
	P0.4 = SCK0
	P0.5 = MISO0
	P0.6 = MOSI0
	P0.7 as SSEL0 
	*/
	IO0DIR = 0x80;//Pin P0.7 as output
	S0SPCR = 0x20;//Set As MASTER by MSTR = 1
	S0SPCCR = (char)0x02;//PCLK = 60MHz and SPI Buad rate = 32MHz
	//IO0CLR=0x80;
}

void spiData(unsigned char data)
{
	IO0SET = 0x00000080;//Set P0.7 as HIGH
	//int x=S0SPSR;
//	while(!(S0SPSR & (0x80)));//Polling for SPIF BIT
	S0SPDR = data;//Send Data to SPDR Buffer
	//S0SPDR = 0xff;
	IO0CLR = 0x00000080;////Set P0.7 as LOW/Clear
}

