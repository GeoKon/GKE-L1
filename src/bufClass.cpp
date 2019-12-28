#include "macros.h"
#include "bufClass.h"

/* ---------------- BASE Buf Class Definition --------------------------------------
 *
 * basep[ prindex ] (aka pntr) is the base location to COPY or SET and new string
 * If string does not fit, prindex is wrapped around to zero.
 * 
 * SET places strings at pntr. Number of available bytes for SET is 'maxsiz-prindex'
 * 
 * ADD places strings at 'prindex+strlen( pntr )' overwriting the EOS.
 *   Before adding, the number of available bytes is 'maxsiz-(prindex+strlen(pntr)+1)'
 *
 * APPEND advances the prindex beyond the last EOS but no more than maxsiz-1,
 *   that is, prindex += strlen(pntr)+1
 * ----------------------------------------------------------------------------------
 */
	
	void BUF::balloc( int size1 )
	{
		allocated = false;
		
		if( size1 <= 0 )
			return;	

		basep = (char *) realloc( NULL, size1 );	// used realloc() to be consistent with String
		if( basep )
		{
			allocated = true;
			maxsiz = size1;
			DEBUG( if( *name ) PF("Allocated %s[%d]\r\n", name, maxsiz ) );			
			init();
		}
		else									
		{
			maxsiz = 0;
			PF( "Allocation " );	
			DEBUG( PF("of %s", name ) );
			PF( "[%d] failed\r\n", size1  );				
		}
	}
	BUF::BUF( int size1, const char *label )
    {
        DEBUG( name = label );							// label for this buffer
		balloc( size1 );
    }
	// BUF::BUF( const char *userstr, int size )	// constant buffer copy. DEPRECATE!!!
    // {
        // balloc( size );
		// copy( userstr );
    // }
	BUF::BUF( char *userstr, int size )			// user specified buffer. No alloc!
    {
		basep     = userstr;
        maxsiz 	  = size;
		allocated = false;
		DEBUG( name = "" );
		init();		
    }
    BUF::~BUF()
    {
        if( maxsiz )					// this checks if pointer is valid. Required!
		{
			maxsiz = 0;
			if( allocated )				// deallocate buffer only if allocated previously.
				free( basep );
			DEBUG( if( *name )	PF("Deallocated %s\r\n", name ) );
		}
    }
    BUF & BUF::operator = ( const char *s ) 
    {
        copy( s );
        return *this;
    }
	void BUF::init()					// re-initialize buffer pointers
	{
		if( maxsiz )
		{
			prindex  = 0;				// index of this pointer			
			basep[prindex] = 0;
		}
	}
	void BUF::append()	
	{
		char *pntr = basep+prindex;
		int usedsz = prindex+strlen(pntr)+1;
		
		if( usedsz < maxsiz )			// do nothing if usedsz>=maxsiz
		{
			prindex = usedsz;			// new index is the current size (this includes EOS)
			basep[prindex] = 0;	
		}											
	}	
	char *BUF::copy( const char *s )	// copies string to current pointer 
    {       
		if( *s=='\a' )
		{
			if(!((prindex==0)&&(*basep==0)))
				append();				// adjust prindex to next available space
			s++;						//  only if basep is not empty
		}
		char *pntr = basep+prindex;
		int avail = maxsiz - prindex;	// left over space
		int needed = strlen( s )+1;		// required space

        if( avail<needed )
		{								// if does not fit, wrap around
			init();
			pntr = basep;
		}	
		strcpy( pntr, s );				// copy no more than available	
		DEBUG( debug() );
        return pntr;
    }		
	char *BUF::set( const char *format, ... )
    {
        if( *format=='\a' )
		{
			if(!((prindex==0)&&(*basep==0)))
				append();				// adjust prindex to next available space	
			format++;
		}
		char *pntr = basep+prindex;
		int available = maxsiz - prindex; 
		
		va_list args;
        va_start (args, format );
		int needed = vsnprintf( pntr, available, format, args);		// this will not exceed avail-1. Adds EOS
			
		if( available < needed )		// if does not fit, warp around
		{
			init();						// wrap-around, go to the beginning
			pntr = basep;
			vsnprintf( pntr, maxsiz, format, args);					// reformat there
		}
        va_end( args );
		DEBUG( debug() );
        return pntr;
    }
    char *BUF::add( const char *format, ... )	// will only add whatever it fits. Does not wrap-around
    {
 		int usedsz = prindex+strlen(basep+prindex)+1;
		
		int avail  = maxsiz - usedsz;			// left over space
        if( avail>0 )
		{
			va_list args;
			va_start (args, format );
			char *p = basep + usedsz - 1;  		// where to store new data
			avail++;							// given that we override EOS, extra is char available
			vsnprintf(p, avail, format, args);	// this will not exceed avail-1. Adds EOS
			va_end( args );
        }
		DEBUG( debug() );
        return basep + prindex;
    }
	char *BUF::quotes()
    {
		int i=0;
		char *pntr = basep+prindex;
		for( char *p = pntr; (*p) && (i<maxsiz); p++, i++ )
		{	
			if( *p=='\'' )
				*p= '\"';
		}
        return pntr;
    }
    void BUF::print( const char *prompt )
    {
		PR( prompt );   						// print prompt
		char *pntr = basep+prindex;
		PR( pntr );
		int L = strlen( pntr );
		if( L==0 )				
			CRLF();        						// ...then CRLF  
		else 
		{
			if( pntr[L-1] != '\n' )
				CRLF();
		}
    }
    char * BUF::operator ! ()    	// pointer to buffer
    {
        return basep + prindex;
    }
    char *BUF::c_str()				// pointer to buffer
    {
        return basep + prindex;
    }
    size_t BUF::length()			// same as strlen() if no rebase()
    {
        return strlen( basep + prindex );
    }
	size_t BUF::size()
    {
        return maxsiz;
    }
	DEBUG(
		void BUF::debug()
		{
			int usedsz = prindex+strlen(c_str())+1;
			PFN("prindex=%d, maxsiz=%d, usedsz=%d total, [%s]", prindex, size(), usedsz, c_str() );
	
			// Print all strings
			char *p=basep;
			PFN( "[%d]=%s", 0, p );
			for( int iz=0; iz<maxsiz-1; iz++ )	// look for EOSes
			{
				if( *p == 0 )
				{
					char c = *(p+1);
					if( (c>=0x20) && (c<0x7f) )
						PFN( "[%d]=%s", iz, p+1 );
				}
				p++;
			}
		}
	)	

