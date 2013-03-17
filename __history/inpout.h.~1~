#define LED1ON	output_high(pLD1)
#define LED1OFF	output_low (pLD1)

#define LED2ON	output_high(pLD2)
#define LED2OFF	output_low (pLD2)

WORD read_Pot()
{
	WORD a;
	set_tris_b(0b00001000 );						// 1=INPUT, 0=OUTPUT
	setup_adc_ports(  AN0_TO_AN9 );					// reference 0...Vcc
	set_adc_channel( 9 ); 
	delay_us( 10 );
	a = read_adc();
	setup_adc_ports(  AN0_TO_AN9 | VREF_VREF );		// restore the default
	set_tris_b(0b00000100 );						// DEFAULT
	return a;
}
WORD read_Th()
{
	set_adc_channel( 8 ); 
	delay_us( 10 );
	return read_adc();
}
WORD read_V1( )			// BAT
{
	set_adc_channel( 0 ); 
	delay_us( 10 );
	return read_adc();
}
WORD read_V2( )			// PS
{
	set_adc_channel( 1 ); 
	delay_us( 10 );
	return read_adc();
}
WORD read_Ic( )
{
/*
	set_adc_channel( 4 ); 
	delay_us( 10 );
	return read_adc();
*/
	return read_Pot();
}

#define BUTTON1 (input( pPB1 ))
#define BUTTON2 (input( pPB2 ))


/* usage:
	if( button_pressed() )
		if( button1 )
			...
	while( button_pressed() )
		;
*/
byte read_button()				// 1,2 or 3 if any key is pressed.
{
	byte i;
	int1 i1, i2;

	set_tris_b(0b11000100 );			// inputs RB7 and RB6
  
	for(i=0;i<10;i++)
   	{
      	i1 = BUTTON1;
		i2 = BUTTON2;
		
		if( i1 || i2 )					// if either is pressed, continue looping
      		delay_ms( 2 );
		else
		{								// if both of them are released, return immediately

			set_tris_b(0b00000100 );	// all outputs
			return 0;
		}
   	}
   	HI( pin_B0 );
	while( BUTTON1 || BUTTON2 )			// wait until released
		;
	LO( pin_B0 );

	set_tris_b(0b00000100 );			// all outputs

	if( i1 && i2 ) 
		return 3;
	if( i2 )
		return 2;
//	if( i1 )
		return 1;
}

