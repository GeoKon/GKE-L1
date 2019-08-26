/*
 * This example tests the Named Parameter Class
 */

#include "cpuClass.h"
#include "cliClass.h"       // includes cpuClass.h and ESP8266WiFi.h
#include "nmpClass.h"

// --------------------- allocation of classes ------------------------------------
    CPU cpu;
    CLI cli;
    EXE exe;
    NMP nmp;
    
    extern CMDTABLE mytable[];     // forward references

// ------------------------------- MAIN setup() -----------------------------------------

struct
{
    float fp1;
    float fp2;
    int i1;
    int i2;
    char s1[USER_STR_SIZE];    
} gp = {1.0, 2.2, 10, 20, "test"} ;


void setup(void) 
{
    cpu.init();
    
    exe.registerTable( mytable );
    exe.printTables( "See Tables" );  // instead of PF, you can optionally specify a Buf

    strcpy( gp.s1, "This is a test" );
    
    nmp.registerParm( "fp1", 'f', &gp.fp1, "info fp1", "%5.1f" );
    nmp.registerParm( "fp2", 'f', &gp.fp2, "info fp2" );
    nmp.registerParm( "i1",  'd', &gp.i1,  "info i1", "%2d"  );
    nmp.registerParm( "i2",  'd', &gp.i2,  "info i2" );
    nmp.registerParm( "s1",  's',  gp.s1,  "info s1" );

    // Clear Meguno
    PFN( "{UI|SET|help.Text=%s}n", " " ); 
    PFN( "{TABLE:CONFIG|CLEAR}"); 
    
    cli.init( ECHO_ON, "cmd:" );
    cli.prompt();
}
void loop()
{
    if( cli.ready() )
    {
        char *cmd = cli.gets();                     // cmd points to the heap allocated by CLI
        exe.dispatchConsole( cmd );                // using Serial.printf(); unbuffered version
        cli.prompt();
    }
    yield();
}

// ------------------------------- CLI Handlers -----------------------------------

void myhelp( int n, char *arg[] ) 
{ 
//    arg[0] = NULL;
//    exe.help( 1, arg );    // print help on console

    BUF temp(256);

    arg[0] = (char *)&temp;
    exe.help( 1, arg );           // save into buffer
    //temp.print();               // show buffer
    PF( "{UI|SET|help.Text=%s}\r\n", temp.c_str() );     
}

void fnc1( int n, char **arg )
{
    nmp.printAllParms("See all parms");
}

void fnc2( int n, char **arg  )
{
    nmp.printMgnInfo( "CONFIG", "", "Table" );
    nmp.printMgnAllParms("CONFIG");
}
void fnc3( int n, char **arg )
{
    if( n<=1 )
        PRN("Missing arg");
    else
    {
        for( int i=1; i<n; i++ )
            nmp.printParm( arg[i] );

        nmp.printMgnInfo( "CONFIG", "", "Table Updated" );
        for( int i=1; i<n; i++ )
            nmp.printMgnParm( "CONFIG", arg[i] );
            
    }
}
void set( int n, char **arg )
{
    if( n<=2 )
    {
        nmp.printMgnInfo( "CONFIG", "", "Missing args" );
    }
    else
    {
        nmp.printMgnInfo( "CONFIG", arg[1], "Updated" );
        nmp.setParmByStr( arg[1], arg[2] );
        nmp.printMgnParm( "CONFIG", arg[1] );
    }
}
// ------------------------------- Table Definitions -----------------------------------
CMDTABLE mytable[]
{
  {"h",    "[select] help",                     myhelp },
  {"pp",   "prints parms",                       fnc1 },
  {"pm",   "prints meguno",                      fnc2 },
  {"p",    "p1 p2 ... Prints given parms",       fnc3 },
  {"set",  "name value. Sets parameter",         set },
  {NULL, NULL, NULL}
};
