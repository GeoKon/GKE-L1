#include "cpuClass.h"
#include "oledClass.h"

//#define P(A,vargs) Serial.printf(A,vargs); Serial.printf("\r\n");

void e( const char *p )
{
    PF("---%s\r\n", (*p<' ')? ++p: p );     // skip control character
}   // print display

CPU cpu;
OLED oled;

void setup()
{
  cpu.init();
 
  oled.init( OLED130 );                      // Initialze SSD1306 OLED dsp
  
  char s[64];
  oled.setBrightness( atoi( cpu.prompt("Enter brightness: ") ));
  
  for( int br=0; br<255; br+=32 )
  {
    oled.setBrightness( br );
    e(oled.dsp( 0, 0, "\aBright %d", br ));
    e(oled.dsp( 1, "\aCentered1" ));
    e(oled.dsp( 2, "\bBold2" ));
    e(oled.dsp( 4, 0, "\vVERTLEFT4" ));
    e(oled.dsp( 6, "\vVERTCENT6" ));
    delay( 1000 );
  }    
  char temp[20];

  for(;;)
  {
    strncpy( temp+1, cpu.prompt("Enter string: "),19);       // get string in temp+1
    oled.clearDisplay();

      temp[0] = '\a'; e(oled.dsp( 1, 0, temp ));
      e(oled.dsp( 0, 0, temp+1 ));
      temp[0] = '\v'; e(oled.dsp( 3, 0, temp ));
      temp[0] = '\b'; e(oled.dsp( 5, 0, temp ));
      delay( 4000 );
          
      temp[0] = '\a'; oled.dsp( 1, temp );
      oled.dsp( 0, temp+1 );
      temp[0] = '\v'; oled.dsp( 3, temp );
      temp[0] = '\b'; oled.dsp( 5, temp );
  }
}

void loop()
{
	;
}
