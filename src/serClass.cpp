// This code has been deprecated. Use nmpClass instead

#include <string.h>
#include "bufClass.h"
#include "cpuClass.h"
#include "serClass.hpp"

void SER::registryStatus( char *prompt )
{
	PF("%s %d-bytes %d-parms %08x-types\r\n", prompt, nbytes, nparms, ntypes );
}
bool SER::notify( char *s )
{
	PF("*** SER ERROR: %s ***\r\n", s );
	return false;
}
void SER::printBuf( char *prompt, byte *base )
{
	HEADER *hd = (HEADER *)base;
	uint32_t *b = (uint32_t *)base;	
	
	PF( "%s %d-bytes %d-parms, %08x-types\r\n", 
		prompt, (int)hd->ibytes, (int)hd->iparms, hd->itypes );
	
	int sz = hd->ibytes;
	if( sz > nbytes )
		sz = nbytes;
	sz /=4;					// total size of the buffer including the header in 32b-words
	
	for( int i=0; i<sz; i++ )
		PF( "%08x ", *(b+i) );
	PF("\r\n");
}
// --------------------------- SERIALIZER --------------------------------------------------
	
	SER::SER()
	{
		resetRegistry();
	}
	void SER::resetRegistry()
    {
		nparms  = 0; 			// number of parameters registered
		nbytes  = HEADER_SIZE;	// byte count needed to encode
		ntypes  = 0;			// bit mask of types
    }	
    bool SER::registerParm( const char *nam, char typ, void *pnt, const char *format )
    {
		prm[nparms].name = (char *)nam;
        prm[nparms].type = typ;
        prm[nparms].pntr = pnt;
		
		LSHIFT( ntypes );		// shift to the left
		if( typ == 'd' )
		{
			ntypes |= 1;
			nbytes += sizeof( int );
			prm[nparms].help = (*format)?format:"=%d";
		}
		else if( typ == 'f' )
		{
			ntypes |= 2;
			nbytes += sizeof( float );
			prm[nparms].help = (*format)?format:"=%5.3f";
		}
		else if( typ == 's' )
		{
			ntypes |= 3;
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
    bool SER::encodeParms( byte *base, int maxsiz, int nmany )
    {
		byte *p = base + HEADER_SIZE;	// this is the real pointer to save parms

		int bcount = HEADER_SIZE;		// counts bytes
		int i      = 0;					// counts parms
		uint32_t btypes = 0;			// types of parms
		
		int incr;
		if( nmany > nparms )			// cannot exceed number of registered parms
			nmany = nparms;

        for( i=0; i<nmany; i++ )		// construct memory layout
        {
            incr = 0;
			LSHIFT( btypes );			// shift to the left;	
			if( prm[i].type=='f')
            {
                float *fp = (float *)p;
                *fp = *((float *)prm[i].pntr);
                incr = sizeof( float );
				btypes |= 2;
            }
            else if( prm[i].type=='d')
            {
                int *ip = (int *)p;
                *ip = *((int *)prm[i].pntr);
                incr = sizeof( int );
				btypes |= 1;
            }
            else if( prm[i].type=='s')
            {
                strncpy( (char *)p, (const char *)prm[i].pntr, USER_STR_SIZE-1 );
				p[ USER_STR_SIZE-1 ] = 0; // EOS
                incr = USER_STR_SIZE;
				btypes |= 3;
            }
			else
				return notify( BAD_PARM_TYPE );

			p += incr;					// increment the pointer by this byte count
			bcount += incr;				// number of bytes used
			
			if( bcount > maxsiz - USER_STR_SIZE )
				return notify( INSUF_BUFFER );
        }
		HEADER *hd = (HEADER *)base;		// 'hd' is now pointing to 'base'
		hd->ibytes = bcount;				// set number of bytes saved
		hd->iparms = i;
		hd->itypes = btypes;
		return true;
    }
    bool SER::decodeParms( byte *base )
    {
		HEADER *hd = (HEADER *)base;		// 'hd' is now pointing to 'base'
		uint16_t nmany = hd->iparms;		// number of parms in buffer
		
		if( nmany > nparms )
			return notify( PARM_OVERRUN );
		if( hd->ibytes > MAX_BUFSIZE)
			return notify( BAD_ENCODING );
		
		byte *p = base + HEADER_SIZE;		// start decondig point
		int i = 0;							// counts parms
		uint32_t btypes = 0;				// types of parms
		int incr;

        for( i=0; i<nmany; i++ )
        {
            incr = 0;
			LSHIFT( btypes );
			if( prm[i].type=='f')
            {
                float *fp = (float *)prm[i].pntr;
                *fp = *(float *)p;
                incr = sizeof( float );
				btypes |= 2;
            }
            else if( prm[i].type=='d')
            {
                int *ip = (int *)prm[i].pntr;
                *ip = *(int *)p;
                incr = sizeof( int );
				btypes |= 1;
            }
            else if( prm[i].type=='s')
            {
                strncpy( (char *)prm[i].pntr, (const char *)p, USER_STR_SIZE-1 );
                incr =USER_STR_SIZE;
				btypes |= 3;
            }
			else
			{
				notify( BAD_PARM_TYPE );
				break;
			}	
			p += incr;				// point to nparms parm in buffer
        }
		//PF("Types=%08x\r\n", btypes );
		return (btypes == hd->itypes);
    }	
	int SER::getParmType( const char *uname )
	{
		for( int i=0; i<nparms; i++ )
        {
			if( strcmp( prm[i].name, uname ) == 0 )
				return prm[i].type;
		}
		return 0;
	}
	bool SER::setParmValue( const char *uname, void *vp )
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
				else if( prm[i].type == 'd' )
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
	bool SER::setParmByStr( const char *uname, char *s )
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
				else if( prm[i].type == 'd' )
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
	int SER::getParmIndex( const char *uname )
	{
		int i;
		for( i=0; i<nparms; i++ )
        {
			if( strcmp( prm[i].name, uname ) == 0 )
				return i;
		}
		return -1;				// not found
	}
	BUF SER::getParmString( int i )
    {
        if( i >= nparms )		// must be between 0 and nparms-1;
			i = nparms-1;
		
		B80( s);

		s.set( "%16s", prm[i].name );
		if( prm[i].type == 'f')
			s.add( prm[i].help, *((float *)prm[i].pntr) );
		else if( prm[i].type == 'd')
			s.add( prm[i].help, *((int *)prm[i].pntr) );
		else if( prm[i].type == 's')
			s.add( prm[i].help, (char *)prm[i].pntr );
		else
			notify( BAD_PARM_TYPE );
		
    	return s;
    }
	void SER::printParms( char *prompt )
	{
		if( *prompt )
            PF( "%s\r\n", prompt );
		for( int i=0; i<nparms; i++ )
			PF( "%s\r\n", !getParmString( i ) );
	}

