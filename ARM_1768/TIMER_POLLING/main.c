/*
PCLK ==>	TIMER-CLOCK
|						|-> PLCOK/4
|->100MHz		|-> 25MHz

For 1Sec delay

		1
___________		x 25000	=	10^-3  

 25000000


..................................
FROM DATASHEET
..................................
The Timer 0, 1, 2, and 3 peripherals are configured using the following registers:
1. Power: In the PCONP register (Table 46), set bits PCTIM0/1/2/3.
Remark: On reset, Timer0/1 are enabled (PCTIM0/1 = 1), and Timer2/3 are disabled
(PCTIM2/3 = 0).
2. Peripheral clock: In the PCLKSEL0 register (Table 40), select PCLK_TIMER0/1; in the
PCLKSEL1 register (Table 41), select PCLK_TIMER2/3.
3. Pins: Select timer pins through the PINSEL registers. Select the pin modes for the
port pins with timer functions through the PINMODE registers (Section 8.5).
4. Interrupts: See register T0/1/2/3MCR (Table 430) and T0/1/2/3CCR (Table 431) for
match and capture events. Interrupts are enabled in the NVIC using the appropriate
Interrupt Set Enable register.
5. DMA: Up to two match conditions can be used to generate timed DMA requests, see
Table 544.

...................................
*/

#include <LPC17xx.h>

/*
Initailize timer
*/
void initTimer(void);


int main()
{
	/*
	PINSEL4 Register
	BIT[23:22]	00 	P2.11		GPIO Port BUZZER
	BIT[19:18] 	00	P2.9 		GPIO Port LED
	*/
	//LPC_PINCON->PINSEL4 &= ~(1<<23) & ~(1<<22);
	/*
	Fast GPIO Port Direction control register(FIODIR)
	BIT[11]	FOR P2.11 	as OUTPUT
	BIT[9]	FOR P2.9		as OUTPUT
	*/
	//LPC_GPIO2->FIODIR |= (1<<11);
	LPC_GPIO2->FIODIR |= (1<<9);
	
	initTimer();
	
	while(1)
	{
		/*
		Fast Port Output Set register(FIOSET)
		BIT[11]	FOR P2.11 	as HIGH
		BIT[9]	FOR P2.9 		as HIGH
		*/
		//LPC_GPIO2->FIOSET |=(1<<11);
		LPC_GPIO2->FIOSET |= (1<<9);
		LPC_TIM0->TCR = 0x00;
	  LPC_TIM0->TCR |= (1<<0);
		while(!(LPC_TIM0->TC == 0x3E7));//TIMER
		
		//LPC_GPIO2->FIOCLR |=(1<<11);
		LPC_GPIO2->FIOCLR |= (1<<9);
		LPC_TIM0->TCR = 0x00;
	  LPC_TIM0->TCR |= (1<<0);
		while(!(LPC_TIM0->TC == 0x3E7));//TIMER
	}
}

void initTimer()
{
	LPC_TIM0->PC = 0x00;
	/*
	Power Control for Peripherals register (PCONP)
	BIT[1]	 PCTIM0 Timer/Counter 0 power/clock control bit.
	*/
	LPC_SC->PCONP |= (1<<1);
	
	/*
	Peripheral Clock Selection registers 0 and 1 (PCLKSEL0
	
	BIT[3:2] PCLK_TIMER0 Peripheral clock selection for TIMER0
	BIT[00] PCLK_peripheral = CCLK/4
	*/
	LPC_SC->PCLKSEL0 &= ~(1<<3) | ~(1<<2);
	
	/*
	Prescale register (T0PR)
	25000 - 1 = 0x61A7
	*/
	LPC_TIM0->PR = 0x61A7;
	/*
	Match Registers (MR0)
	*/
	LPC_TIM0->MR0 = 0x3E7;
	
	/*
	Match Control Register (T[0/1/2/3]MCR)
	BIT[1] MR0R 1 Reset on MR0: the TC will be reset if MR0 matches it.
	*/
	LPC_TIM0->MCR |= (1<<1);

	/*
	Timer Control Register (TCR0)
	BIT[0] Counter Enable When one, the Timer Counter and Prescale Counter are enabled for counting.
	*/
	//LPC_TIM0->TCR = 0x00;
	//LPC_TIM0->TCR |= (1<<0);
}
