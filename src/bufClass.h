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
	const char *name;				// debug name
	bool allocated;					// true if buffer in heap

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

	BUF( int size, const char *label="" );	// labeled buffer
	BUF( char *userstr, int size );			// user specified buffer. No alloc!
	BUF( const char *userstr, int size );	// constant buffer copy. To be deprecated
	
    ~BUF();
	
    BUF & operator = ( const char *s );
	
private:
	void balloc( int size, const char *label="" );	// allocates buffer of given capacity with optional label
};


// Macros defining BUF in the stack
#define  B32(A) char A##_str[32];  BUF A( A##_str, 32)
#define  B64(A) char A##_str[64];  BUF A( A##_str, 64)
#define  B80(A) char A##_str[80];  BUF A( A##_str, 80)
#define B128(A) char A##_str[128]; BUF A( A##_str, 128)
#define B256(A) char A##_str[256]; BUF A( A##_str, 256)


