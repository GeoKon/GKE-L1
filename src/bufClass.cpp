#include "macros.h"
#include "bufClass.h"

// ---------------- BASE Buf Class Definition --------------------------------------

	void BUFBASE::init()
	{
		if( maxsiz )
			*pntr = 0;
	}
    char *BUFBASE::copy( const char *s ) 
    {
        if( maxsiz )
        {
			strncpy( pntr, (const char*)s, maxsiz-1 );		
			*(pntr+maxsiz-1) = 0;
		}    
        return pntr;
    }
    char *BUFBASE::set( const char *format, ... )
    {
        if( maxsiz )
        {
            va_list args;
            va_start (args, format );
            vsnprintf(pntr, maxsiz - 1, format, args);
			*(pntr+maxsiz-1) = 0;
            va_end( args );
        }
        return pntr;
    }
    char *BUFBASE::add( const char *format, ... )
    {
        if( maxsiz )
        {
			va_list args;
			va_start (args, format );
			char *p = pntr + strlen( pntr );    		// where to store new data
            int avail = maxsiz - 1 - strlen( pntr );	// left over space
			vsnprintf(p, avail, format, args);
			*(pntr+maxsiz-1) = 0;						// EOS
			va_end( args );
        }
        return pntr;
    }
	char *BUFBASE::quotes()
    {
		int i=0;
		for( char *p = pntr; (*p) && (i<maxsiz); p++, i++ )
		{	
			if( *p=='\'' )
				*p= '\"';
		}
        return pntr;
    }
    void BUFBASE::print( const char *prompt )
    {
		PR( prompt );   				// print prompt
        if( pntr )      				// ...then string
        {
			PR( pntr );
			// if( pntr[ strlen(pntr)-1 ] != '\n' )
				// PR("\r\n");        	// ...then CRLF  
		}
    }
    char * BUFBASE::operator ! ()    	// pointer to buffer
    {
        return pntr;
    }
    char *BUFBASE::c_str()				// pointer to buffer
    {
        return pntr;
    }
    size_t BUFBASE::length()
    {
        return strlen( pntr );
    }
	size_t BUFBASE::size()
    {
        return maxsiz;
    }
	
// --------------- the actual BUF class is below ----------------------------	
	
	void BUF::balloc( int size1, const char *label )
	{
		name   	= label;							// default values of failure
		maxsiz 	= 0;
		allocated = false;
		
		if( size1 <= 0 )
			return;
	
		pntr = (char *) realloc( NULL, size1 );	// used realloc() to be consistent with String
		if( pntr )
		{
			allocated = true;
			maxsiz = size1;
			if( *name )
				PF("Allocated %s[%d]\r\n", name, maxsiz );
			
			*pntr = 0;							// initialize the buffer
		}
		else									
			PF("Allocation of %s[%d] failed\r\n", name, maxsiz );
	}
	BUF::BUF( int size1, const char *label )
    {
        balloc( size1, label );
    }
	BUF::BUF( const char *userstr, int size )	// constant buffer copy. DEPRECATE!!!
    {
        balloc( size, "" );
		copy( userstr );
    }
	BUF::BUF( char *userstr, int size )		// user specified buffer. No alloc!
    {
		pntr   	  = userstr;
        maxsiz 	  = size;
		allocated = false;
		name      = "";
		*pntr = 0;		
    }
    BUF::~BUF()
    {
        if( maxsiz )					// this checks if pointer is valid. Required!
		{
			maxsiz = 0;
			if( allocated )				// deallocate buffer only if allocated previously.
				free( pntr );
			if( *name )
				PF("Deallocated %s\r\n", name );
		}
    }
    BUF & BUF::operator = ( const char *s ) 
    {
        copy( s );
        return *this;
    }
