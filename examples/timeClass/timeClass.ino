//#define TEST1
  #define TEST2
//#define TEST3
//#define TEST4
//#define TEST5

#include <Ticker.h>
#include "cpuClass.h"
#include "timeClass.h"

CPU cpu;
TIME tm;            // used in TEST2
Ticker tk;          // used only on TEST2 and TEST3

#ifdef TEST1        // ------------------- ModuloTic() test ---------------
ModuloTic tic( 5 ); 
void setup() 
{
    cpu.init();
}
void loop()
{
    if( tic.ready() )
        PR("5sec elapsed");    
}
#endif

#ifdef TEST2        // -------------------- HH:MM:SS TEST -----------------
void setup() 
{
  cpu.init();
  tm.init( 4, 3, 10 );

  tm.setTime( 0,0,0 );
  tm.setDate( 2018, 6, 26 );  PR( "Expect [Tue]. " + tm.getDateVerbose() );
  tm.setDate( 2019, 7, 26 );  PR( "Expect [Fri]. " + tm.getDateVerbose() );
  tm.setDate( 2020, 8, 26 );  PR( "Expect [Wed]. " + tm.getDateVerbose() );
  tm.setDate( 2021, 9, 26 );  PR( "Expect [Sun]. " + tm.getDateVerbose() );
  tm.setDate( 2018, 6, 29);
  
  tk.attach( 1.0, [](){ tm.update(); } );
  PR("Timer Started");
}
void loop() 
{
  if( tm.ready( MINUTE_BEGIN ) )                // AUTO_RESET by default
    PR( "Minute begin " + tm.getTimeString() );
  if( tm.ready( HOUR_BEGIN ) )
    PR( "Hour begun" );
  if( tm.ready( DAY_BEGIN ) )
    PR( "DAY begun");
  delay(5);
}
#endif

#ifdef TEST3        // ------------------- ModuloTic() test ---------------

ModuloSec tcs( tm );    // pass the timeClass()
void setup() 
{
    cpu.init();
    tm.init();          // initialize timer

    tm.setTime( 0,0,0 );
    tm.setDate( 2018, 6, 26 );
 
    tk.attach( 1.0, [](){ tm.update(); } );
    PR("Timer Started");
}
void loop()
{
    if( tcs.ready( 4, 1 ) )
        PF("Time every 4-sec, offset 1: %s\r\n", !tm.getTimeString() );    
}
#endif

#ifdef TEST4        // -------------------- HH:MM:SS TEST MEASURES DELAY -----------------
TmCount measure;
int lpcount=0;

void setup() 
{
  cpu.init();
  tm.init( 4, 3, 10 );
  tm.setTime( 0,0,0 );
  tm.setDate( 2018, 6, 26 );  PR( "Expect [Tue]. " + tm.getDateVerbose() );
  tk.attach( 1.0, [](){ tm.update(); } );
  PR("Timer Started");
}
void loop() 
{
  measure.startT();
  if( tm.ready( MINUTE_BEGIN ) )                // AUTO_RESET by default
    PR( "Minute begin " + tm.getTimeString() );
  if( tm.ready( HOUR_BEGIN ) )
    PR( "Hour begun" );
  if( tm.ready( DAY_BEGIN ) )
    PR( "DAY begun");
  delay(5);
  measure.stopT();
  lpcount++;
  if( lpcount > 100 )
  {
    lpcount = 0;
    PF( "Elapsed-Hour = %d\r\n", measure.deltaT() );
    PF( " Max Elapsed = %d\r\n", measure.deltaMax() );
    //measure.resetMax()
  }
  delay(10);
}
#endif

#ifdef TEST5        // ------------------- ModuloTic() test ---------------

ModuloHrs tch( tm );    // pass the timeClass()
void setup() 
{
    cpu.init();
    tm.init( 4, 3, 10 );          // fast timer

    tm.setTime( 0,0,0 );
    tm.setDate( 2018, 6, 26 );
 
    tk.attach( 1.0, [](){ tm.update(); } );
    PR("Timer Started");
}
void loop()
{
    if( tch.ready( 2 ) )         // every 2-hours
        PF("Time every 2-hrs: %s\r\n", !tm.getTimeString() );    
}
#endif
