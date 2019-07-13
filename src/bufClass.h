#pragma once
#include <macros.h>
#include <Arduino.h>

//#define bufClass_debug
// -------------------------------- Base class for HEAP or STACK ----------------------

class BUFBASE						// base class
{
  protected:
    char *pntr;						// to be moved to "protected"
    size_t maxsiz;					// max size of available buffer
	 
  public:
   
	void init();
	
	char *copy( const char *s );		
	char *set( const char *format, ... );
    char *add( const char *format, ... );

	char *quotes();
	void print( const char *prompt="" );

	char * operator !();    	// pointer to buffer
	char *c_str();
	size_t length();			// strlen()
    size_t size();				// max size
};

// -------------------------------- BUF located at the HEAP --------------------------
class BUF: public BUFBASE
{
private:
	const char *name;
	
public:
    BUF( int size, const char *first="" )
    {
        pntr = new char[ size ];
        maxsiz = size;
		name = first;		// for debugging purposes
        copy( first );		// this also initialized BUF
		#ifdef bufClass_debug
			PF("Allocated '%s[%d]'.\t", name, size );
		#endif
    }
    ~BUF()
    {
        if( maxsiz )			// this is required!
		{
			delete [] pntr;
			maxsiz = 0;
			#ifdef bufClass_debug
				PF("Deallocated '%s'.    \t", name );
			#endif
		}
    }
    BUF & operator = ( const char *s ) 
    {
        copy( s );
        return *this;
    }
};
// -------------------------------- BUF located at the STACK --------------------------
template < int T>
class BUFT: public BUFBASE
{
private:
    char s[T];
public:
    BUFT( const char* sinit="" ) 
    {
        pntr = s;
        maxsiz = sizeof s;
        copy( sinit );
    }
	BUFT( int ignore, const char* sinit="" ) 
    {
        pntr = s;
        maxsiz = sizeof s;
        copy( sinit );
    }
	~BUFT()
	{
		maxsiz=0;
	}
};

#define B32 BUFT<32>
#define B64 BUFT<64>
#define B80 BUFT<80>

