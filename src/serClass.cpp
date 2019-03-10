#include <string.h>
#include <cpuClass.h>
#include <serClass.h>

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
	String SER::getParmString( int i )
    {
        if( i >= nparms )		// must be between 0 and nparms-1;
			i = nparms-1;
		
		String s("", 256);
		s.set("");

		s.add("%16s", prm[i].name );
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
			PR( getParmString( i ) );
	}

// ------------------------------ OLD CODE ---------------------------------------------
#ifdef OLD_CODE
    EEP::EEP()
    {
        // userp   =NULL; 
        // userf   ="";
        callbk  = NULL;
		nparms       = 0;
		USER_PSIZE  = 0;
    } 
	void EEP::resetUserStruct()
    {
        nparms=0;
		USER_PSIZE = 0;
    }	
    void EEP::registerUserParm( const char *nam, char typ, void *pnt )
    {
        prm[nparms].name = (char *)nam;
        prm[nparms].type = typ;
        prm[nparms].type = pnt;
		
		USER_PSIZE += (typ=='f') ? sizeof( float ) : ((typ=='d') ? sizeof( uint16_t ) : USER_STR_SIZE);
		
        nparms++;
		if( nparms >= MAX_USERPARMS )		// do not allow more than max
			nparms = MAX_USERPARMS-1;
    }
    void EEP::registerUserInit( void (*p)() )
    {
        callbk = p;
    }

// ---------------------------- INIT STRUCT FUNCTIONS ------------------------------------		
	void EEP::initHeadStruct()
	{
		head.magic=EEP_MAGIC;
        head.headN=HEAD_PSIZE;
        head.wifiN=WIFI_PSIZE;
		head.reboots = 0;
	}
	// If "" deletes value. If NULL, leaves unmodified
    void EEP::initWiFiStruct( char *myssid, 
                        char *mypwd, 
                        char *staticIP, 
                        int myport )    // initializes memory WiFi parms
    {
        if( myssid )
            strncpy( wifi.ssid, myssid, 16 );
        if( mypwd )
            strncpy( wifi.pwd, mypwd, 16 );
        if( staticIP )
            strncpy( wifi.stIP, staticIP, 16 );
        if( myport )
            wifi.port = myport;
    }
    void EEP::initUserStruct()
    {
        if( callbk )        // not NULL
            (*callbk)();
    }

// ---------------------------- SAVE PARM FUNCTIONS ------------------------------------		
    void EEP::saveHeadParms()
    {
        byte c;
        EEPROM.begin( HEAD_PSIZE + WIFI_PSIZE + USER_PSIZE );
        EEPROM.put( 0, head );            
        EEPROM.end();                   
    }
    void EEP::saveWiFiParms()
    {
        EEPROM.begin( HEAD_PSIZE + WIFI_PSIZE + USER_PSIZE );
        EEPROM.put( HEAD_PSIZE, wifi );                         // save the WiFi parm structure
        EEPROM.end();                   // commit
    }
    void EEP::saveUserParms()
    {
        BUF base(200);					// temp allocation
		char *p = base.pntr;
		ASSERT( USER_PSIZE < 200 );
		
        for( int i=0; i<nparms; i++ )		// construct memory layout
        {
            if( prm[i].type=='f')
            {
                float *fp = (float *)p;
                *fp = *((float *)prm[i].pntr);
                p += sizeof( float );
            }
            if( prm[i].type=='d')
            {
                int *ip = (int *)p;
                *ip = *((int *)prm[i].pntr);
                p += sizeof( int );
            }
            if( prm[i].type=='s')
            {
                strncpy( p, (const char *)prm[i].pntr, 19 );
                p += USER_STR_SIZE;
            }
        }
										// Now, save layout to EEPROM
        byte c;
		EEPROM.begin( HEAD_PSIZE + WIFI_PSIZE + USER_PSIZE );
		byte *up = (byte *)base.pntr;

		EEPROM.put( HEAD_PSIZE+WIFI_PSIZE,   nparms );				// write number of parms included
		EEPROM.put( HEAD_PSIZE+WIFI_PSIZE+2, userbcount );		// write the byte user size

		for( int i=0; i<USER_PSIZE; i++ )                       		// write user
			EEPROM.put( HEAD_PSIZE+WIFI_PSIZE+4+i, (c=*up++) );
		
        EEPROM.end();                   						// commit
    }

// ---------------------------- FETCH PARM FUNCTIONS ------------------------------------		
    int EEP::fetchHeadParms() 
    {
        int error=0;
        
        EEPROM.begin( HEAD_PSIZE + WIFI_PSIZE + USER_PSIZE );
        EEPROM.get( 0, head0 );                     			// fetch HEADER_MASK candidate
        
        if( head0.magic != EEP_MAGIC )
            error = 1;
        if( head0.headN != HEAD_PSIZE )
            error = 2;
        if( head0.wifiN != WIFI_PSIZE )
            error = 3;
        if( !error )
            head = head0;
        EEPROM.end();                   						// commit
        // CHECK("Fetch error=%d\r\n", error );
        return error;
    }
    int EEP::fetchWiFiParms()
    {
        EEPROM.begin( HEAD_PSIZE + WIFI_PSIZE + USER_PSIZE );
        EEPROM.get( HEAD_PSIZE, wifi );  
        EEPROM.end();    
        return 0;
    }
    int EEP::fetchUserParms()
    {
        BUF base(200);					// temp allocation
		char *p  = base.pntr;
		ASSERT( USER_PSIZE < 200 );
		int error = 0;
		uint16_t nparms1;
		uint16_t size1;
		
        EEPROM.begin( HEAD_PSIZE + WIFI_PSIZE + USER_PSIZE );
		EEPROM.get( HEAD_PSIZE+WIFI_PSIZE,   nparms1 );
		EEPROM.get( HEAD_PSIZE+WIFI_PSIZE+2, size1 );
		if( nparms1 != nparms )
			error = 1;
		if( size1 != USER_PSIZE )
			error = 2;
		
		int c;
		for( int i=0; (i<USER_PSIZE) && (error==0); i++ )     
		{
			EEPROM.get( HEAD_PSIZE+WIFI_PSIZE+4+i, c );
			*p++ = c;
		}
        EEPROM.end();                   
		
		if( error )
		{
			//CHECK("FETCH USER Error %d\r\n", error );
			return error;
		}
		
		// Now, base contains all user parms.
		// Place them to user structure
		p  = base.pntr;
        for( int i=0; i<nparms; i++ )
        {
            if( prm[i].type=='f')
            {
                float *fp = (float *)prm[i].pntr;
                *fp = *(float *)p;
                p += sizeof( float );
            }
            if( prm[i].type=='d')
            {
                int *ip = (int *)prm[i].pntr;
                *ip = *(int *)p;
                p += sizeof( int );
            }
            if( prm[i].type=='s')
            {
                strncpy( (char *)prm[i].pntr, (const char *)p, 19 );
                p+=USER_STR_SIZE;
            }
        }
		return 0;
    }	
// ------------------------------ MODIFICATIONS ------------------------------------------
    void EEP::updateRebootCount()
    {
        head.reboots++;
        saveHeadParms();
    }

	int EEP::getUserParmType( const char *uname )
	{
		for( int i=0; i<nparms; i++ )
        {
			if( strcmp( name[i], uname ) == 0 )
				return prm[i].type;
		}
		return 0;
	}
	int EEP::setUserParmValue( const char *uname, void *vp )
	{
		for( int i=0; i<nparms; i++ )
        {
			if( strcmp( name[i], uname ) == 0 )
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
					return 2;	// illegal type givenPF
				}
				return 0;
			}
		}
		return 1;				// not found
	}

// ---------------------------- GET STRING FUNCTIONS ------------------------------------			
    String EEP::getHeadString()
    {
        String s("", 256);
		s.set("Magic=%04x; Head_sz=%d, WiFi_sz=%d; Rebooted %d times\r\n", 
                head.magic, head.headN, head.wifiN, head.reboots );
		return s;
    }
    String EEP::getWiFiString()
    {
        String s("", 256);
		s.set("SSID=%s; ", 	 &wifi.ssid[0] );
		s.add(" PWD=%s; ", 	 &wifi.pwd[0] );
		s.add("stIP=%s; ", 	 &wifi.stIP[0] );
		s.add("Port=%d\r\n", wifi.port );
		return s;
    }
	String EEP::getUserString()
    {
        String s("", 256);
		s.set("");
        for( int i=0; i<nparms; i++ )
        {
            s.add("%16s=", name[i] );
            if( prm[i].type == 'f')
            {
				 float f = *((float *)prm[i].pntr);
				 if( f >= 1000.0 )
					 s.add( "%.1f", f );
				 else if( f >= 10.0 )
					 s.add( "%.2f", f );
				 else
					 s.add( "%.3f", f );
			}   
            if( prm[i].type == 'd')
                s.add( "%d", *((int *)prm[i].pntr) );
            if( prm[i].type == 's')
                s.add( "%s", (char *)prm[i].pntr );
            s.add("\r\n");
        }
    	return s;
    }

// ---------------------------- PRINT STRUCT FUNCTIONS ------------------------------------		
	void EEP::printHeadStruct( char *prompt )
	{
		if( *prompt )
            PF( "%s\r\n", prompt );
		PN( getHeadString() );
	}
	void EEP::printWiFiStruct( char *prompt )
	{
		if( *prompt )
            PF( "%s\r\n", prompt );
		PN( getWiFiString() );
	}
	void EEP::printUserStruct( char *prompt )
	{
		if( *prompt )
            PF( "%s\r\n", prompt );
		PN( getUserString() );
	}
    void EEP::updateWiFiParms( char *myssid, char *mypwd, char *staticIP, int myport )    // initializes memory WiFi parms
    {
        initWiFiParms( myssid, mypwd, staticIP, myport );
        saveWiFiParms();
    }
   int EEP::fetchParms( select_t select1 )    // fetches to memory; assumes consistent structures
    {
        byte c;
        int error;
        int select = (int)select1;
        
        EEPROM.begin( HEAD_PSIZE + WIFI_PSIZE + USER_PSIZE );
        EEPROM.get( 0, head0 );                     // fetch HEADER_MASK candidate
        error = 0;
        if( head0.magic != EEP_MAGIC )
            error = 1;
        if( head0.userN != head.userN )
            error = 2;
        if( head0.wifiN != head.wifiN)
            error = 3;
        
        if( !error )
        {
            if( select & HEAD_PARMS )
                head = head0;
            if( select & WIFI_PARMS )
                EEPROM.get( HEAD_PSIZE, wifi );                              // save HEADER_MASK
            if( select & USER_PARMS )    
            {
                byte *up = userp;
                for( int i=0; i<USER_PSIZE; i++ )     // write user;
                {
                    EEPROM.get( HEAD_PSIZE+WIFI_PSIZE+i, c );
                    *up++ = c;
                    // PRI("%02x.", c);
                }
                // PRI("\r\n");
            }
        }
        EEPROM.end();                   // commit
        // PRI("Fetch error=%d\r\n", error );
        return error;
    }
    String EEP::getParmString( char *prompt, select_t select )          // fetch and print User parms
    {
		String s("", 256-16);
        s.set("");
		
        int c = (int) select;
        if( c == 0 ) 
            c=0xF;     // if zero, do all parameters 

        if( *prompt )
            s.add( "%s\r\n", prompt );
        
		if( c & HEAD_PARMS )
        {
			s.add("Magic=%04x; WiFi_sz=%d; USER_PARMSz=%d; Rebooted %d times\r\n", 
                head.magic, head.wifiN, head.userN, head.reboots );
			// PN("C is 1 "); PR( !s );
        }
        if( c & WIFI_PARMS )
        {            
			s.add("SSID=%s; ", 	 &wifi.ssid[0] );
            s.add(" PWD=%s; ", 	 &wifi.pwd[0] );
            s.add("stIP=%s; ", 	 &wifi.stIP[0] );
            s.add("Port=%d\r\n", wifi.port );
			// PN("C is 2 ");PR( !s );
        }
        if( c & USER_PARMS )
        {
			char temp[100];
            s.add( "%s", formatUserParm( userf, userp, temp, 100 ) );
			// PN("C is 4 ");PR( !s );
        }		
		return s;
    }
    void EEP::printParms( char *prompt, select_t select ) 
    {
        PR( getParmString( prompt, select ) );
    }

    void EEP::printUserParms( char *prompt )
    {
        BUF base(200);					// temp allocation
		char *p  = base.pntr;
		char *up = base.pntr;
		ASSERT( userbsize < 200 );

        EEPROM.begin( HEAD_PSIZE + WIFI_PSIZE + userbsize );
		for( int i=0; i<USER_PSIZE; i++ )     
		{
			EEPROM.get( HEAD_PSIZE+WIFI_PSIZE+i, c );
			*up++ = c;
		}
        EEPROM.end();                   // commit

        if( *prompt )
            PF( "%s\r\n", prompt );

        for( int i=0; i<nparms; i++ )
        {
            if( prm[i].type=='f')
            {
                float *fp = (float *)p;
                p += sizeof( float );
                PF("%s=%f\r\n", name[i], *fp );
            }
            if( prm[i].type=='d')
            {
                int *ip = (int *)p;
                p += sizeof( int );
                PF("%s=%d\r\n", name[i], *ip );
            }
            if( prm[i].type=='s')
            {
                PF("%s=%s\r\n", name[i], (char *)p );
                p+=USER_STR_SIZE;
            }
        }
    }
#endif