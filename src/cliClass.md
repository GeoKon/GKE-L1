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

### Public class methods

    EXE();									
	void registerTable( CMDTABLE *t );
	void printTables( char *prompt = "" );
	void help( int n, char *arg[] );
	void getHelp( char *mask, Buf &temp );
	
	void dispatchConsole( char *s );		
	void dispatchBuf( char *s, Buf &result );
	
### CMDTABLE

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

The argument `args[0]` has special purpose and will be described later. 


### MAIN setup()

Tables are registered with the `exe.registerTable()` method. The EXE class
also includes methods to print the content of the tables. 
For example,

	EXE exe;
	void setup()
	{
		....
		exe.registerTable( table1 );
		exe.registerTable( table2 );
		exe.printTables("See Tables");
		....
	}
There are a couple of hard coded limits on this:

	#define MAX_TENTRIES 10	// max number of command tables
	#define MAX_TOKENS 10	// max number of tokens in a command line
	#define MAX_INPCMD 80	// max size of a command line

### MAIN loop() 

There are two ways to dispatch commands in the main loop: using the `exe.dispatchConsole()` and `exe.dispatchBuf()`. The operation of the first is shown by the following example:

	void loop()
	{
		if( cli.ready() )
		{
			char *p = cli.gets();			// get pointer to user entered string
			exe.dispatchConsole( p );		// execute CLI based using previously registered tables
			cli.prompt();
		}
	}

The dispatcher calls the various handlers using as agrs[0] = NULL. So, handlers must use the usual PR() or PF() functions to respond to the console. An example of a typical handler is shown below:

	// Assume that user entry is 'test 10 20.3' with the first argument required but the last argument optional

	void test( int n, char *args[] )
	{
		if( n<=1 )
		{
			PR( missingArgs );		// prints an error message
			return;
		}
		int n1 = atoi( args[1] );			// the first argument

		float n2 = 0.0;						// default value for 2nd argument
		if( n>2 )							// if provided, extract 2nd argument
			n2 = atof( args[2] );
		
		PFN("You entered %d, %f", n1, n2 );
	}

In addition to using the console, the `exe.dispatchBuf()` can be used to process buffer strings. This is illustrated below:

	BUF buf(1024);							// allocation of buffer in the HEAP
	void loop()
	{
		if( cli.ready() )
		{
			char *p = cli.gets();			// get pointer to user entered string
			exe.dispatchBuf( p, buf );		// execute 's' based using previously registered tables
			PR( !buf );						// print buffer as stored in 'buf' by the handlers
		}	
	}

The dispatcher calls the various handlers using agrs[0] = &buf. So, can handlers could use this pointer to save the results. An example of a typical handler is shown below:

	// Buffered version of previous handler

	void test( int n, char *args[] )
	{
		BUF b = (BUF *)arg[0];				// see below for BINIT() macro definition
		if( b == NULL )						// included in macro
		{
			PR( bufferedOnly );
			return;
		}									// last line of BINIT()

		if( n<=1 )
		{	
			b->set( missingArgs );
			return;
		}

		int n1 = atoi( args[1] );			// the first argument

		float n2 = 0.0;						// default value for 2nd argument
		if( n>2 )							// if provided, extract 2nd argument
			n2 = atof( args[2] );
		
		b->set("You entered %d, %f", n1, n2 );
	}

Using the macro BINIT(), the same code as above is implified as follows:

	void test( int n, char *args[] )
	{
		BINIT( b, args );
	
		if( n<=1 )
		{	
			b->set( missingArgs );
			return;
		}
		... etc ...
	}

See `\examples\cliClass.ino` for an example.		