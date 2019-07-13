#include <cpuClass.h>
#include <ticClass.h>

CPU cpu;
uint32_t T0;

void setup() 
{
    cpu.init();
    T0 = millis();
}

TICms t( 1000 );
void loop() 
{
    if( t.ready() )
    {
        uint32_t T1 = millis();
        
        PF("every 1sec %ld\r\n", millis()-T0 );     // every loop
        if( t.countMod( 2 )  )                           // every 2nd loop
        {
            PF("every 2sec %ld. ", millis()-T0 );
            delay( 240 );
            PF("Elapsed = %d\r\n", millis()-T1 );
        }
        if( t.countMod( 10 ) )                         // every 10th loop
        {
            PF("60th %ld\r\n", millis()-T0 );
            delay( 1010 );
        }
        if( t.overrun() )
            PF("OVERUN\r\n");
    }
}
