Class CLI
---------

This class is to be used to receive a serial stream from the ESP `Serial` 
port in an asynchronous (non-blocking) fashion, with the usual single line 
editing of backspace. 

**Public class methods**

    CLI( int nchar=80 );	// default heap allocation for input buffer

    void init( echo_t echo1, const char *prompt=NULL );
    void prompt();
    bool ready();
    char *gets();

**Usage**

Allocate the CLI class in an appropriate scope, typically globally as:

	CLI cli;

Initialize in `setup()` using `cli.init( echo, prompt )` where `echo` is either 
`ECHO_ON` (`true`) or `ECHO_OFF` (`false`). The `prompt' can be also set accordingly. Example:

	cli.init( ECHO_ON, "cmd:");

In `loop()`, check if a line has been received using the `cli.ready()` boolean 
function. If a line is received, you can use `cli.gets()` to access the line. 
After completing processing, call cli.prompt() to prind the prompt, if previously
defined in `setup()`. Example:

	if( cli.ready() )
	{
		char *p = cli.gets();
		... do something with p...

		cli.prompt();
	}

See `\examples\cliClass.ino` for a working example.



Class EXE
---------

This class is to be used to parse a string and dispatch to an appropriate
execution function. 

**Public class methods**

    EXE();									
	void registerTable( CMDTABLE *t );
	void printTables( char *prompt = "" );
	void getTables  ( char *prompt, Buf &temp );
	void printHelp( char *mask = "" );
	void getHelp( char *mask, Buf &temp );
	
	void dispatch( char *s );				// must use printf()
	void dispatch( Buf &cmd );				
	void dispatch( char *s, Buf &result );	// may or may not use printf()
	void dispatch( Buf &cmd, Buf &result );

Before describing the class, the CMDTABLE structure needs to be introduced. CMDTABLE is a collection of labels, followed by a help literal, followed by
an function. Example:

	CMDTABLE table1[]
	{
	  {"h",    "[select] help", 						help },
	  {"func1", "description of function-1", 			fnc1 },
	  {"func2", "arg1 arg2. Description of function-2", fnc2 },
	  {NULL, NULL, NULL}
	};

If you pass a command line (char *, or Buf *) to `dispatch()`, like `func2 
10 20` the table is scanned for the `func1` label. Then, the corresponding 
function `fnc2` is called with the following prototype:

	void fnc2( int nargs, char *args[] );

where `nargs` indicates the number of arguments (3 in this case) and the
`arg[]` array pointing to `func2`, `10` and `20` respectively.
The function may do any level of processing within, print something and return.

To allow usage of this structure in WEB applications, a buffering option
is provided. The prototype is extended with a third argument, `Buf &bf`. In
this case, the function may use `bf.add()` instead of the typical `printf()`
to accumulate a response. The following example of a main loop is indicative
of the use.

	void loop()
	{
		if( cli.ready() )
		{
			Buf cmd;			// buffer of user input
			cmd = cli.gets();	// save the response in cmd

			Buf resp;					// buffer of response
			exe.dispatch( cmd, resp );	// execute the command pointed by cmd

			... do something with response 'resp' ...
		}
	}

If it totally up to the pointed functions (`fnc2()` in this example) to use
the 3-rd argument or to ignore it and use `printf()`. Typically, a set of
functions use a macro to define the default type of responses. For example:

	#define RESPONSE Serial.printf
		or
	#define RESPONSE bf.add

	void fnc2( int n, char *arg[], Buf &bf )
	{
		RESPONSE( "This is function fnc2\r\n" );
	}

Tables are registered with the `exe.registerTable()` method. The EXE class
also includes methods to print the content of the tables, or get them as buffers. 
For example,

	EXE exe;
	void setup()
	{
		....
		exe.registerTable( table1 );
		exe.registerTable( table2 );
		exe.printTables("See Tables");

		 OR
		Buf mybuf;
		exe.getTables( "See Tables", mybuf );
		mybuf.print();
		....
	}

See `\examples\cliClass.ino` for an example.		