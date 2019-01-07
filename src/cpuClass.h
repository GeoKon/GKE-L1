    #ifndef CPUCLASS_H
    #define CPUCLASS_H

    #include <ESP8266WiFi.h>
    
    #define ASSERT(e) if( !(e) ) die( #e, "",__LINE__,__FILE__ )
    void die( const char *s1, const char *s2, int s3, const char *s4 );

/*--------------------------------------------------------------------------
##                  Frequent Macros

- **ASSERT(condition)** aborts if not true
- **PR( text )** prints text with CRLF at the end
- **PN( text )** prints text as is
- **PF( format, ...)** same as printf()
---------------------------------------------------------------------------- */

    #define PN(A)   Serial.print(A)
    #define PR(A)   Serial.println(A)
    #define PF      Serial.printf         // use this for best WEB IDE compatibility
    #define CRLF()  Serial.println("")
    
    /* --------------------------------------------------------------------------------
##                 Buffer Class

- **Buf s(size)** to declare a new buffer area. **size** defaults to 80 characters
- **s.pntr** to access the allocated buffer. Same is **!s**. Same as **s.c_str()**
- **s.free()** to free this buffer
- **s.length()** same as strlen( !s )
- **s.maxsiz** max size of allocated buffer
- **s.clear()** zero's contents
- . 
- **s=string** sets s equal to **string**
- **s+=string** appends **string**
- **s.set( format, ...)** formats and sets contents
- **s.add( format, ... )** formats and adds contents
- **s.print()** prints current buffer. Analogous to PN()

-------------------------------------------------------------------------------- */

class Buf
{
  public:
    char *pntr;
    int maxsiz;
    
    Buf( int siz=64 )
    {
        pntr = new char[siz];
        //PF("Allocated %d\r\n", siz );
        maxsiz = siz;
    }
    ~Buf()
    {
        if( pntr )
        {
            delete [] pntr;
            pntr = NULL;
        }
        maxsiz = 0;
    }
    void init()
    {
        if( pntr )
            *pntr = 0;
    }
    void free()
    {
        if( pntr )
        {
            delete [] pntr;
            pntr = NULL;
        }
        maxsiz = 0;   
    }
    Buf & operator = ( char *s ) 
    {
        if( pntr )
            strcpy( pntr, (const char*)s );
        return *this;
    }
    Buf & operator = ( const Buf &s ) 
    {
        if( this == &s )
            return *this;
        if( pntr )
            strcpy( pntr, s.pntr );
        return *this;
    }
    Buf & operator += ( char *s ) 
    {
        if( pntr )
            strcat( pntr, (const char*)s );
        return *this;
    }
    char *set( const char *format, ... )
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
    char *set( const Buf &s )
    {
        if( pntr )      
            strncpy( pntr, s.pntr, maxsiz );        // copy string to this buffer safely
        return pntr;
    }
    char *add( const char *format, ... )
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
    char *add( Buf &s )
    {
        if( pntr )
        {
            int L=strlen( pntr );   // current size
            strncpy( pntr+L, s.pntr, maxsiz-L-1 ); // copy at the end of the Buf safely
        }
        return pntr;
    }
    void print( const char *prompt )
    {
        PN( prompt );   // print prompt
        if( pntr )      // ...then string
            PN( pntr );
        PR("");         // ...then CRLF  
    }
    void print()
    {
        if( pntr )
            PN( pntr );
    }
    char * operator ! ()    // pointer to buffer
    {
        return pntr;
    }
    char *c_str()
    {
        return pntr;
    }
    int length()
    {
        return strlen( pntr );
    }
};
  
typedef unsigned long ulong;

// ------- DEFAULT HARDWARE DEFINITIONS FOR NodeMCU -------------------------------

    #define LED             16       // PIN1 D0 GPIO16    NEGATIVE LOGIC
    #define BUTTON           0       // PIN4 D3 GPIO0 (also push-button)
    
/* --------------------------------------------------------------------------------
##                 CPU Class

- **CPU cpu** class allocation
- **cpu.init( baud, led, button )** initialzies serial port
- **cpu.led( BLINK|ON|OFF, times)** controls the LED
- **cpu.button()** true or false depending if button is pressed
- **cpu.buttonPressed()** true if pressed; waits until released
- **cpu.prompt()** prints a prompt and waits for string response
- .
- **cpu.heapUpdate()** updates heap max
- **cpu.heapUsedNow()** indicates amount of heap used
- **cpu.heapUsedMax()** indicates max amount of heap used

-------------------------------------------------------------------------------- */

enum reset_t
{
  AUTO_RESET = true,
  NO_RESET   = false
};

#define NEGATIVE_LOGIC 0x8000

enum onoff_t
{
  ON = 1,
  OFF = 0,
  BLINK = 2 
};

class CPU     // generic to any Node CPU
{
  int  ledpin;      // top bit is 1 if negative logic
  int  btnpin;

  uint32_t heapref;     // heapsize after init();
  uint32_t heapused, heapmax;
  
  public:
    void init(  int baud=115200, int ledp=LED+NEGATIVE_LOGIC, int button=BUTTON+NEGATIVE_LOGIC ) {  
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
    void blink( int times )
    {
      for( int i=0; i<times; i++ )
      {
        digitalWrite( ledpin&0x7FFF, (ledpin & NEGATIVE_LOGIC) ? OFF: ON );
        delay(100);
        digitalWrite( ledpin&0x7FFF, (ledpin & NEGATIVE_LOGIC) ? ON: OFF );
        delay(200);
      }
    } 
    void led( onoff_t onoff, int times=1 ) // defines a led
    {
      if( onoff == 2 )
        blink( times );      
      else // onoff is either 0 or 1
        digitalWrite( ledpin&0x7FFF, (ledpin & NEGATIVE_LOGIC) ? !onoff: onoff );
    }  
    bool button()
    {
      return (btnpin & NEGATIVE_LOGIC) ? !digitalRead( btnpin&0x7FFF ) : digitalRead( btnpin&0x7FFF );
    }   
    bool buttonPressed()     // true if button is pressed and released
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
    void die( char *prompt="Dead!", int times=1 )
    {
      PF( "%s Heap used=%d (max used=%d)\r\n", prompt, heapUsedNow(), heapUsedMax() );
      for(;;)
      {
        blink( times );
        delay( 500 );
      }
    }
	char *prompt( char *prmpt, char *buf, int siz ) 
    {
        if( prmpt ) PF( "%s", prmpt );
        Serial.setTimeout( -1 );
        
        static char temp[32];
        if( buf==NULL )
        {
            buf = temp;
            siz = sizeof( temp );
        }
        size_t n = Serial.readBytesUntil( '\r', buf, (size_t) siz );
        buf[n] = 0;
        PR("");
        return buf;
    }
    Buf prompt( char *prmpt )
    {
        Buf t(80);      // allocation of 80-bytes
        PN( prmpt );
        Serial.setTimeout( -1 );
        size_t n = Serial.readBytesUntil( '\r', t.pntr, 79);
        t.pntr[n] = 0;
        PR("");
        return t;       // buffer is deallocated after exit
    }
    void heapUpdate()
    {
      uint32_t heapnow = ESP.getFreeHeap();     // heap will decrease as used
      heapused = heapref - heapnow;
      if( heapused > heapmax )
        heapmax = heapused;
    }
    uint32_t heapUsedMax()
    {
      heapUpdate();
      return heapmax;
    }
    uint32_t heapUsedNow()
    {
      heapUpdate();
      return heapused;
    }
};

/* --------------------------------------------------------------------------------
##                 Console Pipe

- **COUT P** to allocate a new pipe stream
- **P | ""** streams CR/LF at the end of a stream
- **P | "literal" | ""** streams literal with space before, unless 1st
- **P | 10 | ""** streams integer with space before
- **P | 1.23 | ""** streams float with space before
- **P | c | ""** streams character in HEX with space before

-------------------------------------------------------------------------------- */

class COUT
{
    char format[8];
    char *cr="\r\n";
    bool cont;

public:
    COUT( const char *fo=" %.3f" )
    {
        strcpy( format, fo);
        cont = false;
    }
    COUT & operator | ( double x ) 
    {
        PF( format, x );
        cont = true;
        return *this;
    }
    COUT & operator | ( int x ) 
    {
        PF( " %d", x );
        cont = true;
        return *this;
    }
    COUT & operator | (char *s) 
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
    COUT & operator | ( byte c ) 
    {
        PF(" %02X",  (int)c );
        cont = true;
        return *this;
    }
};

extern CPU cpu;       
#endif
