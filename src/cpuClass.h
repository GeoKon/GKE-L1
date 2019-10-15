#pragma once

    #include <ESP8266WiFi.h>
    #include <macros.h>
	
    #define ASSERT(e) if( !(e) ) die( #e, "",__LINE__,__FILE__ )
    void die( const char *s1, const char *s2, int s3, const char *s4 );

// ------- DEFAULT HARDWARE DEFINITIONS FOR NodeMCU -------------------------------

    #define NODEMCU_LED             16       // PIN1 D0 GPIO16    NEGATIVE LOGIC
    #define NODEMCU_BUTTON           0       // PIN4 D3 GPIO0 (also push-button)
	
	#define NEGATIVE_LOGIC 0x8000
	#define POSITIVE_LOGIC 0
	#define MAX_PROMPT 80

	enum onoff_t
	{
	  ON = 1,
	  OFF = 0,
	  NO_LED = 0,
	  USE_LED = 1,
	  BLINK = 2 
	};	
	
	class CPU     // generic to any Node CPU
	{
	public:

		void init(  int baud=115200, int ledp=NODEMCU_LED+NEGATIVE_LOGIC, int button=NODEMCU_BUTTON+NEGATIVE_LOGIC );
		void blink( int times );
		void led( onoff_t onoff, int times=1 ); // defines a led
		int getLedPin() {return ledpin;}
		void toggleEvery( uint32_t ms );		// toggles LED. Non-blocking
		
		bool button();
	/*
	Usage:
		if( k = buttonReady() )
			... use k=1, 2, 3
		if( buttonReady()==1 )
			... just button pressed down
		if( buttonReady()>1 )
			... just button up.
	*/
		bool buttonPressed( onoff_t = USE_LED );		
		int buttonReady( uint32 tmlong = 1000, onoff_t = USE_LED );					// if key is pessed and released, returns 0, 1, 2

		void die( char *prompt="Dead!", int times=1 );

		char *prompt( char *prmpt );

		void heapUpdate();
		uint32_t heapUsedMax();
		uint32_t heapUsedNow();

	private:
		int  ledpin;      			// top bit is 1 if negative logic
		int  btnpin;
		
		char temp[ MAX_PROMPT ];	// temp buffer used for prompts
		uint32_t heapref;     		// heapsize after init();
		uint32_t heapused, heapmax;
	};

char *sf( char *s, size_t L, const char *format, ... );
#define SF(A,F,...) sf( A, sizeof(A), F, ##__VA_ARGS__ )

// ------------------------------ CONOUT PIPE --------------------

class COUT
{
    char format[8];		// float point format
    char *cr="\r\n";
    bool cont;

public:
    COUT( const char *fo=" %.3f" );		// default floating point format
    COUT & operator << ( double x );
    COUT & operator << ( int x );
    COUT & operator << (char *s); 
    COUT & operator << ( byte c );		// prints in hex 
};
