#define TEST1

#include <cpuClass.h>
#include <timeClass.h>

#include <OneWire.h>
#include "ds18Class.h"

CPU cpu;
ModuloTic tic(2);   // one tic per second
    
OneWire  ds(4);     // on pin D2 of the NodeMCU
DS18 temp( &ds );    // associate DS18B20 class with OneWire

#ifdef TEST1
void setup() 
{
    cpu.init();
    temp.init( 3, true );               // look for 3-devices with debug enabled
}
void loop() 
{
    if( tic.ready() )
        temp.start( temp.thisID() );   // specify index or use thisID()

    if( temp.ready() )
    {
        temp.read();
        PF("ID:%d Err:%d Temp=%.1f°C\t%.1f°F\r\n", 
            temp.thisID(), temp.error(), temp.getDegreesC(), temp.getDegreesF() );
        temp.nextID();
    }
}
#endif
#ifdef TEST2
FLT filter(3);

void setup() 
{
    cpu.init();
    temp.init( 3, true );
    
    char *p = cpu.prompt("Enter coef: ");
    filter.setCoef( atof( p ) );        
    temp.simulate( true, 0.0 );
}
void loop() 
{
    if( tic.ready() )
        temp.start( 0 );   // specify index or use thisID()

    if( cpu.buttonPressed() )
        temp.simulate( true, 100.0 );
        
    if( temp.ready() )
    {
        temp.read();
        filter.smooth( 0, temp.getDegreesC() );
        
        PF("Temp=%5.1f°C\tsmoothed=%5.1f°C\r\n", temp.getDegreesC(), filter.getValue(0) );
    }
}
#endif
