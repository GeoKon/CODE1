/* -------------------------------------------------------------------------
 *								GK_LCD.C
 *						Driver for LCD 1x16 module
 * -------------------------------------------------------------------------
 
   lcd_init(r)	call before any other function
 
   lcd_putc(c)	displays character c on the next position of the LCD
   				Escape codes include:

   				\f	0x0c	Clear Display, home cursor
   				\n	0x0a	Go home (or next line if available -- not implemented)
   				\r	0x0d	Go home
   				\b	0x08	Go backwards one position
   				\t	0x09	Go forwards one position
   				\a	0x07	Display cursor
   				\v	0x0b	Hide cursor
   				\x80..\x8F	Go to position 0...15

   -------------------------------------------------------------------------

	GK 7/10/2002 Original
*/   				

#ifndef LCD_DATA4
  #define LCD_DATA4		PIN_B4
  #define LCD_DATA5		PIN_B5
  #define LCD_DATA6		PIN_B6
  #define LCD_DATA7		PIN_B7
  #define LCD_ENABLE	PIN_B2		// sometimes this is backwards
  #define LCD_RS		PIN_B3
//  #define LCD_RW	    PIN_D6		// used only during init().  Typically set low at all times
#endif

#define DISPLAY(S)  printf(lcd_putc,S)
#define DISPLAY1(S,A)  printf(lcd_putc,S,A)
#define DISPLAY2(S,A,B)  printf(lcd_putc,S,A,B)
#define DISPLAY3(S,A,B,C)  printf(lcd_putc,S,A,B,C)

// -------------------------------------------------------------------------
//	INTERNAL: lcd_nibble()		sends one nibble to the LCD
// -------------------------------------------------------------------------

void lcd_nibble( byte n )
{
	short temp;
//	temp = input( LCD_DATA4 );		// save bit 0

	output_bit( LCD_DATA4, n&1);
	output_bit( LCD_DATA5, n&2);
	output_bit( LCD_DATA6, n&4);
	output_bit( LCD_DATA7, n&8);
    delay_cycles(1);

 	output_high( LCD_ENABLE );
    delay_us( 100 );
	output_low( LCD_ENABLE );
	delay_us( 100 );

//	output_bit( LCD_DATA4, 0 /*temp*/ );	// restore bit0
}

// -------------------------------------------------------------------------
//	INTERNAL:  lcd_command()	Sends a byte command to the LCD
// -------------------------------------------------------------------------

void lcd_command( byte n )
{
	output_low( LCD_RS );
	delay_us( 2 );

	output_low( LCD_ENABLE );
	lcd_nibble( n>>4 );
	lcd_nibble( n & 0xf );

	delay_us( 60 );				// delay at least 40us
}

#define LCD_CLEAR_DISPLAY 	lcd_command( 0b00000001 ); delay_ms( 10 )
#define LCD_HOME_CURSOR		lcd_command( 0b00000010 ); delay_ms( 5 )
#define LCD_BACKWARD		lcd_command( 0b00010000 )
#define LCD_FORWARD			lcd_command( 0b00010100 )
#define LCD_SET_CGRAM(x)    lcd_command( 0b01000000 | (x) )
#define LCD_SET_ADDRESS(x)  lcd_command( 0b10000000 | (x) )	// display 1...16 (memory 00...0f)

// -------------------------------------------------------------------------
//	INTERNAL:  lcd_data()		sends a datum byte to the LCD
// -------------------------------------------------------------------------

void lcd_data( byte n )
{
	output_high( LCD_RS );
	delay_us( 2 );

	output_low( LCD_ENABLE );
	lcd_nibble( n>>4 );					// toggle ENABLE
	lcd_nibble( n & 0xf );				// toggle ENABLE again.  Leave it low.
}

// -------------------------------------------------------------------------
//	EXTERNAL:  lcd_init()		call before you use any other function
// -------------------------------------------------------------------------

int pos = 0;							// cursor position (in software)

void lcd_init() 
{
//	output_low( LCD_RW );				// this is the ONLY place RW is used!
	output_low( LCD_ENABLE );
	output_low( LCD_RS );	

	delay_ms( 45 );						// Wait the LCD to finish is power on reset

	lcd_nibble( 0b0011 );					
	delay_ms(5);						// Need to wait at least 4.1 mS
	lcd_nibble( 0b0011 );									
	delay_ms(1);						// need at least 100us
	lcd_nibble( 0b0011 );
	
	lcd_nibble( 0b0010 );
	
	lcd_command( 0b00101000 );			// FUNCTION SET
										// 0010 xxxx = 4-bit interface
										// 0010 1xxx = 1/16 duty cycle (M1641), 0xxx = 1/8 DC
										// 0010 x0xx = 5x7 dots (M1641), x1xx = 5x10 dots

	lcd_command( 0b00001110 );			// 0000 11xx = display ON
										// 0000 1x10 = cursor ON
										// 0000 1xx1 = cursor blink

    LCD_CLEAR_DISPLAY;
    
	lcd_command( 0b00000110 );			// 0000 011x = increment cursor position
										// 0000 01x0 = no display shift
	pos = 0;
}

// -------------------------------------------------------------------------
//	EXTERNAL:  lcd_putc()		writes a characted to LCD
// -------------------------------------------------------------------------

void lcd_putc( char c)
{
	switch (c)
	{
    	case '\f':						// 0x0C CLEAR Display, home cursor
										// Serial LCD standard = 0x01 ^A
    		LCD_CLEAR_DISPLAY;
    		pos = 0;
			
            break;

		case '\n':						// ^M
			LCD_SET_ADDRESS( 0x40 );
			pos = 0;
			break;

		case '\r':						// home cursor.  Does not clear LCD
										// Serial LCD standard = 0x02 ^B
			LCD_HOME_CURSOR;
			pos = 0;
			break;

		case '\a':						// Display cursor (set cursor ON)
										// Serial LCD standard = 0x0e
			lcd_command( 0b00001110 );	// 0000 11xx = display ON
			break;						// 0000 1x10 = cursor ON
										// 0000 1xx1 = cursor blink

		case '\v':						// Hide cursor (set cursor OFF)
										// Serial LCD standard = 0x0c
			lcd_command( 0b00001100 );	// 0000 11xx = display ON
			break;						// 0000 1x10 = cursor ON
										// 0000 1xx1 = cursor blink

     	default:						// if >128, go to location.  Othewise, display character
		    if( pos < 8 )
   			{
   		   		lcd_data( c );
       			pos++;				    // if cursor at location 0x08, jump to 0x40
   	 	    }
     		break;
   }
}

