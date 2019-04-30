#include "utils.h"
#include "lpc_2148.h"
#include "lcd.h"
#if 0
#include "softtimer.h"

#define LCD_BACK_LIGHT_TIMEOUT 1000

#define LCD_BACKLIGHT BIT21

#define LCD_BACK_LIGHT_DIR FIO1DIR
#define LCD_BACK_LIGHT_SET FIO1SET
#define LCD_BACK_LIGHT_CLR FIO1CLR
#endif
#define LCD_DATA_DIR		FIO0DIR
#define LCD_DATA_SET		FIO0SET
#define LCD_DATA_CLR		FIO0CLR

#define LCD_CTRL_DIR		        FIO1DIR
#define LCD_CTRL_SET            FIO1SET
#define LCD_CTRL_CLR            FIO1CLR

#define LCDRS	                (BIT24)
#define LCDRW	                (BIT23)
#define LCDEN	                (BIT22)

#define LCD_D4 BIT10
#define LCD_D5 BIT11
#define LCD_D6 BIT12
#define LCD_D7 BIT13

#define LCD_DATA_MASK           (LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7)
#define LCD_BUSY_FLAG           LCD_D7

#define LCD_CONTROL_MASK        0x01C00000
#if 0
struct uclk lcd_back_light_timer;

static void start_lcd_back_light_timer(void);
static void stop_lcd_back_light_timer(void);
#endif
/**
********************************************************************************************
  Function Name :	wait_lcd()
  
  Description   :	
  
  Input         :	
  
  Output        :	Void
  
  Note          :
********************************************************************************************
*/
void wait_lcd( void )
{
  LCD_CTRL_CLR |=  LCDRS;	
  LCD_CTRL_SET |=  LCDRW |LCDEN;
  while(FIO1PIN & LCD_BUSY_FLAG);		/* wait for busy flag to become low */
  
  LCD_CTRL_CLR |= LCDEN | LCDRW;
  LCD_DATA_DIR |= LCD_DATA_MASK;
  
  delay(100);  
}
/**
********************************************************************************************
  Function Name :	lcd_command_write()
  
  Description   :	
  
  Input         :	
  
  Output        :	Void
  
  Note          :
********************************************************************************************
*/
void lcd_command_write( unsigned char command )
{
  U8 temp=0;
  U32 temp1=0;

  temp=command;
  temp=(temp>>4)&0x0F;
  temp1=(temp<<10)&LCD_DATA_MASK;

  LCD_CTRL_CLR = LCDRS;
  LCD_CTRL_SET = LCDEN;
  LCD_DATA_CLR = LCD_DATA_MASK;
  LCD_DATA_SET = temp1;
  delay(10000);
  LCD_CTRL_CLR = LCDEN;

  temp=command;
  temp&=0x0F;
  temp1=(temp<<10)&LCD_DATA_MASK;
  delay(100*2);

  LCD_CTRL_CLR |= LCDRS;
  LCD_CTRL_SET |= LCDEN;
  LCD_DATA_CLR = LCD_DATA_MASK;
  LCD_DATA_SET = temp1;
  delay(10000);	
  LCD_CTRL_CLR |= LCDEN;
  wait_lcd();
}
/**
********************************************************************************************
  Function Name :	set_lcd_port_output()
  
  Description   :	
  
  Input         :	
  
  Output        :	Void
  
  Note          :
********************************************************************************************
*/
void set_lcd_port_output( void )
{
  LCD_CTRL_DIR |= ( LCDEN | LCDRS | LCDRW );
  LCD_CTRL_CLR |= ( LCDEN | LCDRS | LCDRW );	
  LCD_DATA_DIR |= LCD_DATA_MASK;
}
/* *
********************************************************************************************
  Function Name :	lcd_clear()
  
  Description   :	
  
  Input         :	
  
  Output        :	Void
  
  Note          :
********************************************************************************************
*/
void lcd_clear( void)
{
  lcd_command_write( 0x01 );
}
/**
********************************************************************************************
  Function Name :	lcd_gotoxy()
  
  Description   :	
  
  Input         :	
  
  Output        :	Void
  
  Note          :
********************************************************************************************
*/
int lcd_gotoxy( unsigned int x, unsigned int y)
{
  int retval = 0;
  
  if( (x > 1) && (y > 15) )
  {
    retval = -1;
  } else {
  if( x == 0 )
  {
    lcd_command_write( 0x80 + y );		/* command - position cursor at 0x00 (0x80 + 0x00 ) */
  } else if( x==1 ){
    lcd_command_write( 0xC0 + y );		/* command - position cursor at 0x40 (0x80 + 0x00 ) */
    }
   }
   return retval;
}

/**
********************************************************************************************
  Function Name :	lcd_data_write()
  
  Description   :	
  
  Input         :	
  
  Output        :	Void
  
  Note          :
********************************************************************************************
*/
void lcd_data_write( unsigned char data )
{
  U8 temp=0;
  U32 temp1=0;

  temp=data;
  temp=(temp>>4)&0x0F;
  temp1=(temp<<10)&LCD_DATA_MASK;

  LCD_CTRL_SET |= LCDEN|LCDRS;
  LCD_DATA_CLR = LCD_DATA_MASK;
  LCD_DATA_SET = temp1;
  delay(10);
  LCD_CTRL_CLR |= LCDEN;

  temp=data;
  temp&=0x0F;
  temp1=(temp<<10)&LCD_DATA_MASK;

  LCD_CTRL_SET |= LCDEN|LCDRS;
  LCD_DATA_CLR = LCD_DATA_MASK;
  LCD_DATA_SET = temp1;
  delay(10);
  LCD_CTRL_CLR |= LCDEN;
  wait_lcd();
}
/**
********************************************************************************************
  Function Name :	lcd_putchar()
  
  Description   :	
  
  Input         :	
  
  Output        :	Void
  
  Note          :
********************************************************************************************
*/
void lcd_putchar( int c )
{
  lcd_data_write( c );
}

/**
********************************************************************************************
  Function Name :	lcd_putstring()
  
  Description   :	
  
  Input         :	
  
  Output        :	Void
  
  Note          :
********************************************************************************************
*/
void lcd_putstring( unsigned char line, char *string )
{
  U8 len = MAX_CHAR_IN_ONE_LINE;

  lcd_gotoxy( line, 0 );
  while(*string != '\0' && len--)
  {
    lcd_putchar( *string );
    string++;
  }
}
#if 0
/**
********************************************************************************************
  Function Name :	lcd_backlight_on()
  
  Description   :	
  
  Input         :	
  
  Output        :	Void
  
  Note          :
********************************************************************************************
*/
void lcd_backlight_on()
{    
  stop_lcd_back_light_timer();
  LCD_BACK_LIGHT_DIR |= LCD_BACKLIGHT;
  LCD_BACK_LIGHT_SET |= LCD_BACKLIGHT;	
  start_lcd_back_light_timer();
}

/**
********************************************************************************************
 Function Name : turn_off_lcd_back_light()

 Description   :	

 Input         :	

 Output        : Void

 Note          :
********************************************************************************************
*/
void turn_off_lcd_back_light_cb(void)
{
   LCD_BACK_LIGHT_DIR |= LCD_BACKLIGHT;
   LCD_BACK_LIGHT_CLR |= LCD_BACKLIGHT;
}

/**
********************************************************************************************
 Function Name : start_lcd_back_light_timer()

 Description   :	

 Input         :	

 Output        : Void

 Note          :
********************************************************************************************
*/
static void start_lcd_back_light_timer(void)
{
  def_uclk(&lcd_back_light_timer);
  lcd_back_light_timer.repeat = 0;
  lcd_back_light_timer.expired= 0;
  lcd_back_light_timer.ticks = LCD_BACK_LIGHT_TIMEOUT;
  lcd_back_light_timer.function_callback = turn_off_lcd_back_light_cb;
  reg_uclk(&lcd_back_light_timer);
}

/**
********************************************************************************************
 Function Name : stop_lcd_back_light_timer()

 Description   :	

 Input         :	

 Output        : Void

 Note          :
********************************************************************************************
*/
static void stop_lcd_back_light_timer(void)
{
  dereg_uclk(&lcd_back_light_timer);
}
#endif
/**
********************************************************************************************
  Function Name :	init_lcd()
  
  Description   :	
  
  Input         :	
  
  Output        :	Void
  
  Note          :
********************************************************************************************
*/
void init_lcd( void )
{
  set_lcd_port_output();
  delay(100*100);
  lcd_command_write(0x28);     /*   4-bit interface, two line, 5X7 dots.        */
  lcd_clear() ;                /*   LCD clear                                   */
  lcd_command_write(0x02);     /*   cursor home                                 */
  lcd_command_write(0x06);     /*   cursor move direction                       */
  lcd_command_write(0x0C) ;    /*   display on      */
  lcd_gotoxy(0, 0);
  lcd_clear();	
  lcd_putstring(0,"     N G X   ");
  lcd_putstring(1,"  TECHNOLOGIES ");
}
