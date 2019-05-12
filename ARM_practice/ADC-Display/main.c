#include <LPC17xx.h>
#include "lcd.h"
#include <string.h>
#include <stdio.h>

/********ADC**********/
void initADC(void);
int getADCVal(void);
///////////////////////

int main()
{
	init_lcd();
	send_cmd(0x00);
	char buff[16] = { 0 };
	initADC();
	while(1)
	{
			/*sprintf(buff,"ADC: %d",getADCVal());
			user_string((unsigned char *)buff);
			//for(long int i = 0; i<= 6000000;i++)
			//	for(long int i = 0; i< 2; i++);
			memset(buff,0,16);
			//send_cmd(0x01);
			*/
			send_cmd(0xC0);
			sprintf(buff,"V: %f",(3.3/4096)*(float)getADCVal());
			user_string((unsigned char *)buff);
			for(long int i = 0; i<= 6000000;i++)
				for(long int i = 0; i< 2; i++);
			memset(buff,0,16);
			send_cmd(0x01);
	}
}
/*****************************************/
void initADC()
{
	//ADC Power On
	LPC_SC->PCONP |= (1<<12);
	//AD0.2 for PINSEL1
	LPC_PINCON->PINSEL1 |= (1<<18);
	//ADC0.2
	//15:8	CLKDIV	2
	//16 BURST MODE
	//21 PDN	ADC Operational
	LPC_ADC->ADCR = 0x00000000;
	LPC_ADC->ADCR |= (1<<2) | (0x01<<8) | (1<<16) | (1<<21);
}

int getADCVal()
{
	LPC_ADC->ADCR |= (1<<24);
	while(!(LPC_ADC->ADSTAT & (1<<2)));
	LPC_ADC->ADCR &= ~(1<<24);
	return((LPC_ADC->ADDR2 >> 4) & 0xfff);
}
/******************************************/
