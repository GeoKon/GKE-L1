#include "bufClass.h"     // this includes Arduino.h
#include "cpuClass.h" 	// this includes Arduino.h
#include "cliClass.h"

// ----------- class instantiation ------------------------
    CPU cpu;
	CLI cli;
	EXE exe;
// -------------------- Commands ---------------------------

// B32(b);
BUF b(32, "mybuf");

#define PROTO [](int n, char **arg)
CMDTABLE myTable[] = 
{
	{"h", "HELP",               PROTO { exe.help( n, arg );  } },
	{"i", "Initialize buffer",  PROTO {	b.init(); }},
	{"r", "Rebase/append",      PROTO {	b.append(); }},

	{"s", "str. Set string",    PROTO
                			    {
                    			  	if( exe.missingArgs(1) ) return;
                    			  	b.set("%s", arg[1] );
                			    } },
    {"p", "str. Set string pntr",PROTO
                                {
                                    if( exe.missingArgs(1) ) return;
                                    b.pset("%s", arg[1] );
                                } },
	{"c", "str. Copy string",   PROTO
                                {
                                	if( exe.missingArgs(1) ) return;
                                	b.copy( arg[1] );
                                } },
	{"a", "str. Add string",    PROTO
                                {
                                	if( exe.missingArgs(1) ) return;
                                	b.add("%s", arg[1] );
                                } },
	{"d", "Display/Print this", PROTO {	b.print("B is: "); }},
			  
	{NULL,"",NULL}
};
// ---------------- main code -----------------------------
void setup()
{
    cpu.init();  

    exe.registerTable( myTable );
    cli.init( ECHO_ON, "cmd: ");
    cli.prompt();
}
void loop()
{
	if( cli.ready() )
	{
		exe.dispatchConsole( cli.gets() );
		cli.prompt();
	}
}
