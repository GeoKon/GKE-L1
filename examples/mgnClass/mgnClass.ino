#include "cpuClass.h"
#include "mgnClass.h"   // this included bufClass.h

#define OK line(__LINE__)
void line( int n )
{
    PFN("Line %d", n );    
}
CPU cpu;
MGN mg;
void setup()
{
    cpu.init();
    
    PRN("Hello World");
    BUF t(256);
    OK;
    
    mg.init( &t, "MC" );
    OK;
    
    mg.tableSet("Time", "[Now()]");
    OK;

    mg.tableSet("Voltage", 20.3f, "mV");
    OK;
    
    mg.tableSet("Current", 2, "A");
    OK;
    
    mg.tableClear( "Voltage");
    OK;
    
    mg.tplotData( "Voltage", 29);
    OK;
    
    mg.tplotData( "Current", 30.34);
    OK;
    
    mg.tplotYRange( -10, 23.45);
    OK;
    
    mg.tplotSet( "Title=Garbage");
    OK;    
    
    mg.controlSetText( "button", "ACTIVE");
    OK;
    
    mg.controlSetValue( "bar", 20);
    OK;
    
    mg.controlSetValue( "bar", 20.345f);
    OK;
    
    mg.controlSetCheck( "trace", true );
    OK;
    
    mg.print();
    PFN("Buffer size used=%d max=%d", t.length(), t.size() );
}
void loop()
{
    yield();
}
