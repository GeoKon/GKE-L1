#include "cpuClass.h"
#include "ads15Class.h"

CPU cpu;	
ADS15 ads;            // ADDR pin to GND
	
void setup(void) 
{
    cpu.init();
    pinMode( 14, OUTPUT );

    ads.init( 0x48 );
    ads.initChannel( 0, FS_4096mV );
    ads.initChannel( 1, FS_4096mV );
    ads.initChannel( 2, FS_4096mV );
    ads.initChannel( 3, FS_4096mV );
    ads.initChannel( 4, FS_4096mV );
     
    for(;; )
    {
        int v[5];
        for( int i=0; i<5; i++ )
        {
            ads.startConversion( i );
            v[i] = ads.readConversion();
        }
        for( int i=0; i<5; i++ )
            PF("[%d]:%5.3fV\t", i, ads.toVolts( i, v[i] ) );    
        CRLF();
        delay(1000);
    }
}
void loop()
{
		yield();
}
