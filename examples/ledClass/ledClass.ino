// Example of using the PATTERN class

#include <cpuClass.h>
#include <cliClass.h>
#include <ledClass.h>

CPU cpu;
CLI cli;
EXE exe;
LED led;

extern CMDTABLE myTable[];

void setup() 
{
	cpu.init();
	PRN("Starting");
    led.pin( cpu.getLedPin() );  
    
#ifdef LED_EXTENDED
    PRN("Blink for 10 sec");
    led.tmout( 10 );
    while( led.blink() )
        ;

    PRN("Blink for 12 sec with pattern");
    led.pattern("$*..*......", 12 );
    while( led.blink() )
        ;

    PRN("Blink 10 times"); // use previous pattern
    led.pattern("**......");
    led.count( 10 );
    while( led.blink() )
        ;
   
    PRN("Blink for 5 sec");
    led.pattern("*.");
    led.tmout( 5 );
    while( led.blink() )
        ;
#endif
	exe.registerTable( myTable );
	cli.init( ECHO_ON, "cmd:" );
	cli.prompt();

    PRN("Entering loop");
}

static bool condition = true;
void loop() 
{
	bool ok;
    	
	if( cli.ready() )
    {
        char *cmd = cli.gets(); 
        exe.dispatchConsole( cmd );
        cli.prompt();
        if( led.blinking() )
            CRLF();
    }
    #ifdef LED_EXTENDED
    ok = led.blink( condition );
    #else
    ok = led.blinker( condition );
    #endif

    if( led.aborted() ) 
        PFN("Aborted");
    if( led.expired() ) 
        PFN("Expired");


    yield();
}
void cliPattern( int n, char *arg[] )
{
    if( n>1 )
    {
        static char pattern[32];
        strcpy( pattern, arg[1] );
        if( n>2 )       
            led.pattern( (const char *)pattern, atoi( arg[2] ) );
        else
            led.pattern( (const char *)pattern );
    }
    else
        led.stop(); 
}
void cliDefer( int n, char *arg[] )
{
    if( n>1 )
    {
        static char prompt[64];
        strcpy( prompt, arg[1] );
        led.deferPrompt( (const char *)prompt );
    }
    else
        led.deferPrompt( "" );
}

#ifdef LED_EXTENDED

#endif

CMDTABLE myTable[]
{
  {"h",    "[select] help", 	[]( int n, char *arg[]) { exe.help( n, arg ); } },

  {"cpuoff",  "Turn LED off",      []( int n, char *arg[]) { cpu.led(OFF); } },
  {"cpuon",   "Turn LED on",    []( int n, char *arg[]) { cpu.led(ON); } },

  {"off",  "Turn LED off",    []( int n, char *arg[]) { led.off(); } },
  {"on",   "Turn LED on",       []( int n, char *arg[]) { led.on(); } },

  {"true",  "Set Condition=true",    []( int n, char *arg[]) { condition=true; } },
  {"false", "Set Condition=false",   []( int n, char *arg[]) { condition=false; } },
  {"stop",  "stop()",           []( int n, char *arg[]) { led.stop(); } },
  {"pattern",  "pattern [times]", cliPattern },
  {"defer",    "prompt. Defer prompt", cliDefer},
  
#ifdef LED_EXTENDED
  {"count", "Set count()",       []( int n, char *arg[]) { led.count( n>1?atoi(arg[1]):1 );  } },
  {"tmout", "Set timeout()",     []( int n, char *arg[]) { led.tmout( n>1?atoi(arg[1]):1 );  } },
#endif
  {NULL, NULL, NULL}
};
