#include <LPC214x.h>


/*
Initialize Timer
*/
void initTimer(void);



int main()
{
	initTimer();
	//P0.31 LED
	IO0DIR = 0x80000000;//P0.31 => OUTPUT
	//IO0SET = 0x80000000;//P0.31 as HIGH
	//IO0CLR = 0x80000000;//P0.31 as LOW
	while(1)
	{
		IO0CLR = 0x80000000;//P0.31 as LOW
		while(!(T1TC == 0x000003E4));//Check whether it reached or not
		
		IO0SET = 0x80000000;//P0.31 as HIGH
		while(!(T1TC == 0x000003E4));//Check whether it reached or not
	}
	return 0;
}

void initTimer()
{
	//Set PCLK same as CCLK Value 
	//APVDIV    BIT[01]		APB bus clock is the same as the processor clock.
	VPBDIV = (1<<0);
	/*
	1ms = 60x10^3 - 1
	*/
	//T1CTCR=0x00;
	T1PR = 0x0000EA5F;
	T1MR1 = 0x000003E7;//1000 - 1
	/*
	MATCH CONTROL REG.
	BIT[4]	Reset on MR1: the TC will be reset if MR1 matches it
	*/
	T1MCR |= (1<<4);
	
	/*
	Timer Control Register
	BIT[0] Counter Enable 
				 When one, the Timer Counter and Prescale Counter are
				 enabled for counting. When zero, the counters are
		     disabled
	
	BIT[1] Counter Reset
				 When one, the Timer Counter and the Prescale Counter
				 are synchronously reset on the next positive edge of
				 PCLK. The counters remain reset until TCR[1] is
				 returned to zero
	*/
	T1TCR = 0x00;
	T1TCR |= (1<<0);// | (1<<1); 
	//T1TCR =0x01; 
	
}
