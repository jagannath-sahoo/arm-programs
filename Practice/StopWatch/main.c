#include "LPC17xx.h"
#include "uart.h"

int main()
{
	initUART();
	sendChar('A');
	while(1)
	{
		sendString("Hello UART\r\n");
	}
}
