#include <string.h>
#include "cpuClass.h"	// needed for the sf() only
#include "bufClass.h"
#include "nmpClass.h"

static char *BAD_ENCODING = "Bad Encoding";
static char *PARM_OVERRUN = "Parm Overrun";
static char *INSUF_BUFFER = "Insufficient Buffer";
static char *BAD_PARM_TYPE = "Bad Parm Type";
static char *INSUF_NPARMS = "Insufficient nParms";
static char *UNKNOWN_PARM = "Unknown Parm";

size_t NMP::getSize()
{
	return nbytes;
}
int NMP::getParmCount()
{
	return nparms;
}
bool NMP::notify( char *s )
{
	PF("*** SER ERROR: %s ***\r\n", s );
	return false;
}
// --------------------------- NAMED PARAMETERS ----------------------------------------------
	
	void NMP::resetRegistry()
    {
		nparms  = 0; 			// number of parameters registered
		nbytes  = 0;			// byte count needed to encode
    }	
    bool NMP::registerParm( const char *nam, char typ, void *pnt, const char *info1, const char *format )
    {
		prm[nparms].name = (char *)nam;
        prm[nparms].type = typ;
        prm[nparms].pntr = pnt;
		
		if( (typ == 'd') || (typ == 'i') )
		{
			nbytes += sizeof( int );
			prm[nparms].format = (*format)?format:"%d";
		}
		else if( typ == 'f' )
		{
			nbytes += sizeof( float );
			prm[nparms].format = (*format)?format:"%5.3f";
		}
		else if( typ == 's' )
		{
			nbytes += USER_STR_SIZE;
			prm[nparms].format = (*format)?format:"%s";
		}
		else
			return notify( BAD_PARM_TYPE );
		
		prm[nparms].info = info1;
        nparms++;
		if( nparms >= MAX_USERPARMS )				// do not allow more than max
			return notify( INSUF_NPARMS );
		return true;
    }
	
	// ------------------------- SET PARAMETERS -------------------------------------
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
					notify( UNKNOWN_PARM );
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
	// ------------------------- GET PARAMETERS -------------------------------------

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
	const char* NMP::getParmName( int idx )
	{
		return prm[idx].name;
	}
	int NMP::getParmType( const char *uname )
	{
		int i = getParmIndex( uname );
		if( i<0 )
			return 0;

		return prm[i].type;
	}	
	int NMP::getParmType( int i )
	{
		return prm[i].type;
	}	
	const char* NMP::getParmInfo( const char *uname )
	{
		int i = getParmIndex( uname );
		if( i<0 )
			return UNKNOWN_PARM;

		return prm[i].info;
	}
	const char* NMP::getParmInfo( int i )
	{
		return prm[i].info;
	}	
	char* NMP::getParmValueStr( int i )
	{
		int type  = prm[i].type;
		const char *fmt = prm[i].format;
				
		if( type == 'f' )
			sf( tempbuf, 32,  fmt, *(float *)prm[i].pntr );

		else if( (type == 'd') || (type == 'i') )
			sf( tempbuf, 32,  fmt,  *(int *)prm[i].pntr );

		else if( type == 's' )
			sf( tempbuf, 32,  fmt, (char *)prm[i].pntr );
		else
			return BAD_PARM_TYPE;
		
		return tempbuf;
	}	
	char* NMP::getParmValueStr( const char *uname )
	{
		int i = getParmIndex( uname );
		if( i<0 )
			return UNKNOWN_PARM;
		return getParmValueStr( i );
	}
		
	// -------------------------------- PRINT METHODS -------------------------------
	
	void NMP::_printhandler( BUF *bp, int i )
	{
		const char *pformat = "\t%7s =\t%s\t%s\r\n";
	
		if( bp==NULL )
			PF( pformat, prm[i].name, getParmValueStr( i ), prm[i].info );
		else
			bp->add( pformat, prm[i].name, getParmValueStr( i ), prm[i].info );
	}
	
	void NMP::printParm( char *name, BUF *bp )
	{
		int i = getParmIndex( name );
		if( i<0 )
		{
			if( bp==NULL )
				PF     ( "%s [%s]\r\n", UNKNOWN_PARM, name );
			else
				bp->add( "%s [%s]\r\n", UNKNOWN_PARM, name );
		}
		else
			_printhandler( bp, i );
	}

	void NMP::printAllParms( char *prompt, BUF *bp )
	{
		if( *prompt )
            PF( "%s\r\n", prompt );

		for( int i=0; i<nparms; i++ )
			_printhandler( bp, i );
	}
	
	// ================== Meguno Link Methods ==========================
	
	void NMP::_megunohandler( int i, char *channel )
	{
		char *mformat;
		if( *channel )
			PF( "{TABLE:%s|SET|%s|%s|%s}\r\n", channel, prm[i].name, getParmValueStr( i ), prm[i].info );
		else
			PF( "{TABLE|SET|%s|%s|%s}\r\n", prm[i].name, getParmValueStr( i ), prm[i].info );
	}
	void NMP::printMgnParm( char *channel, char *name  )
	{
		int i = getParmIndex( name );
		
		if( i<0 )
			printMgnInfo( channel, name, "is unknown" );
		else
			_megunohandler( i, channel );
	}
	void NMP::printMgnAllParms( char *channel )
	{
		for( int i=0; i<nparms; i++ )
			_megunohandler( i, channel );
	}
	void NMP::printMgnInfo( char *channel, char *value, char *info )
	{
		if( *channel )
			PF( "{TABLE:%s|SET|INFO|%s|%s}\r\n", channel, value, info );
		else
			PF( "{TABLE|SET|INFO|%s|%s}\r\n", value, info );
	}
