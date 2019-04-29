#include "LPC17xx.h"

int Maun()
{
unsigned int i,j;
	LPC_PINCON->PINSEL4&=~((1<<22)|(1<<23));
	LPC_PINCON->PINSEL4&=~((1<<18)|(1<<19));
	LPC_GPIO2->FIODIR1 |= ((1<<3)|(1<<1));
	
	
	while(1)
	{
	LPC_GPIO2->FIOSET1 |=((1<<3)|(1<<1));
		for(int i =0 ;i<10000;i++);
	LPC_GPIO2->FIOCLR1 &=~((1<<3)|(1<<1));
		for(int i =0 ; i<10000;i++);
		
	}

}