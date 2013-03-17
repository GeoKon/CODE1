/* LRU: GK 3/17/2013

   To Do:

	- calibrate 1min counter
	- include #define for options
		01 for fast minute advancing (for testing)
		02 for include 
	- remove cursor from LCD. goto (0,0) without reseting.
	- include thermistor reading
	- connect RS-232
	- connect UART
	- implement CLI
	- implement EEPROM
		configuration options
		calibration coefficients
		past statistics
	- implement one hour interruption to EEPROM
*/

#include <18F2550.h>
#device ADC=10

//configure a 20MHz crystal to operate at 48MHz
#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL5,CPUDIV1,VREGEN
//, CCP2B3
//#fuses INTRC, NOWDT, NOPROTECT, NOLVP, NODEBUG	/*************/
//#fuses   USBDIV, PLL1, CPUDIV1, PROTECT, NOCPD, noBROWNOUT,HSPLL,NOWDT,nolvp, VREGEN

#use delay(clock=48000000)

//#use i2c(master, sda=PIN_B0, scl=PIN_B1, FORCE_HW, SLOW)

/* -----------------------------------------------------------------------------------
   Typedefs and useful macros
   ------------------------------------------------------------------------------------
*/
typedef unsigned int16 WORD;
#define HI(A) output_high( A )
#define LO(A) output_low( A )
#define BLINK(A,B,C) HI(A);delay_ms(B);LO(A);delay_ms(C)
#define BL3ST(A,B,C) HI(A);delay_ms(B);input(A);delay_ms(C)

/* -----------------------------------------------------------------------------------
   Hardware Pin Definitions
   -----------------------------------------------------------------------------------
*/
#define pV1ADC pin_A0		// Battery measuring voltage
#define pV2ADC pin_A1		// Power supply voltage
#define pBTEST pin_A4		

#define pFLUX5 pin_A4		// pin shared with pBTEST (!!!)
#define pFLUX4 pin_C1
#define pFLUX3 pin_C2
#define pFLUX2 pin_C3

#define LCD_DATA4	PIN_B4
#define LCD_DATA5	PIN_B5
#define LCD_DATA6	PIN_B6
#define LCD_DATA7	PIN_B7
#define LCD_ENABLE	PIN_B1		
#define LCD_RS		PIN_B3

#define pPB1	pin_B7		// pin shared with LCD_DATA7
#define pPB2	pin_B6		// pin shared with LCD_DATA6
#define pPOT	pin_B5		// pin shared with LCD_DATA5

#define pSENPWR	pin_B1		// LOW to enable sensor power
#define pLD1	pin_B0		// Positive logic
#define pLD3	pin_C6		// TxD and respective LED. On if LOW

#define EE_BASE 	0
#define EE_V12COEF	1
#define NEXT		EE_ADCCOEF+8*sizeof(float)

/* -----------------------------------------------------------------------------------
   Include files
   -----------------------------------------------------------------------------------
*/
#include "lcd16c.h"			// the LCD routine -- no dependencies
#include "inpout.h"			// all IO routines -- no dependencies
#include "tmbase.h"			// implements queue at ISR level with all timing. Depends on inpout.h
#include "calibr.h"			// conversion adc-->V 				depends on LCD and INPOUT		
#include "adcomp.h"			// adc_reset(), adc_update(), adc_compute()
#include "amphrs.h"			// computes AH						depends on INPOUT
#include "disply.h"			// menuing and display routines		depends on LCD, INPOUT, calcAH

/* -----------------------------------------------------------------------------------
   Initialization
   -----------------------------------------------------------------------------------
*/
void io_init()         			// initialized all IO pins to the proper questent state
{								// anyone who modifies this default, sets it back
	set_tris_c(0);
	set_tris_b(0b00000100 );	// 1=INPUT, 0=OUTPUT

	setup_adc_ports(  AN0_TO_AN9 | VREF_VREF );
   	setup_adc( ADC_CLOCK_INTERNAL  );
}

/* -----------------------------------------------------------------------------------
   Main Progam
   -----------------------------------------------------------------------------------
*/

void blinkLED()
{
	static byte cadence;
	cadence++;

	if( ahr.state ) 
	{
		if( cadence & 1 )	// every 400ms
			LED1ON;
		else
			LED1OFF;
	}
	else
	{
		if( cadence & 4 )	// every 400ms
			LED1ON;
		else
			LED1OFF;
	}

}

void main()
{
	word i;
	byte pb, j, ev, minute=0, count;
	byte i1, i2;
	
	io_init();

/*							
	for(;;)									// quick test routine
	{
		HI( PIN_C6 );
		delay_ms( 200 );
		LO( PIN_C6 );
		delay_ms( 200);

	}
*/
	calibr_init();							// initialize calibration routines
	lcd_init();
	tm_init();								// initialize timer routines
	
	adc_reset();							// null adc averages
	ahr_reset();								// null AH counters

	enable_interrupts( GLOBAL );
	
	for(i=0;;i++)							// main polling loop
	{
		if( pb = read_button() )			// check if any button is pressed
		{
			if( pb==3 )						// calibration
				calibr_V12();
			else
				disp_select( pb );
				;
		}
		while( tm_evready() )				// process event queue
		{
			ev = tm_getev();				// get event

			if( ev == TIC_ADC )				// Every 5ms (appx) Take an ADC measurement and accumulate
			{
				adc_update();				
			}
			else if( ev == TIC_DISP )		// every 400ms (appx) Compute averages, see if AC is off, start AH counting
			{
				if( ahr.state ) LED1OFF;
				else LED1ON;
				adc_compute();				// compute averages and engineering values
				ahr_check();				// check if necessary to start computing AH
				disp_values();				// display values
				adc_reset();				// reset averages
				if( ahr.state ) LED1ON;
				else LED1OFF;
			}
			else if( ev == TIC_MIN )		// every 1min (exactly). If on BAT, compute AH
			{
				ahr_update();				// compute AH and statistics
			}
			else
				display1("\fERR=%d", ev);
		}
	}
}
