/*
 * 1= test eeprom (no parms)
 * 2= test used parms
 * 3= test eeprom with user parms
 *
 * Use eepGlobal.ino (in Lib2) as a model to write main setup()
 */
#define TEST 3
/*
 * Sketch uses 283960 bytes (27%) of program storage space. Maximum is 1044464 bytes.
 * Global variables use 30332 bytes (37%) of dynamic memory, leaving 51588 bytes for local variables. Maximum is 81920 bytes.
 * Uploading 288112 bytes
 */
#include "cpuClass.h"
#include "eepClass.h"   
#include "serClass.h" 
#include <FS.h>

// ------------------- allocation of base classes ------------------------------
    CPU cpu;
    EEP eep;
// -----------------------------------------------------------------------------

#if TEST > 1
    
    SER ser;    
    class Global
    {
    public:    
        float f1;
        float f2;
        int i1;
        char cp[20];

        void init()
        {
            f1=4.56;
            f2=123.0;
            i1=10;
            strcpy( &cp[0], "ABCDabcd");
        }
        void install()
        {
            ser.resetRegistry();
            ser.registryStatus("0");
            ser.registerParm( "f1", 'f', &f1, "=%4.2f Volts");
            ser.registryStatus("After f1");
            ser.registerParm( "f2", 'f', &f2 );
            ser.registryStatus("After f2");
            ser.registerParm( "i1", 'd', &i1 );
            ser.registryStatus("After i1");
            ser.registerParm( "cp", 's', &cp[0] );
            ser.registryStatus("After cp");
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
        char *s = cpu.prompt( "\r\nPress i=initialize, p=print, m=modify, e=encode: " );
        char c = s[0];
        PF("\r\n");
        if( c == 'i' )          // initialize parms
        {
            my.init();
            my.install();
            ser.printParms("--- See initialized parms");
        }
        else if( c == 'p' )     // print parms
        {
            ser.printParms("--- See parms");
        }
        else if( c == 'm' )
        {
            float fv = 20.30;
            ok = ser.setParmValue( "f1", &fv );
            PF( "f1=%f (ok:%d)\r\n", fv, ok );

            int  iv = 20;
            ok = ser.setParmValue( "i1", &iv );
            PF( "i1=%d (ok:%d)\r\n", iv, ok );

            ok = ser.setParmValue( "cp", (void *)"NEW" );
            PF( "cp=%s (ok:%d)\r\n", "NEW", ok );
        }
        else if( c == 'e' )
        {
            byte buf[200];
            ser.encodeParms( buf, sizeof buf );
            ser.printBuf("See encoded buffer", buf );

            my.f1 = 1.0;
            my.f2 = 2.0;
            my.i1 = 3;
            ser.printParms("--- Modified parms");
            
            ok = ser.decodeParms( buf );
            PF( "Parms decoded (ok:%d)\r\n", ok );            

            ser.printParms("--- See decoded parms");
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
    byte buf[200];
    
    for(;;)
    {
        char *s = cpu.prompt( "\r\nPress i=init&save, r=register modified parms, f=fetch&display: " );
        char c = s[0];
        PF("\r\n");
        
        if( c=='i' )
        {
            my.init();
            my.install();
            ser.printParms("--- See initialized parms");

            ser.encodeParms( buf, sizeof buf );
            ser.printBuf("--- See encoded buffer", buf );

            eep.initHeadParms();
            eep.printHeadParms("--- See head");

            eep.initWiFiParms();
            eep.printWiFiParms("--- See Wifi");

            eep.saveUserStruct( buf, ser.nbytes );
            eep.printHeadParms("--- See updated header");
        }
        else if( c == 'r' )
        {
            my.install();               // just register parms
            my.f1 = 1.0;
            my.f2 = 2.0;
            my.i1 = 3;
            ser.printParms("--- See user parms");
        }
        else if( c == 'f' )
        {
            eep.fetchHeadParms();
            eep.printHeadParms("--- Fetched Head parms");

            eep.fetchUserStruct( buf, sizeof buf );
            ser.printBuf("--- See fetched buffer", buf );

            ok = ser.decodeParms( buf );
            PF( "Parms decoded (ok:%d)\r\n", ok );            

            ser.printParms("--- See decoded parms");
        }
    }
}
#endif

void loop() 
{
    yield();
}
