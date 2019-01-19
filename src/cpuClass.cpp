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
      Serial.begin( baud );
      delay( 500 );
      blink(3);
      PR("\r\nCPU initialized");
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
    void CPU::led( onoff_t onoff, int times ) // defines a led
    {
      if( onoff == 2 )
        blink( times );      
      else // onoff is either 0 or 1
        digitalWrite( ledpin&0x7FFF, (ledpin & NEGATIVE_LOGIC) ? !onoff: onoff );
    }  
    bool CPU::button()
    {
      return (btnpin & NEGATIVE_LOGIC) ? !digitalRead( btnpin&0x7FFF ) : digitalRead( btnpin&0x7FFF );
    }   
    bool CPU::buttonPressed()     // true if button is pressed and released
    {
        static bool justpressed = false;
        if( !justpressed )
        {
            if( button() )
            {
                led( ON );
                justpressed = true;
            }
            return false;
        }
        else // if( justpressed )
        {
            if( button() )  // continuing pressed
                return false;
            justpressed = false;
            led( OFF );
            return true;        // released
        }   
    }
    // Buf CPU::prompt( char *prmpt )
    // {
        // Buf t(80);      // allocation of 80-bytes
        // PN( prmpt );
        // Serial.setTimeout( -1 );
        // size_t n = Serial.readBytesUntil( '\r', t.pntr, 79);
        // t.pntr[n] = 0;
        // PR("");
        // return t;       // buffer is deallocated after exit
    // }
	char * CPU::prompt( char *prmpt )
    {
        PN( prmpt );
		memset( temp, 0, MAX_PROMPT );
        Serial.setTimeout( -1 );
        size_t n = Serial.readBytesUntil( '\r', temp, MAX_PROMPT-1);
        temp[MAX_PROMPT-1] = 0;
        PR("");
        return temp;       // buffer is deallocated after exit
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
			digitalWrite( LED, flip );
			if( flip ) flip=false; else flip=true;
			//ESP.wdtDisable();
			//ESP.wdtFeed();
			yield();
			delay(200);
		}
	}

// ---------------- Buf Class Definition --------------------------------------
	void Buf::alloc( int siz )		// helper routine to allocate buffer
	{
		if( siz <= BUFSTK_SIZE )
		{
			pntr = defbuf;
			maxsiz = BUFSTK_SIZE;
		}	
		else
		{
			siz = (siz+15) & 0xFFF0;	// round up to 16 byte boundary
			if( siz < BUFDEF_SIZE )
				siz = BUFDEF_SIZE;
			pntr = new char[siz];
			maxsiz = siz;	
		}	
		//PF("Allocated %d\r\n", maxsiz );
	}
	Buf::Buf( int siz )
    {
		alloc( siz );
		*pntr = 0;
    }
	Buf::Buf( char *p, int siz )
    {
        int N = strlen( p )+1;
		if( siz < N )
			siz = N;
		alloc( siz );
		strcpy( pntr, p );
    }
    Buf::~Buf()
    {
        free();
    }
	void Buf::free()
	{
		if( pntr && (maxsiz > BUFSTK_SIZE) )
        {
            delete [] pntr;
            pntr = NULL;
        }
        maxsiz = 0;
		//PF("Deallocated\r\n" );
	}
	void Buf::init()
	{
		if( pntr )
			*pntr = 0;
	}
    Buf & Buf::operator = ( char *s ) 
    {
        if( pntr )
        {
			strncpy( pntr, (const char*)s, maxsiz-1 );		
			*(pntr+maxsiz-1) = 0;
		}    
        return *this;
    }
    Buf & Buf::operator = ( const Buf &s ) 
    {
        if( this == &s )
            return *this;
        if( pntr )
        {
			strncpy( pntr, s.pntr, maxsiz-1 );		
			*(pntr+maxsiz-1) = 0;
		}  
        return *this;
    }
    Buf & Buf::operator += ( char *s ) 
    {
        if( pntr )
        {
            int L=strlen( pntr );   // current size
            strncpy( pntr+L, s, maxsiz-L-1 ); // copy at the end of the Buf safely
		}
        return *this;
    }
    char *Buf::set( const char *format, ... )
    {
        if( pntr )
        {
            va_list args;
            va_start (args, format );
            char *p = pntr;             // where to store new data
            int avail = maxsiz - 1;
            vsnprintf(p, avail, format, args);
            va_end( args );
        }
        return pntr;
    }
    char *Buf::set( const Buf &s )
    {
        if( pntr )      
        {
			strncpy( pntr, s.pntr, maxsiz-1 );        // copy string to this buffer safely
			*(pntr+maxsiz-1) = 0;
		}    
        return pntr;
    }
    char *Buf::add( const char *format, ... )
    {
        if( pntr )
        {
            va_list args;
            va_start (args, format );
            char *p = pntr + strlen( pntr );    // where to store new data
            int avail = maxsiz - 1 - strlen( pntr );
            vsnprintf(p, avail, format, args);
            va_end( args );
        }
        return pntr;
    }
	char *Buf::quotes()
    {
        if( pntr )
        {
            for( char *p = pntr; *p; p++ )
			{	
				if( *p=='\'' )
					*p= '\"';
			}
        }
        return pntr;
    }
    char *Buf::add( Buf &s )
    {
        if( pntr )
        {
            int L=strlen( pntr );   // current size
            strncpy( pntr+L, s.pntr, maxsiz-L-1 ); // copy at the end of the Buf safely
        }
        return pntr;
    }
    void Buf::print( const char *prompt )
    {
        PN( prompt );   // print prompt
        if( pntr )      // ...then string
            PN( pntr );
        PR("");         // ...then CRLF  
    }
    void Buf::print()
    {
        if( pntr )
            PN( pntr );
    }
    char * Buf::operator ! ()    // pointer to buffer
    {
        return pntr;
    }
    char *Buf::c_str()
    {
        return pntr;
    }
    int Buf::length()
    {
        return strlen( pntr );
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
