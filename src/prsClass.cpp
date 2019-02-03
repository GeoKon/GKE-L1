#include <Arduino.h>
#include <ctype.h>
#include <stdlib.h>
#include "prsClass.h"

PRS::PRS( int size )
{
    buffer = new char [size];
    maxsiz = size;
    nextbf  = 0;
    errcode = 0;
    errfunc = 0;
}
PRS::~PRS()
{
    if( buffer ) 
    {
        delete buffer;
        buffer = NULL;
        maxsiz = 0;
    }
}
void PRS::init( char *text )         // reset pointer
{
    p = porg = text;
    nextbf  = 0;
    errcode = 0;
    errfunc = 0;
}
void PRS::nospace()
{
    while( *p==' '||*p=='\t'||*p=='\r'||*p=='\n' )
        p++;
}
char * PRS::copyString()
{
    if( errcode ) 
        return buffer+nextbf;
    errfunc++;
    
    nospace();
    if( *p != DQUOTE )
    {
        errcode = errfunc;
        return p;
    }
    p++;                                // next char should be buffered
    char *result = buffer+nextbf;
    while( *p != DQUOTE )
    {
        *(buffer + nextbf++) = *p++;
        if( nextbf >= maxsiz-2 )        // leave room to terminate buffer with EOS
        {
            errcode = errfunc;
            break;
        }
    }
    p++;                                // pointer is left AFTER the quote
    *(buffer + nextbf++) = 0;    
    return result;
}
void PRS::expect( char *user )
{
    if( errcode ) 
        return;
    errfunc++;
    
    nospace();
    if( *p != DQUOTE )
    {
        errcode = errfunc;
        return;
    }
    p++;                                // next char should be buffered
    while( *p != DQUOTE )
    {
        if( *p++ != *user++ )
        {
            errcode = errfunc;
            break;
        }
    }
    p++;                                // pointer is left AFTER the quote
}
void PRS::find( char *user )
{
    if( errcode ) 
        return;
    errfunc++;
    
    char *q = strstr( p, user );		// search string for user string
	if( q == NULL )
	{
		errcode = errfunc++;
		return;
	}
	p = q;								// p now points to the beginning of the user string
	p += strlen( user );				// point to the end, i.e. the quote
	if( *p != DQUOTE )
    {
        errcode = errfunc;
        return;
    }
    p++;                                // pointer is left AFTER the quote
}

char * PRS::copyDigits()
{
    if( errcode ) 
        return buffer+nextbf;
    errfunc++;
    nospace();

    char *result = buffer+nextbf;
    while( isdigit(*p) || (*p=='.') || (*p=='-') )
    {
        *(buffer + nextbf++) = *p++;
        if( nextbf >= maxsiz-1 )
        {
            errcode = errfunc;
            break;
        }
    }
    //                                // pointer is left AFTER numbers
    *(buffer + nextbf++) = 0;    
    return result;
}
void PRS::moveAfter( int c )
{
    if( errcode )
        return;
    errfunc++;
    
    while( *p )
    {
        if( *p++ == c )
            return;
    }
    errcode = errfunc;
}
int PRS::error()
{
    return errcode;
}
