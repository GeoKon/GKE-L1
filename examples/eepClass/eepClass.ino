/*
 * 1= test eeprom (no parms)
 * 2= test used parms
 * 3= test eeprom with user parms
 *
 * Use eepGlobal.ino (in Lib2) as a model to write main setup()
 */
#define TEST 2
/*
 * Sketch uses 283960 bytes (27%) of program storage space. Maximum is 1044464 bytes.
 * Global variables use 30332 bytes (37%) of dynamic memory, leaving 51588 bytes for local variables. Maximum is 81920 bytes.
 * Uploading 288112 bytes
 */
#include "cpuClass.h"
#include "eepClass.h"   
#include "nmpClass.h" 
#include <FS.h>

// ------------------- allocation of base classes ------------------------------
    CPU cpu;
    EEP eep;
// -----------------------------------------------------------------------------

#if TEST > 1
    
    NMP nmp;    
    class Global
    {
    public:    

        float f1;
        float f2;
        int i1;
        char cp[USER_STR_SIZE];
        
        #define MPARM_SIZE   3*4+USER_STR_SIZE
        
        void init()
        {
            f1=4.56;
            f2=123.0;
            i1=10;
            strcpy( &cp[0], "ABCDabcd");
        }        
        void install()
        {
            nmp.resetRegistry();
            nmp.registerParm( "f1", 'f', &f1, "Volts");
            nmp.registerParm( "f2", 'f', &f2 );
            nmp.registerParm( "i1", 'd', &i1 );
            nmp.registerParm( "cp", 's', &cp[0] );
        }
    } my;
    
#endif

#if TEST == 1       // ----------------------------- test EEP Class -----------------------------------
void setup()
{
    cpu.init();
    SPIFFS.begin();                     // crashes sometimes if not present
    int e;
   
    for(;;)
    {
        char *s = cpu.prompt( "\r\nPress i=init&save, f=fetch&display, r=incr reboot, w=new wifi&save: " );
        char c = s[0];
        PF("\r\n");
        
        if( c=='i' )
        {
            eep.initHeadParms();
            eep.printHeadParms("--- See head");

            eep.initWiFiParms();                    // use the defaults
            eep.printWiFiParms("--- See Wifi");
        }
        else if( c == 'f' )
        {
            eep.fetchHeadParms();
            eep.printHeadParms("--- Fetched Head parms");

            PF("Check MAGIC returned: %d\r\n", eep.checkHeadParms( EEP_MAGIC, 0 ) );
            PF("Check 0 returned: %d\r\n", eep.checkHeadParms( 0, 0 ) );
            
            eep.fetchWiFiParms();
            eep.printWiFiParms("--- Fetched WiFi parms");
        }
        else if( c == 'r' )
        {
            eep.incrBootCount();
            eep.printHeadParms("--- Updated parms");
        }
        else if( c == 'w' )
        {
            eep.initWiFiParms( "new SSID", "new pwd" );
            eep.printWiFiParms("--- New WiFi parms");
        }
    }
}
#endif

#if TEST == 2   // ----------------------------- test SER Class -----------------------------------
void setup()
{
    cpu.init();
    SPIFFS.begin();                     // crashes sometimes if not present
    int e;
    bool ok;
    
    for(;;)
    {
        char *s = cpu.prompt( "\r\nPress i=initialize, p=print, m=modify, o=mod again: " );
        char c = s[0];
        PF("\r\n");
        if( c == 'i' )          // initialize parms
        {
            my.init();
            my.install();
            nmp.printAllParms("--- See initialized parms");
        }
        else if( c == 'p' )     // print parms
        {
            nmp.printAllParms("--- See parms");
        }
        else if( c == 'm' )
        {
            float fv = 20.30;
            ok = nmp.setParmValue( "f1", &fv );
            PF( "f1=%f (ok:%d)\r\n", fv, ok );

            int  iv = 20;
            ok = nmp.setParmValue( "i1", &iv );
            PF( "i1=%d (ok:%d)\r\n", iv, ok );

            ok = nmp.setParmValue( "cp", (void *)"NEW" );
            PF( "cp=%s (ok:%d)\r\n", "NEW", ok );

            eep.saveUserStruct( (byte *)&my.f1, MPARM_SIZE );
            nmp.printAllParms("--- Saved parms parms");
        }
        else if( c == 'o' )
        {
            my.f1 = 1.0;
            my.f2 = 2.0;
            my.i1 = 3;
            eep.saveUserStruct( (byte *)&my.f1, MPARM_SIZE );
            nmp.printAllParms("--- Updated parms parms");
        }
    }        
}
#endif

#if TEST == 3       // ----------------------------- test EEP & SER Class -----------------------------------
void setup()
{
    cpu.init();
    SPIFFS.begin();                     // crashes sometimes if not present
    int e, ok;
    
    for(;;)
    {
        char *s = cpu.prompt( "\r\nPress i=init&save, r=register modified parms, f=fetch&display: " );
        char c = s[0];
        PF("\r\n");
        
        if( c=='i' )
        {
            my.init();
            my.install();
            nmp.printAllParms("--- See initialized parms");

            eep.initHeadParms();
            eep.printHeadParms("--- See head");

            eep.initWiFiParms();
            eep.printWiFiParms("--- See Wifi");

            eep.saveUserStruct( (byte *)&my.f1, MPARM_SIZE );
            eep.printHeadParms("--- See updated header");
        }
        else if( c == 'r' )
        {
            my.install();               // just register parms
            my.f1 = 1.0;
            my.f2 = 2.0;
            my.i1 = 3;
            nmp.printParms("--- See user parms");
        }
        else if( c == 'f' )
        {
            eep.fetchHeadParms();
            eep.printHeadParms("--- Fetched Head parms");

            eep.fetchUserStruct( (byte *)&my.f1, MPARM_SIZE );

            nmp.printAllParms("--- See decoded parms");
        }
    }
}
#endif

void loop() 
{
    yield();
}
