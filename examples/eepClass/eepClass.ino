#include "eepClass.h"   
#include <FS.h>

// ------------------- allocation of base classes ------------------------------
    CPU cpu;
    EEP eep;
// -----------------------------------------------------------------------------

void setup()
{
    cpu.init();
    SPIFFS.begin();                     // crashes sometimes if not present
    
    for(;;)
    {
        Buf s = cpu.prompt( "Press i=initialize, d=display, r=increment reboot counter, m=modify wifi:" );

        char c = *s.pntr;
        if( c=='i' )
        {
            eep.initWiFiParms();                    // use the defaults
            eep.printParms("See parms\r\n");
        }
        else if( c == 'd' )
        {
            eep.fetchParms();
            eep.printParms("Fetched parms\r\n");
        }
        else if( c == 'r' )
        {
            eep.updateRebootCount();
            eep.printParms("Updated parms\r\n");
        }
        else if( c == 'm' )
        {
            eep.updateWiFiParms( "new SSID", "new pwd" );
            eep.printParms("New WiFi parms\r\n");
        }
            
    }
}
void loop() 
{
    yield();
}
