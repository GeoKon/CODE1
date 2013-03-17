/*
	48MHz --> each tic is 0.08333us
	Overflow occurs every 5.461ms if DIV by 1
*/

#define OVFL_400ms 	 73			// number of overlows for DISPLAY 
#define OVFL_1sec 	183			// number of overlfows for 1 sec event
#define OVFL_1min  500	//10986		// number of overflows for 1 min event

struct _tm
{
	WORD	loops_400ms;		// counts to up to OVFL_400ms
	WORD	loops_1min;			// counts to up to OVFL_1min
	
	byte	queue[8];			// event queue
	byte	ip;					// producer index into queue 0...7
	byte	ic;					// consumer index

} tm;

#define WRAP_MASK 0x7			// wrap around mask of 8 events depth

#define TIC_ADC  0x01			// every 5ms (appr) or so to start averaging A/D readings
#define TIC_DISP 0x02			// every 400ms or so to drive the display
#define TIC_MIN  0x04			// every one minute (exactly)

// Initializes timebase and all data structures
void tm_init()
{
	tm = 0;						// set all data structure to zero

   	setup_timer_1( T1_INTERNAL | T1_DIV_BY_1  );
   	set_timer1( 0 );
   	enable_interrupts(INT_TIMER1);
}

int1 tm_evready()					// TRUE(1) if something is waiting in the queue
{
	int1 t;
	disable_interrupts(INT_TIMER1);
	t = (tm.ip != tm.ic );
	enable_interrupts(INT_TIMER1);
	return t;
}
byte tm_getev()						// get event from the queue
{
	byte ev;
	disable_interrupts(INT_TIMER1);
	ev = tm.queue[ tm.ic++];
	tm.ic &= WRAP_MASK;
	enable_interrupts(INT_TIMER1);
	return ev;	
}

#INT_TIMER1
void isr1()                  		// come here after timer1 expires.
{
	byte event = 0;

	tm.queue[ tm.ip++ ] = TIC_ADC;		// always enqueue ADC request
	tm.ip &= WRAP_MASK;

	tm.loops_400ms++;				// increment all counters
	tm.loops_1min++;

	if( tm.loops_400ms >= OVFL_400ms )
	{
		tm.queue[ tm.ip++ ] = TIC_DISP;
		tm.ip &= WRAP_MASK;
		tm.loops_400ms = 0;
	}
	if( tm.loops_1min >= OVFL_1min )
	{
		tm.queue[ tm.ip++ ] = TIC_MIN;
		tm.ip &= WRAP_MASK;
		tm.loops_1min = 0;
	}
}
