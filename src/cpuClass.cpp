// file: cpuClass.cpp

#include "cpuClass.h"

// ------- DEFAULT HARDWARE DEFINITIONS FOR NodeMCU -------------------------------

    void CPU::init(  int baud, int ledp, int button ) 
	{  
      ledpin   = ledp;
      pinMode( ledpin&0x7FFF, OUTPUT);
      
      btnpin = button;
      pinMode( btnpin&0x7FFF, (btnpin & NEGATIVE_LOGIC) ? INPUT_PULLUP : INPUT );

      led( OFF );
      
	  pribaudrate = secbaudrate = baud;
	  Serial.begin( baud );
      pinswap = false;
	  oenpin  = 0;
	  
	  delay( 500 );
      blink(3);
      PRN("\r\nCPU initialized");
      heapref = ESP.getFreeHeap();
      heapmax = 0;
    }
    void CPU::blink( int times )
    {
      for( int i=0; i<times; i++ )
      {
        digitalWrite( ledpin&0x7FFF, (ledpin & NEGATIVE_LOGIC) ? OFF: ON );
        delay(100);
        digitalWrite( ledpin&0x7FFF, (ledpin & NEGATIVE_LOGIC) ? ON: OFF );
        delay(200);
      }
    } 
	void CPU::toggleEvery( uint32_t ms )
    {
    // Call repeately. Does not block. Blinks ms-ON, ms-OFF.
		static uint32_t T0=0;
        static bool toggle = false;
        
        if( millis()-T0 > ms )     // most likely true the first time
        {
            toggle = !toggle;
            led( (onoff_t) toggle );
            T0 = millis();          
        }
    }
    void CPU::led( onoff_t onoff, int times ) // defines a led
    {
	  if( onoff == 2 )
        blink( times );      
      else // onoff is either 0 or 1
        digitalWrite( ledpin & 0x7FFF, (ledpin & NEGATIVE_LOGIC) ? !onoff: onoff );
    }  
    bool CPU::button()
    {
      return (btnpin & NEGATIVE_LOGIC) ? !digitalRead( btnpin&0x7FFF ) : digitalRead( btnpin&0x7FFF );
    }   
/*
	bool CPU::buttonPressReady()
    {
        static bool justpressed = false;

        if( button() && (!justpressed) )
        {
            led( ON );
            delay( 20 );                                // debounce
            if( button() )
            {
                justpressed = true;
				return true;    
            }
        }
        else // either justpressed, or !button...       // wait until depressed
        {
            if( !button() )
            {
                justpressed = false;
                led( OFF );
            }
        }
        return false;
    }
*/
	bool CPU::buttonPressed( onoff_t L )
	{
		return buttonReady( 500, L )==1;
	}
	int CPU::buttonReady( uint32 tmlong, onoff_t L )	
    {
        static bool justpressed = false;
		static unsigned long T1;
		
        if( button() && (!justpressed) )
        {
            if( L==USE_LED ) led( ON );
            delay( 20 );                                // debounce
            if( button() )
            {
                justpressed = true;
                T1 = millis();
            }
			return 1;
        }
		if( justpressed )					
		{
			if( button() )					// continue pressing?
			{
				if( millis()>T1+tmlong )
					if( L==USE_LED ) led( OFF );
			}
			else							// button is released
			{
				delay( 10 );				// debounce again
				if( !button() )
				{	
					justpressed = false;
					if( L==USE_LED ) led( OFF );
					int id = (millis()>T1+tmlong) ? 3 : 2;
					return id;
				}
			}
		}
		return 0;
    }

	char * CPU::prompt( char *prmpt )
    {
        PR( prmpt );
		memset( temp, 0, MAX_PROMPT );
        Serial.flush();
		int i=0, c=0;
		for(;;)
		{
			if( Serial.available() && (c = Serial.read())>0 )
			{
				if( (c=='\r') || (i>=MAX_PROMPT-2) ) 
					break;
				temp[i++] = c;	
				PR( (char) c);
			}
			yield();
		}
		temp[i]=0;
		CRLF();
        return temp;       
    }
    void CPU::heapUpdate()
    {
      uint32_t heapnow = ESP.getFreeHeap();     // heap will decrease as used
      heapused = heapref - heapnow;
      if( heapused > heapmax )
        heapmax = heapused;
    }
    uint32_t CPU::heapUsedMax()
    {
      heapUpdate();
      return heapmax;
    }
    uint32_t CPU::heapUsedNow()
    {
      heapUpdate();
      return heapused;
    }
    void CPU::die( char *prompt, int times )
    {
      PF( "%s Heap used=%d (max used=%d)\r\n", prompt, heapUsedNow(), heapUsedMax() );
      for(;;)
      {
        blink( times );
        delay( 500 );
      }
    }
	void die( const char *s1, const char *s2, int s3, const char *s4 )
	{
		PF( "ASSERT(%s), line %d of %s\r\n", s1, s3, s4 );
		bool flip;
		for(;;)
		{
			digitalWrite( NODEMCU_LED, flip );
			if( flip ) flip=false; else flip=true;
			//ESP.wdtDisable();
			//ESP.wdtFeed();
			yield();
			delay(200);
		}
	}
	void CPU::flushRcv()					// flush receiving buffer
    {
        uint32 T0 = millis();
        while( millis() < T0+10 )   		// 10ms flushing
            if( Serial.available() )
                Serial.read();            
    }	
	void CPU::setSecBaud( int baud, int oepin )
	{
		secbaudrate = baud;
		oenpin = oepin;
		if( oenpin > 0 )					// initialize port
		{
			pinMode( oenpin, OUTPUT ); 		// typically GPIO12 = D6. Used to OE the tranlator
			digitalWrite( oenpin, (oenpin & NEGATIVE_LOGIC) ? HIGH : LOW );
		}
	}
	void CPU::swapSerialSec()
	{
		if( pinswap )						// already in SEC
			return;
		pinswap = true;
		Serial.flush();             		// empty Primary transmit buffer
		
		if( pribaudrate != secbaudrate )
		{
			Serial.end();                   // change baud rate
			Serial.begin( secbaudrate );    // change baudrate to Primary
		}
		Serial.swap();              		// swap pins
		
		if( oenpin > 0 )
			digitalWrite( oenpin, (oenpin & NEGATIVE_LOGIC) ? LOW : HIGH);    // enabe translator

		flushRcv();					 		// empty Secondary receive buffer
	}
	void CPU::swapSerialPri()
	{        
		if( !pinswap )						// already in PRI
			return;
		Serial.flush();             		// empty Secondaary transmit buffer

		if( oenpin > 0 )
			digitalWrite( oenpin, (oenpin & NEGATIVE_LOGIC) ? HIGH : LOW);    // enabe translator
		
		Serial.swap();              		// swap pins
		if( pribaudrate != secbaudrate )
		{
			Serial.end();                   // Stop
			Serial.begin( pribaudrate );   	// change baudrate of Primary
		}
		flushRcv();							// empty Primary receive buffer
		pinswap = false;
	}
	bool CPU::serialSwapped()
	{
		return pinswap;
	}

char *sf( char *sp, size_t sL, const char *format, ... )
{
	va_list args;
	va_start (args, format );
	vsnprintf( sp, sL-1, format, args );
	*(sp+sL-1) = 0;						// EOS
	va_end( args );
	return sp;
}

// -------------------------- PIPE OUT ----------------------------------------

    COUT::COUT( const char *fo )
    {
        strcpy( format, fo);
        cont = false;
    }
    COUT & COUT::operator << ( double x ) 
    {
        PF( format, x );
        cont = true;
        return *this;
    }
    COUT & COUT::operator << ( int x ) 
    {
        PF( " %d", x );
        cont = true;
        return *this;
    }
    COUT & COUT::operator << (char *s) 
    {
        if(*s==0)
        {
            PF( cr );
            cont = false;
            return *this;
        }
        if(*s==',')
        {
            PF( "%s", s );
        }
        else
        {
            if( cont )
                PF(" %s",  s );
            else
                PF( "%s", s );
        }
        cont = true;
        return *this;
    }
    COUT & COUT::operator << ( byte c ) 
    {
        PF(" %02X",  (int)c );
        cont = true;
        return *this;
    }
