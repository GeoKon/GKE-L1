#include "ledClass.h"
#include "cpuClass.h"

	void LED::pin( int iopin )
	{
		init( true );
		pinMode( iopin & 0x7FFF, OUTPUT);
		ledpin = iopin;
	}
	void LED::init( bool once )
	{
		if( once )
		{
			ptrn        = "$*..";
			prndots		= false;
		}
		active 		= false;
		item   		= 0;
		state 		= 0;
		dotcount 	= 0;
		dprompt		= "";	
		mode		= STOP;
	}
	void LED::deferPrompt( const char *s )
	{
		dprompt = s;
	}
	void LED::on()
	{
		digitalWrite( ledpin & 0x7FFF, (ledpin & NEGATIVE_LOGIC) ? LOW: HIGH );	
	}
	void LED::off()
	{
		digitalWrite( ledpin & 0x7FFF, (ledpin & NEGATIVE_LOGIC) ? HIGH: LOW );	
	}
	
	// starts the pattern
	void LED::pattern( const char *s, int secs )
	{
		init();
		prndots = (*s=='$');
		ptrn = prndots ? s+1 : s;
		
		if( secs <=0 )
			mode = FOREVER;
		else
			mode = TIMER;
		
		#ifdef LED_EXTENDED
		ncount = secs;				
		#endif
	}

/*	--------------------- blinker() ---------------------------

	Plays the pattern until either
		* condition is false, OR
		* stop() is called 

	After the above, it returns immediately
	Normarly, it returns TRUE, but if one of the above conditions occur
	it returns FALSE
	
	To restart, use pattern()
	
	If the deferPrompt() was previously used, the prompt is printed
	the first time that is called following the pattern()
	
	Displays dots if the pattern starts with $.
*/	
	bool LED::blinker( bool condition )
	{
		if( mode == STOP )
			return false;
		
		if( !active )			// first time called
		{
			active = true;
			if( *dprompt )
				PRN( dprompt );
			
			dotcount = 0;
			Tdot = millis();	// dot timer
		}
		if( !condition )
		{
			stop();
			return false;
		}			
		if( *ptrn==0 )			// do nothing if empty pattern
			return true;

		if( prndots )
			handleDots();

		switch( state )
		{	
			default:	 		
			case 0:	
				T0=millis();
				if(ptrn[item] == '*' )
					on();
				if(ptrn[item] == '.' )
					off();
				state = 1;
				break;
				
			case 1:
				//PF("%d: S1\r\n", millis()/10 );					
				if( (millis()) > (T0+100) )
				{
					off();	
					state = 2;
				}
				break;
				
			case 2:
				item++;
				if( ptrn[item] == 0 )
					item=0;
				state = 0;
				break;
		}
		yield();
		return true;
	}
	void LED::handleDots()
	{
		if( millis() > Tdot+1000L )	// every second
		{
			Tdot = millis();
			PR(".");
			dotcount++;
			if( dotcount > 60 )
			{
				dotcount = 0;
				CRLF();
			}
		}		
	}
	void LED::stop()
	{
		if( *ptrn )
			off();
		if( dotcount > 0 )
			CRLF();
		
		#ifdef LED_EXTENDED
		falsecode = 1;				// 1 is the code of condition=false
		#endif;					// blink() will return false and
								// expired() will return true and
								// reset the falsecode.
		mode = STOP;
	}
	bool LED::blinking()
	{
		return (mode!=STOP);
	}
	
#ifdef LED_EXTENDED
	// one of the setXXX() below must be called first
	void LED::count( int times )
	{
		init();
		if( times <=0 )
			mode = FOREVER;
		else
			mode = COUNTER;
		ncount = times;
	}
	void LED::tmout( int secs )
	{
		init();
		if( secs <=0 )
			mode = FOREVER;
		else
			mode = TIMER;
		ncount = secs;
	}
	
/*	--------------------- blink() ---------------------------

	Blinks() a previous specified pattern, for a given duration or
	number of times. Normally returns TRUE.
	
	Returns FALSE if 
		* timed-out, OR
		* condition is FALSE, OR
		* stopped()
	
	To find out the reason of a FALSE return, use
		* aborted() to check if condition became FALSE or stopped()
		* expired() to check if timeout occured or number of loops completed

	To start, use one of:
		* pattern()
		* tmout()
		* count()
	
	Pattern() is persistent, i.e. if used once, subsequent tmout()
	and count() continue using the same pattern
*/
	bool LED::blink( bool condition )
	{
		bool ok;
		
		if( mode == COUNTER )
		{			
			if( ncount <= 0 )
			{
				stop();
				falsecode = 2;
				return false;
			}	
			else
			{
				ok = blinker( condition );
				if( (state == 0) && (item == 0) )
					ncount--;
				falsecode = ok ? 0 : 1;
				return ok;
			}
		}
		else if( mode == TIMER )
		{
			if( !active )
			{
				Tstart = millis();
				Tend = Tstart + (ncount * 1000L);
			}
			if( millis() > Tend )
			{
				stop();
				falsecode = 2;
				return false;
			}
			else
			{
				ok = blinker( condition );
				falsecode = ok ? 0 : 1;
				return ok;
			}
		}
		else if( mode == FOREVER )
		{
			ok = blinker( condition );
			falsecode = ok ? 0 : 1;
			return ok;
		}	
		else // STOP
		{
			return false;
		}
	}
	bool LED::expired()
	{
		bool retcode = falsecode&2;
		falsecode &= 1;
		return retcode;
	}
	bool LED::aborted()
	{
		bool retcode = falsecode&1;
		falsecode &= 2;
		return retcode;
	}
#endif