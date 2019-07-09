    #include "ads15Class.hpp"
	#include "cpuClass.hpp"

CPU cpu;	
ADS15 ads( 0x48 );            // ADDR pin to GND
	
void setup(void) 
{
    cpu.init();
    pinMode( 14, OUTPUT );

    ads.begin();
    ads.init( 0, FS_4096mV );
    ads.init( 1, FS_4096mV );
    ads.init( 2, FS_4096mV );
    ads.init( 3, FS_4096mV );
    ads.init( 4, FS_4096mV );
     
    for(;; )
    {
        int v[5];
        for( int i=0; i<5; i++ )
        {
            ads.startConversion( i );
            v[i] = ads.readConversion();
        }
        for( int i=0; i<5; i++ )
            PF("[%d]:%5.3fV\t", ads.toVolts( v ) );    
        PF("\r\n");
        delay(1000);
    }
}
void loop()
{
		yield();
}