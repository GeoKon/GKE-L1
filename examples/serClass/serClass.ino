#include "serClass.h"   
/*
 * Sketch uses 282540 bytes (27%) of program storage space. Maximum is 1044464 bytes.
 * Global variables use 30016 bytes (36%) of dynamic memory, leaving 51904 bytes for local variables. Maximum is 81920 bytes.
 * Uploading 286688 bytes 
 */
// ------------------- allocation of base classes ------------------------------
    CPU cpu;
    SER ser;
// -----------------------------------------------------------------------------

    float f1=4.56;
    float f2=123.0;
    int i1=10;
    char cp[20]="TestLine";

void myInit()
{
    ser.resetRegistry();
    ser.registerParm( "f1", 'f', &f1, "=%4.2f Volts" );
    ser.registerParm( "f2", 'f', &f2, "=%.2f amps" );
    ser.registerParm( "i1", 'd', &i1, ""  );
    ser.registerParm( "cp", 's', &cp[0], "=[%s]" );
    ser.registryStatus();}

void setup()
{
    cpu.init();
    bool ok;
   
    for(;;)
    {
        char *s = cpu.prompt( "\r\nPress i=initialize, p=print, m=modify, e=encode: " );
        char c = s[0];
        PF("\r\n");
        if( c == 'i' )          // initialize parms
        {
            myInit();
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
            if( ok ) PF( "f1=%f\r\n", fv );
            else PR("Cannot set f1");

            int  iv = 20;
            ok = ser.setParmValue( "i1", &iv );
            if( ok ) PF( "i1=%d\r\n", iv );
            else PR("Cannot set i1");

            ok = ser.setParmValue( "cp", (void *)"NEW" );
            if( ok ) PF( "cp=%s\r\n", "NEW" );
            else PR("Cannot set cp");
        }
        else if( c == 'e' )
        {
            byte buf[200];
            
            ok = ser.encodeParms( buf, 200 );
            if( ok ) PF( "%d-parms encoded, %d-bytes\r\n", ser.nparms, ser.nbytes );
            else PF("Encoding error\r\n" );

            ok = ser.decodeParms( buf );
            if( ok ) PF( "%d-parms decoded\r\n", ser.nparms );            
            else PF("Decoding error\r\n" );

            ser.printParms("--- See decoded parms");
        }
    }        
}
 void loop() 
{
    yield();
}
