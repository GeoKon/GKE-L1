#include <string.h>
#include "bufClass.h"
#include "nmpClass.h"

size_t NMP::getSize()
{
	return nbytes;
}
bool NMP::notify( char *s )
{
	PF("*** SER ERROR: %s ***\r\n", s );
	return false;
}
// --------------------------- SERIALIZER --------------------------------------------------
	
	void NMP::resetRegistry()
    {
		nparms  = 0; 			// number of parameters registered
		nbytes  = 0;			// byte count needed to encode
    }	
    bool NMP::registerParm( const char *nam, char typ, void *pnt, const char *format )
    {
		prm[nparms].name = (char *)nam;
        prm[nparms].type = typ;
        prm[nparms].pntr = pnt;
		
		if( (typ == 'd') || (typ == 'i') )
		{
			nbytes += sizeof( int );
			prm[nparms].help = (*format)?format:"=%d";
		}
		else if( typ == 'f' )
		{
			nbytes += sizeof( float );
			prm[nparms].help = (*format)?format:"=%5.3f";
		}
		else if( typ == 's' )
		{
			nbytes += USER_STR_SIZE;
			prm[nparms].help = (*format)?format:"=%s";
		}
		else
			return notify( BAD_PARM_TYPE );
		
        nparms++;
		if( nparms >= MAX_USERPARMS )				// do not allow more than max
			return notify( INSUF_NPARMS );
		return true;
    }
	int NMP::getParmType( const char *uname )
	{
		for( int i=0; i<nparms; i++ )
        {
			if( strcmp( prm[i].name, uname ) == 0 )
				return prm[i].type;
		}
		return 0;
	}
	bool NMP::setParmValue( const char *uname, void *vp )
	{
		int i;
		for( i=0; i<nparms; i++ )
        {
			if( strcmp( prm[i].name, uname ) == 0 )
			{
				if( prm[i].type == 'f' )
				{
					float *fp = (float *)prm[i].pntr;
					*fp = *((float *)vp);
				}
				else if( (prm[i].type == 'd') || (prm[i].type == 'd') )
				{
					int *ip = (int *)prm[i].pntr;
					*ip = *((int *)vp);
				}
				else if( prm[i].type == 's' )
				{
					char *cp = (char *)prm[i].pntr;
					strncpy( cp, (const char *)vp, USER_STR_SIZE );
				}
				else
				{
					notify( BAD_PARM_TYPE );
					break;
				}
				return true;		
			}
		}
		return false;				// not found
	}
	bool NMP::setParmByStr( const char *uname, char *s )
	{
		int i;
		for( i=0; i<nparms; i++ )
        {
			if( strcmp( prm[i].name, uname ) == 0 )
			{
				if( prm[i].type == 'f' )
				{
					float *fp = (float *)prm[i].pntr;
					*fp = atof( s );
				}
				else if( (prm[i].type == 'd') || (prm[i].type == 'd') )
				{
					int *ip = (int *)prm[i].pntr;
					*ip = atoi( s );
				}
				else if( prm[i].type == 's' )
				{
					char *cp = (char *)prm[i].pntr;
					strncpy( cp, s, USER_STR_SIZE );
				}
				else
				{
					notify( BAD_PARM_TYPE );
					break;
				}
				return true;		
			}
		}
		return false;				// not found
	}
	int NMP::getParmIndex( const char *uname )
	{
		int i;
		for( i=0; i<nparms; i++ )
        {
			if( strcmp( prm[i].name, uname ) == 0 )
				return i;
		}
		return -1;				// not found
	}
	B80 NMP::getParmString( int i )
    {
        if( i >= nparms )		// must be between 0 and nparms-1;
			i = nparms-1;
		
		B80 s;

		s.set( "%10s", prm[i].name );
		if( prm[i].type == 'f')
			s.add( prm[i].help, *((float *)prm[i].pntr) );
		else if( (prm[i].type == 'd') || (prm[i].type == 'd') )
			s.add( prm[i].help, *((int *)prm[i].pntr) );
		else if( prm[i].type == 's')
			s.add( prm[i].help, (char *)prm[i].pntr );
		else
			notify( BAD_PARM_TYPE );
		
    	return s;
    }
	void NMP::printParms( char *prompt, BUF *bp )
	{
		if( *prompt )
            PF( "%s\r\n", prompt );
		
		for( int i=0; i<nparms; i++ )
		{
			if( !bp )
				PF( "%s\r\n", !getParmString( i ) );
			else
				bp->add( "%s\r\n", !getParmString( i ) );
		}
	}
