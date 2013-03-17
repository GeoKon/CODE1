#define STATE_ON_AC		0
#define STATE_ON_BAT	1

struct _ahr
{
	BYTE	state;			// one of the above states
	WORD	outages;		// counts the number of outages since "ahr_reset()"
	BYTE	tmin;			// minute counter
	WORD	thrs;			// hour counter
	BYTE	time;			// time stamp HRS(12) MIN5(4)

	WORD	tim0;			// valid is battery is actually discharging
	BYTE	min0;			// number of minutes the battery is discharging 0...59
	BYTE	hrs0;			// number of hours the battery is discharging (up to 10 days)
	float 	AH0;			// AH used thus far

	WORD	tim[4];			// time stamp
	BYTE	min[4];			// number of minutes the battery is discharging 0...59
	BYTE	hrs[4];			// number of hours the battery is discharging (up to 10 days)
	float 	AH[4];			// AH used thus far

	float	batV;			// average battery voltage within a minute
	float	batI;			// average battery current within a minute

} ahr;

void ahr_push()
{
	byte i,j,j1;
	for(i=0;i<3;i++)
	{
		j  = 3-i;
		j1 = j-1;
		ahr.tim[j] = ahr.tim[j1];
		ahr.min[j] = ahr.min[j1];
		ahr.hrs[j] = ahr.hrs[j1];
		ahr.AH [j] = ahr.AH [j1];
	}
	ahr.tim[0] = ahr.tim0;
	ahr.min[0] = ahr.min0;
	ahr.hrs[0] = ahr.hrs0;
	ahr.AH [0] = ahr.AH0;
}

/*	eeprom

	duration of last outage	-- updated after AC restoration or Vbat<20V
	battery capaciy used during last outage -- updated after AC is restored or Vbat <20V
	number of outages -- when flip from AC to BAT
	average battery current while on-battery, just before AC restoration
	
	stats:

	ah.min_onAC = 0.0;		// not implemented
	// outages 0...1h
	//         1...2h
	//	...
	//		  23..24H
	//		  >24H
*/

void ahr_reset()
{
	ahr.state=STATE_ON_AC;	// initial state
	ahr.outages = 0;
	ahr.tmin =0;			// minute counter
	ahr.thrs =0;			// hour counter

	ahr.min0 = 0;
	ahr.hrs0 = 0;

	ahr.tim[0]=0;			// time stamp
	ahr.tim[1]=0;			// time stamp
	ahr.tim[2]=0;			// time stamp
	ahr.tim[3]=0;			// time stamp
}

void ahr_check()					// checks voltages and spins the state machine
{
	// COMPUTE TIME STAME
	ahr.tmin++;						// increment minute counter
	if( ahr.min >= 60 )				// hours
	{
		ahr.thrs++;
		ahr.tmin=0;
	}
	ahr.time = (ahr.thrs<<4) + ahr.tmin/4;

	if( ahr.state == STATE_ON_AC )
	{
		if( adc.ave2 > 512 )		// if PS is ON
			return;					// do nothing
									
		// here, PS is detected to go off. We transition to batteries and start counting AH
		
		ahr.state = STATE_ON_BAT;	// flip to battery
		ahr.AH0  = 0.0;				// zero AH accumulator
		ahr.batV = adc.BT;			// reset battey average current
		ahr.batI = adc.Ic;			// reset battey average current
		ahr.min0 = 0;				// zero minutes
		ahr.hrs0 = 0;				// zero hours
		return;	
	}
	else // if( ahr.state == STATE_ON_BAT )
	{
		if( adc.ave2 <= 512 )		// if PS is still OFF
		{
			ahr.batV = (0.9)*ahr.batV + (0.1)*adc.BT;
			ahr.batI = (0.9)*ahr.batI + (0.1)*adc.Ic;
			return;					// do nothing
		}
		// here, PS is detected to go ON. We transition from batteries to PS

		ahr_push();
		ahr.state = STATE_ON_AC;	// flip to AC
	}
}

void ahr_update()					// called every minute to update the AH counters
{
	if( ahr.state == STATE_ON_AC )
		return;
	
	// come here every minute with battery discharging

	ahr.min0++;						// increment number of minutes discharging
	if( ahr.min0 >= 60 )			// hours?
	{
		ahr.hrs0++;
		ahr.min0=0;
	}	
	ahr.AH0 += (ahr.batV*ahr.batI)/(24.0 * 60.0);
}

