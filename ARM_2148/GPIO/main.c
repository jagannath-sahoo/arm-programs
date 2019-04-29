/*
	GPIO Demo Example
	Author: Jagannath & Ajinkya
*/

#include "LPC214x.h"


int main(){
	
	//P1.25 Buzzer
	//P0.31 Connect Led
	
	//1. Specifiy the directional register using IOxDIR
	IO0DIR = 0x80000000;//P0.31 => OUTPUT
	IO1DIR = 0x02000000;//P1.25 => OUTPUT
	
	while(1)
	{
		//2a. Set pins high using IOxSET register
		IO0SET = 0x80000000;//P0.31 as HIGH
		IO1SET = 0x02000000;//P1.25 as HIGH
		
		//3. Create Delay using for loop
		for(int i = 0; i <= 1000000 ; ++i);
		
		//2b. Set pins low using IOxCLR register
		IO0CLR = 0x80000000;//P0.31 as LOW
		IO1CLR = 0x02000000;//P1.25 as LOW
		
		//3. Create Delay using for loop
		for(int i = 0; i <= 1000000 ; ++i);
	}
}
