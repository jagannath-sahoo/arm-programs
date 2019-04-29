#include "LPC17xx.h"                    // Device header
/*
P2.11 FOR BUZZER
P2.9	FOR CONNECT LED
*/

int main()
{
	/*
	PINSEL4 Register
	BIT[23:22]	00 	P2.11		GPIO Port
	BIT[19:18] 	00	P2.9 		GPIO Port
	*/
	LPC_PINCON->PINSEL4 &= ~(1<<23) & ~(1<<22);
	/*
	Fast GPIO Port Direction control register(FIODIR)
	BIT[11]	FOR P2.11 	as OUTPUT
	BIT[9]	FOR P2.9		as OUTPUT
	*/
	LPC_GPIO2->FIODIR |= (1<<11);
	LPC_GPIO2->FIODIR |= (1<<9);
	while(1)
	{
		/*
		Fast Port Output Set register(FIOSET)
		BIT[11]	FOR P2.11 	as HIGH
		BIT[9]	FOR P2.9 		as HIGH
		*/
		LPC_GPIO2->FIOSET |=(1<<11);
		LPC_GPIO2->FIOSET |= (1<<9);
		for(int i = 0; i<=5000;i++)
		{
			for(int j = 0; j<= 100 ;j++);
		}
		/*
		Fast Port Output Clear register(FIOCLR)
		BIT[11]	FOR P2.11 	as LOW
		BIT[9]	FOR P2.9 		as LOW
		*/
		LPC_GPIO2->FIOCLR |=(1<<11);
		LPC_GPIO2->FIOCLR |=(1<<9);
		for(int i = 0; i<=5000;i++)
		{
			for(int j = 0; j<= 100 ;j++);
		}
	}
	return 0;
}
