#include "cpuClass.h"
#include "oledClass.h"

void e( const char *p )
{
    PF("---[ %-16s ]---\r\n", p );     // skip control character
}   

CPU cpu;
OLED oled;

void setup()
{
    cpu.init();
 
    e( oled.dsp( O_LED130 ) );                      // Initialze SSD1306 OLED dsp

    e(oled.dsp( O_CLEAR ) );
    e(oled.dsp( O_BRIGHT1 ) );                         // this is O_BRIGHTn
    e(oled.dsp( 0, "\aLeft Justified" ) );
    delay( 1000 );
    e(oled.dsp( 1, "\t\aCentered" ));
    delay( 1000 );
    e(oled.dsp( 2, "\bBoldLeft" ));
    delay( 1000 );
    e(oled.dsp( 4, "\t\vVert Centered" ));
    delay( 1000 );
    e(oled.dsp( 6, "Last Line" ));
    delay( 3000 );

    e(oled.dsp( O_BRIGHT2 ) );                         // this is O_BRIGHTn
    delay( 3000 );

    e(oled.dsp( O_BRIGHT3 ) );                         
    delay( 3000 );

    e(oled.dsp( O_CLEAR ) );
    e(oled.dsp( 0, "\vFloat Entries" ) );
    e(oled.dsp( 2, "" ));
    e(oled.dsp( 3, "\vSeeN: " ));
    e(oled.dsp( 6, "SeeM: " ));
    
    for( int i=20; i>0; i-- )
    {
        e(oled.dsp( 3, 8, "%-2d", i ));
        e(oled.dsp( 6, 8, "%-3d", i+90 ));
        delay( 500 );
    }
    delay( 2000 );
    
    e(oled.dsp( O_CLEAR ));
    for( int i=20; i>0; i-- )
    {
        e(oled.dsp( 0, "\aN=%d", i ));
        delay( 200 );
        e(oled.dsp( 1, "\t\vM=%d", i ));
        delay( 200 );
        e(oled.dsp( 3, "\t\vV:%d", i ));
        delay( 200 );
        e(oled.dsp( 5, "\t\bB:%d", i ));
        delay( 200 );
        e(oled.dsp( 7, "\aLast Line %d", i ));
        delay( 200 );
    }


}

void loop()
{
	;
}
