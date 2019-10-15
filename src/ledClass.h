#pragma once
#include <cpuClass.h>

#define LED_EXTENDED		// use for conditional compilation

/*
	setPin();				// use default LED
	
	setCount( 5 );		
	while( blink() )		// blink 5 times. 0=forever
		;

	setDuration( 5 );	
	while( blink() )		// blink for 5-seconds. 0=forever
		;
	
	setPattern("*...", [5] )
	while( blink() )		// blink this pattern for 5-seconds (or forever)
		;
	
	deferPrompt("...");		// show prompt with the first call of blink()
	stop();					// forced stop blinking
*/
enum tmoMode_t
{
	STOP    = 0,
	FOREVER = 3,
	COUNTER = 1,
	TIMER   = 2	
};

class LED
{
private:
							// ---- Used by blinker() -------------
	const char *ptrn;		// Set by setPattern(). Must point to a constant string
	int item;				// item 0... of the pattern string. Initialized by init();
	int state;				// state machine. Initialized by init();
	unsigned long T0;		// millis() of the beginning of each item in the pattern
	int ledpin;				// input-output pin. Set by setPin()
	bool active;			// true if blink has been previously called

	const char *dprompt;	// deferred prompt. Set by deferPrompt()

	bool prndots;			// prints dots while blinking. 
							// Set by pattern() and persistent thereafter
							
	int dotcount;			// Set by init(). Used by handleDots()
	unsigned long Tdot;		// timer used to display dots
	void handleDots();		// prints does to serial port
	
	tmoMode_t mode;			// mode of operation	
	void init( bool once=false);	// initializes blnker() variables

	
public:
	void pin( int iopin=NODEMCU_LED+NEGATIVE_LOGIC );

	void on();
	void off();
	void deferPrompt( const char *prompt );	
	
	void pattern( const char *s, int secs=0 ); 	// sets pattern. Must be a const

	bool blinker( bool C);	// main routine to blink
	void stop();
	bool blinking();
	
#ifdef LED_EXTENDED

private:
	int falsecode;			// 1=condition false or stop, 2=timeout
	int  ncount;			// depends on mode. Either counter or sec left
	unsigned long Tstart;	// millis() of 1st blink
	unsigned long Tend;		// expected ending millis()

public:
	// ---- One of the following must be called first --------
    // pattern() OR...
	void count( int c );	// number of times to display pattern
	void tmout( int sec );	// number of seconds to display pattern

	bool blink( bool condition = true );
	bool expired();
	bool aborted();

#endif
};

