#include <LPC214x.h>

static int flag = 1; 
/*
Initialize Timer1
*/
void initTimer(void);

void TIMER1()	__irq
{
	T1IR |= (1<<0);//to reset Interrupt flag for match channel 0 or MR0
	//BIT[1] 	MR1 	Interrupt Interrupt flag for match channel 1.
	//T1IR |= (1<<1);
	if(flag == 1)
	{
		IO0SET = 0x80000000;//P0.31 as HIGH
		flag = 0;
	}
	else
	{
		IO0CLR = 0x80000000;//P0.31 as LOW
		flag = 1;
	}
	
	T1TC=0x00;//Reset the timer counter
 	//Add Dummy value to exit from ISR
	VICVectAddr = 0x00;
}

int main()
{
	//P0.31 LED
	IO0DIR = 0x80000000;//P0.31 => OUTPUT
	//IO0SET = 0x80000000;//P0.31 as HIGH
	//IO0CLR = 0x80000000;//P0.31 as LOW
	initTimer();
	while(1)
	{
		;
	}
	return 0;
}

void initTimer()
{
	//PCLCK = 60MHz
	VPBDIV |= (1<<0);
	//
	//Timer1 Prescale Reg as 60x10^3 - 1
	T1PR = 0x0000EA5F;
	//Timer1 Match Register 
	T1MR1 = 0x000003E;//1000 - 1
	//Timer Match Control Reg
	//BIT 4 as Reset on Match
	//BIT 3 MR1I Interrupt on MR1: an interrupt is generated when MR1 matches the value in the TC.
	T1MCR |= (1<<4) | (1<<3);
	
	/*******INTERRUPT********************************************/
	//BIT[1] 	MR1 	Interrupt Interrupt flag for match channel 1.
	T1IR |= (1<<1);
	//Add the ISR Function with VICVect Address Table
	VICVectAddr0 = (unsigned long) TIMER1;
	//VICVect Interrupt control
	VICVectCntl0 = 0x20 | 5;
	//Enable VIC interrupt table
	VICIntEnable |= (1<<5);
	//Clear Timer Control Reg and Enable Timer
	T1TCR = 0x00;
	T1TCR |= (1<<0);
}
