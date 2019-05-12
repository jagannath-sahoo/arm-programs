#include <lpc17xx.h>
void send_data(unsigned char);
void send_cmd(unsigned char);
void delay(void);
void user_string(unsigned char *);


void send_data(unsigned char data)
{
		unsigned char data1;
		data1=data;
		
		data1=(data1 & 0xF0);	// sending the Higher nibble First
		LPC_GPIO1->FIOCLR|=(1<<23); // make RW LOW
		LPC_GPIO1->FIOSET|=(1<<24); // make RS high--data mode off
		
		LPC_GPIO2->FIOSET|=(data1);	// sending the data	or rather '1' to the port pins (DB4-DB7: connected to pins 10,11,12,13)
		LPC_GPIO2->FIOCLR|=(~(data1)); //we are sending the '0' value to the port pins
	
		LPC_GPIO1->FIOSET|=(1<<22); //Enable LCD
		delay();
		LPC_GPIO1->FIOCLR|=(1<<22); //
		delay();

		// sending the lower nibble
		data1=data;
		data1=(data1 & 0x0F);
		data1=(data1 << 4);
		
		LPC_GPIO2->FIOSET|=(data1);	// sending the data
		LPC_GPIO2->FIOCLR|=(~(data1));
	
		LPC_GPIO1->FIOSET|=(1<<22);
		delay();
		LPC_GPIO1->FIOCLR|=(1<<22);
		delay();

}

void send_cmd(unsigned char data)
{
		unsigned char data1;
		data1=data;
		
		data1=(data1 & 0xF0);	// sending the Higher nibble First
		LPC_GPIO1->FIOCLR|=(1<<23); // make RW LOW
		LPC_GPIO1->FIOCLR|=(1<<24);// make RS LOW
		
		LPC_GPIO2->FIOSET|=(data1);	// sending the Cmd or rather '1' to the port pins (DB4-DB7: connected to pins 10,11,12,13)
		LPC_GPIO2->FIOCLR|=(~(data1));
		
		LPC_GPIO1->FIOSET|=(1<<22);  // Toggle EN_LCD Pin
		delay();
		LPC_GPIO1->FIOCLR|=(1<<22);
		delay();
		
		// sending the lower nibble
		//data1=data;
		data1=(data & 0x0F);
		data1=(data1 << 4);
		
		LPC_GPIO2->FIOSET|=(data1);	// sending the Cmd or rather '1' to the port pins (DB4-DB7: connected to pins 10,11,12,13)
		LPC_GPIO2->FIOCLR|=(~(data1));
		
		LPC_GPIO1->FIOSET|=(1<<22);  // Toggle EN_LCD Pin
		delay();
		LPC_GPIO1->FIOCLR|=(1<<22);
		delay();
}  
void delay(void)
{
	int i,j;

	for(i=0;i<=0x0FF;i++)
	for(j=0;j<=0xFF;j++);
}	
void user_string(unsigned char *str)
{
	unsigned char i;
	for(i=0;str[i]!='\0';i++)
		send_data(str[i]);

}
void init_lcd()
{
	LPC_GPIO2->FIODIR |= (1<<4)|(1<<5)|(1<<6)|(1<<7);
	LPC_GPIO1->FIODIR |= (1<<22)|(1<<23)|(1<<24);
	

	send_cmd(0x28);// 0010 1000 Function Set(): interfacelength N=1 means two lines ,DL=0 means 4-bit interface 
    send_cmd(0x06);// 0000 0110 Entry Mode Set(): ID=1 means increment the cursor position after write
    send_cmd(0x0F);// 0000 1111 Display On/Off(): display =1 , cursor =1 , blinking =1
    send_cmd(0x01);// 0000 0001 Clear Display(): Clears display and returns cursor to the home position (address 0).
    send_cmd(0x02);// 0000 0010 Cursor Home(): 
}