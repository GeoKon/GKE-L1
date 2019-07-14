/*
 * 0 Tests simple gets() and prompt
 *   
 *   THE FOLLOWING NEED TO BE RETESTED!
 * 1 Minimal setup() interaction example
 * 2 non-blocking interaction example
 * 3 heap test

Sketch uses 282160 bytes (27%) of program storage space. Maximum is 1044464 bytes.
Global variables use 29948 bytes (36%) of dynamic memory, leaving 51972 bytes for local variables. Maximum is 81920 bytes.
Uploading 286304 bytes 
*/

#include "cpuClass.h"
#include "cliClass.h"       // includes cpuClass.h and ESP8266WiFi.h

// --------------------- allocation of classes ------------------------------------
    CPU cpu;
    CLI cli;
    EXE exe;
    
    void myhelp( int n, char *arg[] ) { exe.help( n, arg ); }
    
    extern CMDTABLE t1[];     // forward references
    extern CMDTABLE t2[];

// ------------------------------- MAIN setup() -----------------------------------------
static int sel = 0;
void setup(void) 
{
    cpu.init();
    
    exe.registerTable( t1 );
    exe.registerTable( t2 );
    exe.printTables( "See all tables" );  // instead of PF, you can optionally specify a Buf

    sel = atoi( cpu.prompt("Enter 0 for console test, 1 for buffered responses:" ) );
    
    if( sel )
        cli.init( ECHO_ON, "cmd-buf:" );
    else
        cli.init( ECHO_ON, "cmd-prn:" );
    cli.prompt();
}

// ------------------------------- MAIN loop() -----------------------------------------
BUF buf(400);
void loop()
{
    if( !sel )
    {
        if( cli.ready() )
        {
            char *cmd = cli.gets();                     // cmd points to the heap allocated by CLI
            PF("(PRN) Command entered: %s\r\n", cmd );
            exe.dispatchConsole( cmd );                // using Serial.printf(); unbuffered version
            cli.prompt();
        }
    }
    else
    {
        if( cli.ready() )
        {            
            char *cmd = cli.gets();                     // cmd points to the heap allocated by CLI
            PF("(BUF) Command entered: %s\r\n", cmd );
            exe.dispatchBuf( cmd, buf );                // using Serial.printf(); unbuffered version
            buf.print();                                // display the buffer
            
            PRN("OK");
            PF("Maxsiz=%d, used=%d\r\n", buf.size(), buf.length() );
            cli.prompt();
        }
    }
}

// ------------------------------- CLI Handlers -----------------------------------

// Example of a handler which can work either with dispatchConsole() or dispatchBuf()
void fnc1( int n, char **arg )
{
    BUF *bp = (BUF *)arg[0];
    char *p = "Example of function1\r\n";
    if( bp == NULL )
        PR( p );
    else
        bp->set( p );
}

// Example of a handler which can only work with dispatchBuf()
void fnc2( int n, char **arg  )
{
    BUF *bp = (BUF *)arg[0];
    
    if( bp==NULL ) { PR( bufferedOnly ); return; }

    if( n<=1 )
        {PR( missingArgs ); return;}
    
    bp->add("In function FUNC2\r\n" );
    
    for( int i=1; i<n; i++ )
        bp->add("arg[%d] = %s\r\n", i, arg[i] );
}

// Example of a handler using the BINIT macro
void fnc3( int n, char **arg )
{
    BINIT(bp, arg);
    bp->set("In function FUNC3\r\n");
}

// Example of a handler using the BINIT macro
void fnc4( int n, char **arg  )
{
    BINIT( bp, arg );
    for( int i=1; i<n; i++ )
        bp->add("arg[%d] = %s\r\n", i, arg[i] );
}

// ------------------------------- Table Definitions -----------------------------------
CMDTABLE t1[]
{
  {"h",    "[select] help",                                        myhelp },
  {"fn1", "Calls function-1. Console output",                       fnc1 },
  {"fn2", "arg1 arg2 ... argN. Either console or buffer output",    fnc2 },
  {NULL, NULL, NULL}
};
CMDTABLE t2[]
{
  {"fn3", "Calls function-3. Buffered only",                        fnc3 },
  {"fn4", "arg1 arg2 ... argN. Buffered only",                      fnc4 },
  {NULL, NULL, NULL}
};
