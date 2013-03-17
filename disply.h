BYTE selection;
static WORD tics1;

void disp_bar( WORD i )
{
	byte c;
	i = i/128;			// now i=0...7
	for( c=0; c<i; c++ )
		display1("%c", 4 );
}

char symb( WORD x )
{
	if( x<4 )
		return '<';
	if( x>1020 )
		return '>';
	return '=';
}

void disp_values()
{
	switch( selection )
	{
		default:
		case 0:
				display1( "\fVp=%ld\n", adc.ave2 );
				display1(   "Vb=%ld " , adc.ave1 );
				break;
		case 1:
				display2( "\fP%c%5.2fV\n", symb(adc.ave2), adc.PS );
				display2(   "B%c%5.2fV" ,  symb(adc.ave1), adc.BT );
				break;
		case 2:
				display1( "\fIc=%ld\n", adc.ave3 );
				display1(   "Th=%ld " , adc.ave3 );
				break;
		case 3:
				display1( "\fI=%4.2fA\n", adc.Ic );
				display1(   "T=%4.1fC" ,  adc.Ic );
				break;

		case 4:
				display1( "\fPOT=%ld\n", read_Pot() );
				disp_bar( read_Pot() );
				break;
		case 5:
				display2( "\fS%d %4.2fV\n", ahr.state, ahr.batV );
				display2( "%02u:%02u", ahr.hrs0, ahr.min0 );
				break;
		case 6:
				display1( "\f%5.2fAH\n", ahr.AH0 );
				display2( "%02u:%02u", ahr.hrs0, ahr.min0 );
				break;

		case 7:
				if( ahr.state == STATE_ON_AC )
				{
					display2( "\fB%c%5.2fV\n", symb(adc.ave1), adc.BT );
					display1(   "I=%5.2fA" ,   adc.Ic );
				}
				else
				{
					display2( "\f%2.1f%3.0fAH\n",  ahr.batI, ahr.AH0 );
					display2( " %02u:%02u", ahr.hrs0, ahr.min0 );
				}			
				break;
				break;

#define SELECT_MAX 7		
	}
}

void disp_select( BYTE pb )
{
	if( pb==1 )
	{
		selection--;
		if( selection > SELECT_MAX )
			selection = SELECT_MAX;
	}
	if( pb==2 )
	{
		selection++;
		if( selection > SELECT_MAX )
			selection = 0;
	}
}	
