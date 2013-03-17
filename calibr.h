struct _coef
{
	float m1;
	float m2;
	float a;
	float b;

	float n1;
	float n2;
	float c;
	float d;

} coef;

void calibr_init()
{
	BYTE *bp, i;
	
	if( read_eeprom( EE_V12COEF ) != 0x55 )
	{
		coef.m1 = 158;
		coef.m2 = 883;
		coef.a = 16.0/(coef.m2 - coef.m1);
		coef.b = 12.0/(coef.m2 - coef.m1);

		coef.n1 = 135;
		coef.n2 = 868;
		coef.c = 16.0/(coef.n2 - coef.n1);
		coef.d = 12.0/(coef.n2 - coef.n1);

		bp = &coef;
		for( i=0; i<8*sizeof(float); i++)
		{
			write_eeprom( EE_V12COEF+i, *bp++ );
		}
		write_eeprom( 0, 0x55 );
	}
	bp = &coef;
	for( i=0; i<8*sizeof(float); i++)
	{
		*bp++ = read_eeprom( EE_V12COEF+i );
	}
}
float calc_V1( WORD m )
{
	float x;
	x = m;
	return coef.a*(x-coef.m1) + coef.b*(coef.m2-x);
}
float calc_V2( WORD m )
{
	float x;
	x = m;
	return coef.c*(x-coef.n1) + coef.d*(coef.n2-x);
}
float calc_Ic( WORD m )
{
	float x;
	x = m;
	return x/102.4;
}
void calibr_V12()
{
	WORD m1, m2;
	BYTE *bp, i;

	display("\fCalibrat\n");
	display(" ADC V1");
	delay_ms(2000);

	for(;;)
	{
		m1 = read_V1();
		display1("\fSet 12V1\n");
		display1("m1=%ld", m1 );
		if( read_button() == 1 )
			break;
		delay_ms(200);
	}
	for(;;)
	{
		m2 = read_V1();
		display1("\fSet 16V1\n");
		display1("m2=%ld", m2 );
		if( read_button() == 1 )
			break;
		delay_ms(200);
	}
	coef.m1 = m1;
	coef.m2 = m2;
	coef.b = 12.00/(coef.m2-coef.m1);
	coef.a = 16.00/(coef.m2-coef.m1);

	display("\fCalibrat\n");
	display(" ADC V2");
	delay_ms(2000);

	for(;;)
	{
		m1 = read_V2();
		display1("\fSet 12V2\n");
		display1("m1=%ld", m1 );
		if( read_button() == 1 )
			break;
		delay_ms(200);
	}
	for(;;)
	{
		m2 = read_V2();
		display1("\fSet 16V2\n");
		display1("m2=%ld", m2 );
		if( read_button() == 1 )
			break;
		delay_ms(200);
	}
	coef.n1 = m1;
	coef.n2 = m2;
	coef.d = 12.00/(coef.n2-coef.n1);
	coef.c = 16.00/(coef.n2-coef.n1);

	display("\f Done! ");
	bp = &coef;
	for( i=0; i<8*sizeof(float); i++)
	{
		write_eeprom( EE_V12COEF+i, *bp++ );
	}
	delay_ms(1000);
}