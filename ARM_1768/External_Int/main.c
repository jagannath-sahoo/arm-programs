#include "lpc17xx.h"

volatile int flag = 1;

int main()
{
	/*
	Defining LED As Output 
	*/
	LPC_GPIO2->FIODIR|=(1<<9);
	/*
	The PINSEL4 register controls the functions of the lower half of Port 2
	Bit [25:24} used for P2.12[1] GPIO Port 2.12 EINT2
	*/
	LPC_PINCON->PINSEL4|=(1<<24);
	/*
	Making Interrupt as Edge Sensitive 
	Bit [1] EINT2 is edge sensitive Set To 1;
	*/
	LPC_SC->EXTMODE &=~(1<<2);
	/*
	Set Interrupt to EINT2 is low-active or falling-edge sensitive (depending on
EXTMODE2)
	*/
	LPC_SC->EXTPOLAR |=(0<<2);
	NVIC_EnableIRQ(EINT2_IRQn);
while(1)
	{
		//LPC_GPIO2->FIOCLR|=(1<<9);
	}
}


void EINT2_IRQHandler(void)
{
	LPC_SC->EXTINT |=(1<<2);
	if(flag == 1)
	{
		//LED OFF
		flag = 0;
		// SET Bit[9]
		LPC_GPIO2->FIOSET|=(1<<9);
	}
	else{
		//LED ON 
		flag = 1;
		//Clear  the Bit [9] 
		LPC_GPIO2->FIOCLR|=(1<<9);
	}
}