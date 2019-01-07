#ifndef EEPCLASS_H
#define EEPCLASS_H
/* ---------------------------------------------------------------------------------                      
 *    HOW TO DO...                   
 * 
 * To modify WiFi parameters:   updateWiFiParms()
 * To modify a user parameter:  userstruct.X = new; eep.saveParms( USERPARM_MASK );
 * To increment Reboot:         updateReboot()
 
 * Main Code   
 *                              class structure with parameters
 *                              class initialization myInit()
 *                              
 *                              main()
 *                                  eep.registerUserParms( structure, size format )
 *                                  eep.registerInitCallbk( func );
 *                                  bool ok = eep.fetchParms();
 *                                  if( !ok )
 *                                  {
 *                                      eep.initWiFiParms(...)
 *                                      eep.initUserParms()
 *                                      eep.saveParms();
 *                                  }
 *                                  eep.updateRebootCount();
 *                                  PR( eep.getParms("prompt") );
 *                                  
 *                       Access EEPROM       Modifies Struct
 * saveParms( se] )         yes                  no
 * fetchParms( sel )        yes                  yes
 * 
 * getParms( sel )          no                   no
 * initWiFiParms(...)       no                   yes 
 * initUserParms()          no                   yes
 * 
 * registerUserParms()      no                   no
 * registerInitCallBk()     no                   no
 *   
 * updateWiFi()    mod&save yes                 yes    
 * updateReboot()  mod&save yes                 yes
 *  
 * formatUserParms(...)     no                  no
 * --------------------------------------------------------------------------------------
 */

#include <EEPROM.h>
#include <cpuClass.h>

#define EEP_MAGIC ('K'*256+'O')

#define ALLOCSTR 200        // number of String size used by getXX() routines

enum select_t               // used in saveParms(), getParms(), fetchParms()
{
    HEADPARM_MASK = 1,
    WIFIPARM_MASK = 2,
    USERPARM_MASK = 4,
    ALL_PARM_MASK = 7        
};
    
class EEP
{
private:
    byte *userp;            // pointer to user parms and format
    char *userf;
    void (*callbk)();       // callback to initialize parameters

public:
    struct head_t
    {
        uint16_t magic;
        uint16_t wifiN;    // number of WiFi parameters 
        uint16_t userN;    // number of user parameters saved (in bytes)
        uint16_t reboots;  // increments avery time reboots
        
    } head, head0;         // head0 is used by fetch()
    
    #define HEAD_SIZE 8            // sizeof( head_t ) causes an error in eepTables()
    #define USER_SIZE head.userN

    struct wifi_t
    {
        
        char ssid[16];
        char pwd [16];
        char stIP[16];
        uint16_t port;

    } wifi;
    #define WIFI_SIZE 3*16+2   // sizeof( wifi_t )

    EEP()
    {
        userp   =NULL; 
        userf   ="";
        callbk  = NULL;
        
        head.magic=EEP_MAGIC;
        head.userN=0;
        head.wifiN=WIFI_SIZE;
    } 
    void registerUserParms( void *p, int N, char *format )
    {
        userp=(byte *)p; 
        userf=format;
        head.userN=N;
    }
    void registerInitCallBk( void (*p)() )
    {
        callbk = p;
    }
    void saveParms( select_t select=ALL_PARM_MASK )     // saves memory structures to eeprom
    {
        byte c;
        
        EEPROM.begin( HEAD_SIZE + WIFI_SIZE + USER_SIZE );
        if( select & HEADPARM_MASK )
            EEPROM.put( 0, head );                              // save HEADER_MASK
        if( select & WIFIPARM_MASK )
            EEPROM.put( HEAD_SIZE, wifi );                         // save the WiFi parm structure
        if( select & USERPARM_MASK )    
        {
            byte *up = userp;
            for( int i=0; i<USER_SIZE; i++ )                       // write user
                EEPROM.put( HEAD_SIZE+WIFI_SIZE+i, (c=*up++) );
        }        
        EEPROM.end();                   // commit
    }
    int fetchParms( select_t select1=ALL_PARM_MASK )    // fetches to memory; assumes consistent structures
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
            if( select & HEADPARM_MASK )
                head = head0;
            if( select & WIFIPARM_MASK )
                EEPROM.get( HEAD_SIZE, wifi );                              // save HEADER_MASK
            if( select & USERPARM_MASK )    
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
    void initWiFiParms( char *myssid="kontopidis2GHz", 
                        char *mypwd="123456789a", 
                        char *staticIP="", 
                        int myport=80 )    // initializes memory WiFi parms
    {
        if( myssid )
            strncpy( wifi.ssid, myssid, 16 );
        if( mypwd )
            strncpy( wifi.pwd, mypwd, 16 );
        if( staticIP )
            strncpy( wifi.stIP, staticIP, 16 );
            
        wifi.port = myport;
    }
    void initUserParms()
    {
        if( callbk )        // not NULL
            (*callbk)();
    }
    void updateWiFiParms( char *myssid, char *mypwd, char *staticIP, int myport )    // initializes memory WiFi parms
    {
        initWiFiParms( myssid, mypwd, staticIP, myport );
        saveParms( WIFIPARM_MASK );
    }
    void updateRebootCount()
    {
        head.reboots++;
        saveParms( HEADPARM_MASK );
    }
    String getParmString( char *prompt, select_t select=ALL_PARM_MASK )          // fetch and print User parms
    {
        String s("", ALLOCSTR );
        
        int c = (int) select;
        if( c == 0 ) 
            c=0xF;     // if zero, do all parameters 

        if( *prompt )
            s.add( "%s\r\n", prompt );
        if( c & HEADPARM_MASK )
        {
            s.add("Magic=%04x; WiFi_sz=%d; USERPARM_MASKz=%d; Rebooted %d times\r\n", 
                head.magic, head.wifiN, head.userN, head.reboots );
        }
        if( c & WIFIPARM_MASK )
        {            
            s.add("SSID=%s; ", wifi.ssid );
            s.add(" PWD=%s; ", wifi.pwd );
            s.add("stIP=%s; ", wifi.stIP );
            s.add("Port=%d\r\n", wifi.port );
        }
        if( c & USERPARM_MASK )
        {
            char temp[100];
            s.add( "%s", formatUserParm( userf, userp, temp, 100 ) );
        }
        return s;
    }

    // GENERIC -- does not use class members
    char *formatUserParm( char *format, void *up, char *temp, int tN )
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
};

extern EEP eep;

#endif
