#pragma once

#define DQUOTE '"'

#define LCURL pr.moveAfter( '{' )
#define RCURL pr.moveAfter( '}' )
#define COLON pr.moveAfter( ':' )
#define COMMA pr.moveAfter( ',' )

#define COPYS   pr.copyString()
#define COPYD   pr.copyDigits()

#define EXPECT(A)  	pr.expect(A)
#define FIND(A) 	pr.find(A)
#define INIT(A) 	pr.init(A)
#define ERROR		pr.error()

class PRS
{
private:
    void nospace();     // advances pointer to next non-space
   
    char *buffer;
    int maxsiz;         // max size of the buffer
    int nextbf;         // next available byte into buffer
    
    char *p;            // walking pointer into buffer
    char *porg;         // original p-pointer
    
    int errcode;          // error Code
    int errfunc;       // where the error occured
    
public:
    PRS( int size );
    ~PRS();
    
    void init( char *text );
    int error();
    void moveAfter( int c );
	void expect( char *t );
	void find( char *t );
    char *copyString();                          // expects a string in quotes
    char *copyDigits();                          // expects a string in quotes
};

