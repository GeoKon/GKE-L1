#pragma once
#include <macros.h>
#include <Arduino.h>

//	#define DEBUG(A) A
	#define DEBUG(A)

// -------------------------------- Base class for HEAP or STACK ----------------------

class BUF						
{
  protected:
    char *basep;					// allocated buffer. Used by alloc() 
    u16 maxsiz;						// max size of available buffer
	u16 prindex;					// number of bytes used up to this point plus EOS. new base = &basep[prindex];	
	//size_t usedsz;				// total number of bytes used so far (includes EOS) in basep
	
	DEBUG(const char *name);		// debug name
	bool allocated;					// true if buffer in heap
	
  public:
	void balloc( int size );				// allocates buffer of given capacity 
	
	BUF( int size, const char *label="" );	// labeled buffer
	BUF( char *userstr, const int size );	// user specified buffer. No alloc!
	//BUF( const char *userstr, int size );	// constant buffer copy. To be deprecated
	
    ~BUF();
	
    BUF & operator = ( const char *s );
	
	void init();
	void append();
	
	char *copy( const char *s );

	#define pset( format, ... ) set( "\a"format, ##__VA_ARGS__)	
	char *set( const char *format, ... );
    char *add( const char *format, ... );

	char *quotes();
	void print( const char *prompt="" );

	char * operator !();    				// pointer to buffer. To be deprecated
	char *c_str();
	size_t length();						// strlen() of current buffer
    size_t size();							// max size
	
	DEBUG(void debug());
};

// Macros defining BUF in the stack
#define  B32(A) char A##_str[32];  BUF A( A##_str, 32)
#define  B64(A) char A##_str[64];  BUF A( A##_str, 64)
#define  B80(A) char A##_str[80];  BUF A( A##_str, 80)
#define B128(A) char A##_str[128]; BUF A( A##_str, 128)
#define B256(A) char A##_str[256]; BUF A( A##_str, 256)



