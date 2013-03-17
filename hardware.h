#include <18F2550.h>

  //configure a 20MHz crystal to operate at 48MHz
#fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL5,CPUDIV1,VREGEN, CCP2B3
// #fuses INTRC, NOWDT, NOPROTECT, NOLVP, NODEBUG	/*************/

  //#fuses   USBDIV, PLL1, CPUDIV1, PROTECT, NOCPD, noBROWNOUT,HSPLL,NOWDT,nolvp, VREGEN

#use delay(clock=48000000)
// #use delay(clock=4000000)

#use i2c(master, sda=PIN_B0, scl=PIN_B1, FORCE_HW, SLOW)

#define print(A,B)	printf( usb_cdc_putc, (A), (B) )
  
#define LED1			PIN_A5	
#define LED_ON		output_low( PIN_A5 );
#define LED_OFF		output_float( PIN_A5);

#define BUTTON_PRESSED	(!input( PIN_A5 ))
