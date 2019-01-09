#include "eepClass.h"

    EEP::EEP()
    {
        userp   =NULL; 
        userf   ="";
        callbk  = NULL;
        
        head.magic=EEP_MAGIC;
        head.userN=0;
        head.wifiN=WIFI_SIZE;
    } 
    void EEP::registerUserParms( void *p, int N, char *format )
    {
        userp=(byte *)p; 
        userf=format;
        head.userN=N;
    }
    void EEP::registerInitCallBk( void (*p)() )
    {
        callbk = p;
    }
    void EEP::saveParms( select_t select )     // saves memory structures to eeprom
    {
        byte c;
        
        EEPROM.begin( HEAD_SIZE + WIFI_SIZE + USER_SIZE );
        if( select & HEAD_PARMS )
            EEPROM.put( 0, head );                              // save HEADER_MASK
        if( select & WIFI_PARMS )
            EEPROM.put( HEAD_SIZE, wifi );                         // save the WiFi parm structure
        if( select & USER_PARMS )    
        {
            byte *up = userp;
            for( int i=0; i<USER_SIZE; i++ )                       // write user
                EEPROM.put( HEAD_SIZE+WIFI_SIZE+i, (c=*up++) );
        }        
        EEPROM.end();                   // commit
    }
    int EEP::fetchParms( select_t select1 )    // fetches to memory; assumes consistent structures
    {
        byte c;
        int error;
        int select = (int)select1;
        
        EEPROM.begin( HEAD_SIZE + WIFI_SIZE + USER_SIZE );
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
                EEPROM.get( HEAD_SIZE, wifi );                              // save HEADER_MASK
            if( select & USER_PARMS )    
            {
                byte *up = userp;
                for( int i=0; i<USER_SIZE; i++ )     // write user;
                {
                    EEPROM.get( HEAD_SIZE+WIFI_SIZE+i, c );
                    *up++ = c;
                    //PRI("%02x.", c);
                }
                //PRI("\r\n");
            }
        }
        EEPROM.end();                   // commit
        //PRI("Fetch error=%d\r\n", error );
        return error;
    }
    
    // if "" deletes value. If NULL, leaves unmodified
    void EEP::initWiFiParms( char *myssid, 
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
    void EEP::initUserParms()
    {
        if( callbk )        // not NULL
            (*callbk)();
    }
    void EEP::updateWiFiParms( char *myssid, char *mypwd, char *staticIP, int myport )    // initializes memory WiFi parms
    {
        initWiFiParms( myssid, mypwd, staticIP, myport );
        saveParms( WIFI_PARMS );
    }
    void EEP::updateRebootCount()
    {
        head.reboots++;
        saveParms( HEAD_PARMS );
    }
    String EEP::getParmString( char *prompt, select_t select )          // fetch and print User parms
    {
        String s("", ALLOCSTR );
        
        int c = (int) select;
        if( c == 0 ) 
            c=0xF;     // if zero, do all parameters 

        if( *prompt )
            s.add( "%s\r\n", prompt );
        if( c & HEAD_PARMS )
        {
            s.add("Magic=%04x; WiFi_sz=%d; USER_PARMSz=%d; Rebooted %d times\r\n", 
                head.magic, head.wifiN, head.userN, head.reboots );
        }
        if( c & WIFI_PARMS )
        {            
            s.add("SSID=%s; ", wifi.ssid );
            s.add(" PWD=%s; ", wifi.pwd );
            s.add("stIP=%s; ", wifi.stIP );
            s.add("Port=%d\r\n", wifi.port );
        }
        if( c & USER_PARMS )
        {
            char temp[100];
            s.add( "%s", formatUserParm( userf, userp, temp, 100 ) );
        }
        return s;
    }
    void EEP::printParms( char *prompt, select_t select ) 
    {
        PR( getParmString( prompt, select ) );
    }
    
    // GENERIC -- does not use class members
    char *EEP::formatUserParm( char *format, void *up, char *temp, int tN )
    {
        int i=0, n;
        char c;
        char *p = (char *)up;
        do
        {
            c = *format++;
            
            if( c!='%')                 
                temp[i++]=c;            // save character
            else                        // format is found
            {
                c=*format++;    
                if( c == 0 )            // premature ending
                    break;
        
                switch( c )
                {
                    case 'd': 
                        n = sprintf( &temp[i], "%d", *((int *)p) );
                        p += 4;
                        i += n;
                        break;
                    case 'f': 
                        n = sprintf( &temp[i], "%.3f", *((float *)p) );
                        p += 4;
                        i += n;
                        break;
                    case 's': 
                        n = strlen( p );
                        strcpy( &temp[i], p );
                        p += 16;
                        i += n;
                        break;
                    default:
                        break;
                }
            }
            if( i>=tN-16)       // insuffient work area
                break;
        } while( c );
        temp[i] = 0;
        return temp;
    }
