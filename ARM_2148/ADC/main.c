#include <LPC214x.h>
#include <stdio.h>
#include <string.h>

/***************************************************/
/*****************UART******************************/
/*
	Initialize UART
	param: None
	return: void
*/
void initUART(void);
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
unsigned char getChar(void);

/*
Send String
param:	str: String to Send
return: void
*/
void sendString(unsigned char *str);
/***************************************************/

/*
Initialize ADC
*/
void initADC(void);

/*
Get Adc result
params: NONE
return: int type adc result
*/
int	getAdcResult(void);

int main()
{
	VPBDIV = 0x02; //Divide Clock by 2
	char buffer[24] = { 0 };
	int adcValue = 0;
	initUART();
	initADC();
	sendString("Hello");
	while(1)
	{
		adcValue = getAdcResult();
		sprintf(buffer,"ADC = %d\r\n",adcValue);
		sendString(buffer);
		memset(buffer,0,24);
		for(int i = 0; i <= 50000; i++);
	}	
	return 0;
}


void initADC()
{
	AD0CR = 0x00;//Reset
	/*
	PIN Selection Reg.
	BIT[29:28]		P0.30 	AD0.3		01 for use as ADC 
	*/
	PINSEL1 |= (0x01<<28);
	/*
	A/D Control Register (AD0CR)
	BIT[7:0] SEL 		Selects Bit3 for AD0.3
	*/
	AD0CR |= (1<<3);
	/*
	A/D Control Register (AD0CR)
	BIT[15:8] CLKDIV	The APB clock (PCLK) is divided by (this value plus one) to produce the clock for the
						A/D converter, which should be less than or equal to 4.5 MHz
						PCLK = 30MHz and ADC CLOCK = 2MHz so CLKDIV = 0xf
	*/
	AD0CR |= (0xF<<8);
	/*
	BIT[16]		BURST		The AD converter does repeated conversions
	*/
	AD0CR |= (1<<16);
	/*
	19:17 CLKS		This field selects the number of clocks 
								used for each conversion in Burst mode,
				000		  11 clocks / 10 bits
	*/
	//AD0CR |= (0x000<<17);
	//AD0CR &= ~(0x000<<17);
	AD0CR &= ~(0x111<<17);
	/*
	BIT[21]	PDN		1		A/C Converter is Operational		
	*/					
	AD0CR |= (1<<21);
	/*
	BIT[26:24] START		000 No Start conversion
	*/
	AD0CR |= (0x000<<24);
}

int	getAdcResult()
{
	int result;
	/*
	BIT[26:24] START		001  Start conversion
	*/
	AD0CR |= (0x001<<24);
	/*
	A/D Status Register (AD0STAT)
	BIT[3]	 DONE3 		This bit mirrors the DONE status flag from the result register for A/D channel 3.
	*/
	while(!(AD0STAT & (1<<3)));//Trap
	/*
	BIT[15:6] 	RESULT  	Result of ADC conversion
	*/
	result = (AD0DR3>>6)&(0x3FF);//First shift 6 bits and then add mask for first bits 
	return result;
}

/*********************************************************************/
/*************************UART****************************************/
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
/*********************************************************************/
