#define CR 0x0D
#include <LPC214x.H>
void init_serial (void);
char putchar (char ch);
char getchar (void);
unsigned char test;
int main(void)
{
char *Ptr = "Hello UART\n\n\r";
VPBDIV = 0x02; //i Divide Petk by two
init_serial();
while(1)
{
while (*Ptr)
{
putchar(*Ptr);
	Ptr++;
}
putchar(getchar()); // Echo terminal
}
}
void init_serial (void)
{
PINSEL0 = 0x00050000; // Enable RxD0 and TxD0
U1LCR =0x00000083;//#8 bits, no Parity, 1 Stop bit
U1DLL = 0x000000C3;//1/9600 Baud Rate @ 30MHz VPB Clock
U1LCR =0x00000003;
}
char putchar (char ch)
{
if(ch == '\n')
{
while (!(U1LSR & 0x20));
U1THR = CR;
}

while (!(U1LSR & 0x20));
U1THR = ch; 
return ch;

}
char getchar (void)
{
while (!(U1LSR & 0x01));
return (U1RBR);
}