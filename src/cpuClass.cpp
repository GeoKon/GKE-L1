// See cpuClass.h for all class definitions
// updated

#include <ESP8266WiFi.h>
#include "cpuClass.h"

// CPU cpu; // let main program define this

void die( const char *s1, const char *s2, int s3, const char *s4 )
{
// Serial.printf( "ASSERT(%s) in %s, line %d of %s\r\n", s1, s2, s3, s4 );
    PF( "ASSERT(%s), line %d of %s\r\n", s1, s3, s4 );
  bool flip;
  for(;;)
  {
   digitalWrite( LED, flip );
   if( flip ) flip=false; else flip=true;
   //ESP.wdtDisable();
   //ESP.wdtFeed();
   yield();
   delay(200);
  }
}
