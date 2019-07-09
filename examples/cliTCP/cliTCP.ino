#include "cliClass.h"       // includes cpuClass.h and ESP8266WiFi.h
#include "tcpClass.h"

// --------------------- allocation of classes ------------------------------------
    CPU cpu;
    CLI cli;
	TCP tcp;
	EXE exe;
// ---------------------------------- TEST 1 ------------------------------------

#define RESPONSE(...) exe.respond(__VA_ARGS__)

void fnc1( int n, char **arg )
{
  RESPONSE("In function FUNC1\r\n" );
}
void fnc2( int n, char **arg  )
{
  RESPONSE("In function FUNC2\r\n" );
  for( int i=0; i<n; i++ )
    RESPONSE("arg[%d] = %s\r\n", i, arg[i] );
}
void fnc3( int n, char **arg )
{
  RESPONSE("In function FUNC3\r\n");
}
void fnc4( int n, char **arg  )
{
  RESPONSE("In function FUNC4\r\n" );
  for( int i=0; i<n; i++ )
    RESPONSE("arg[%d] = %s\r\n", i, arg[i] );
}
void help( int n, char **arg )
{
    exe.help( n, arg );
}
CMDTABLE t1[]
{
  {"h",    "[select] help",                         help },
  {"func1", "Calls function-1",                      fnc1 },
  {"func2", "arg1 arg2 ... argN. Call function-2",   fnc2 },
  {NULL, NULL, NULL}
};
CMDTABLE t2[]
{
  {"fn3", "void function3()", fnc3 },
  {"fn4", "void function4()", fnc4 },
  {NULL, NULL, NULL}
};
// --------------------- main program ------------------------------------
static int sel = 0;
void setup(void) 
{
    cpu.init();
    
    exe.registerTable( t1 );
    exe.registerTable( t2 );
    exe.printTables( "See all tables" );  // instead of PF, you can optionally specify a Buf

    cli.init( ECHO_ON, "cmd-buf:" );
    cli.prompt();
    tcp.init( 23, ECHO_ON, "cmd-tcp:" );
}

BUF buf(400);
void loop()
{
	if( cli.ready() )
	{            
	    char *cmd = cli.gets();                     // cmd points to the heap allocated by CLI
	    exe.dispatchBuf( cmd, buf );                // using Serial.printf(); unbuffered version
	    buf.print();                                // display the buffer
	    
	    PR("OK");
	    PF("Maxsiz=%d, used=%d\r\n", buf.maxsiz, buf.length() );
	    cli.prompt();
	}
	if( tcp.ready() )
	{            
	    char *cmd = tcp.gets();                     // cmd points to the heap allocated by CLI
	    exe.dispatchBuf( cmd, buf );                // using Serial.printf(); unbuffered version
	    tcp.respond( !buf );                        // display the buffer
	    
	    tcp.prompt();
	}

}

