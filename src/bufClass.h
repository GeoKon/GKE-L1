#pragma once
#include <macros.h>
#include <Arduino.h>

#define DEBUG_CHAR '@'

// -------------------------------- Base class for HEAP or STACK ----------------------

class BUFBASE						// base class
{
  protected:
    char *pntr;						// to be moved to "protected"
    size_t maxsiz;					// max size of available buffer
	const char *name;

  public:
   
	void init();
	
	char *copy( const char *s );		
	char *set( const char *format, ... );
    char *add( const char *format, ... );

	char *quotes();
	void print( const char *prompt="" );

	char * operator !();    	// pointer to buffer. To be deprecated
	char *c_str();
	size_t length();			// strlen()
    size_t size();				// max size
};

// -------------------------------- BUF located at the HEAP --------------------------
class BUF: public BUFBASE
{
public:
	// Usage: BUF t(100, "MyBuf")
	// Allocates t as a buffer of 100 bytes, named MyBuf.
	// If MyBuf starts with @, then alloc - dealloc messages are printed
	BUF( int size, const char *label="" )
    {
        pntr = new char[ size ];
        maxsiz = size;
		name = label;					// for debugging purposes
        *pntr = 0;
		
		if( *name==DEBUG_CHAR )
			PF("Allocated %s[%d]\r\n", name, maxsiz );
    }
	// Usage: B80 s
	// Defines a BUF in the stack of 80 bytes. No malloc
	BUF( char *userstr, int size )		// user specified buffer. No alloc!
    {
        pntr = userstr;
        maxsiz = size;
		name = "$";				// $ indicates a user specified buffer
		*pntr = 0;		
    }
    ~BUF()
    {
        if( maxsiz )			// this is required!
		{
			maxsiz = 0;
			if( *name != '$' )	// deallocate buffer only if allocated previously.
				delete [] pntr;
			if( *name==DEBUG_CHAR )
				PF("Deallocated %s\r\n", name );
		}
    }
    BUF & operator = ( const char *s ) 
    {
        copy( s );
        return *this;
    }
};

// Macros defining BUF in the stack
#define  B32(A) char A##_str[32];  BUF A( A##_str, 32)
#define  B64(A) char A##_str[64];  BUF A( A##_str, 64)
#define  B80(A) char A##_str[80];  BUF A( A##_str, 80)

#define B128(A) char A##_str[128]; BUF A( A##_str, 128)
#define B256(A) char A##_str[256]; BUF A( A##_str, 256)


