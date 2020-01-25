CLI Class
---------

This class is to be used to receive a serial stream from the ESP `Serial` port in an asynchronous (non-blocking) fashion, with the usual single line editing of backspace. 

**Public class methods**

```c++
CLI( int nchar=80 );	// default heap allocation for input buffer

void init( echo_t echo1, const char *prompt=NULL );
void prompt();
bool ready();
char *gets();
```

**Usage**

Allocate the CLI class in an appropriate scope, typically globally as:

	CLI cli;

Initialize in `setup()` using `cli.init( echo, prompt )` where `echo` is either `ECHO_ON` (`true`) or `ECHO_OFF` (`false`). The `prompt' can be also set accordingly. Example:

```c++
cli.init( ECHO_ON, "cmd:");
```

In `loop()`, check if a line has been received using the `cli.ready()` boolean function. If a line is received, you can use `cli.gets()` to access the line. After completing processing, call `cli.prompt()` to print the prompt, if previously
defined in `setup()`. Example:

```c++
if( cli.ready() )
{
	char *p = cli.gets();
	... do something with p...

	cli.prompt();
}
```

See `\examples\cliClass.ino` for a working example.



EXE Class
---------

This class is to be used to parse a string and dispatch to an appropriate
execution function. 

### Public class methods

```c++
EXE();									
void initTables();							// clears previously registered tables		
void registerTable( CMDTABLE *t );			// registers a CMDTABLE
void dispatchConsole( char *s );			// executes command 's' found in CMDTABLEs
void dispatchBuf( char *s, Buf &result );	// same as above but results are buffered
void dispatchBufPtr( char *s, Buf *result );

void printTables( char *prompt = "" );
void help( int n, char *arg[] );			// list of all commands in tables
void brief( int n, char *arg[] );			// brief version of the above

bool missingArgs( int count );				// true if count < n
bool cmd1stUpper();							// true if 1st char of command is upper case

```

### CMDTABLE

Before describing the class, the `CMDTABLE` structure needs to be introduced. `CMDTABLE` is a collection of labels, followed by a help literal, followed by a function. Example:

```c++
CMDTABLE table1[]
{
  {"h",    "[select] help", 						help },
  {"func1", "description of function-1", 			fnc1 },
  {"func2", "arg1 arg2. Description of function-2", fnc2 },
  {NULL, NULL, NULL}
};
```

If you pass a command line (char *, or Buf *) to `dispatch()`, like `func2 10 20` the table is scanned for the `func2` label. Then, the corresponding function `fnc2` is called with the following prototype:

	void fnc2( int nargs, char *args[] );

- `nargs` indicates the number of arguments (3 in this case) 
- `arg[]` is an array of character string pointers.
  - `arg[0]` is either a NULL pointer if `dispatchConsole()` is called, or a pointer to `BUF` if `dispatchBuf()` or `dispatchBufPtr()` is called. See next section for more details.
  - `arg[1]` points to string "10"
  - `arg[2]` points to string '20'
  - pointing to `func2`, `10` and `20` respectively.
    The function may do any level of processing within, print something and return.


### MAIN setup()

Tables are registered with the `exe.registerTable()` method. The EXE class
also includes methods to print the content of the tables. 
For example,

```c++
EXE exe;
void setup()
{
	....
	exe.initTables();					// resets all table entries
    exe.registerTable( table1 );		// registers 1st table
	exe.registerTable( table2 );		// registers 2nd table
	exe.printTables("See Tables");		// prints all commands from all tables
	....
}
```
There are a couple of hard coded limits on this:

```c++
#define MAX_TENTRIES 10	// max number of command tables
#define MAX_TOKENS 10	// max number of tokens in a command line
#define MAX_INPCMD 80	// max size of a command line
```

### MAIN loop() 

There are two ways to dispatch commands in the main loop: using the `exe.dispatchConsole()` and `exe.dispatchBuf()`. The operation of the first is shown by the following example:

```c++
void loop()
{
	if( cli.ready() )
	{
		char *p = cli.gets();		// get pointer to user entered string
		exe.dispatchConsole( p );	// execute CLI based using registered tables
		cli.prompt();
	}
}
```

The dispatcher calls the various handlers using as arg[0] equal to NULL. So, handlers must use the usual PR() or PF() functions to respond to the console command. An example of a typical handler is shown below:

```c++
// Assume that user entry is 'test 10 20.3' with the first argument required but 
// optional last argument

void test( int n, char *args[] )
{
	if( exe.missingArgs(1) )			// prints an error message if 'n' is less or
		return;							//   equal to 1.
	int n1 = atoi( args[1] );			// the first argument

	// If second argument is given, convert it to float. Otherwise use 0.0 as default
	float n2 = n>2 ? atof( args[2] ) : 0.0;

	PFN("You entered %d, %f", n1, n2 );
}
```

In addition to using the `dispatchConsole()`, the `dispatchBuf()` can be used to process buffer strings. This is illustrated below:

```c++
BUF buf(1024);						// allocation of buffer in the HEAP
void loop()
{
	if( cli.ready() )
	{
		char *p = cli.gets();		// get pointer to user entered string
		exe.dispatchBuf( p, buf );	// execute 's' based using registered tables
		PR( buf.c_str() );			// print buffer as stored in 'buf' by the handlers
	}	
}
```

The dispatcher calls the various handlers using agrs[0] = &buf. So, can handlers could use this pointer to save the results. An example of a typical handler is shown below:

```c++
// Buffered version of previous handler

void test( int n, char *args[] )
{
	BUF b = (BUF *)arg[0];				// see below for BINIT() macro definition
	if( b == NULL )						// included in macro
	{
		... code using PR( format ... );
	}							
	else
	{
		... code using BUF::set( format, ... ) and BUF::add( format ...);
	}
}
```

The above is simplified using two useful macros:

- **BINIT()** defined as: `BINIT( Bp, Arg) BUF *Bp = (BUF *)Arg[0]` and 
- **RESPOND()** defined as: 

`RESPOND( Bp, A, ... ) if( Bp ) Bp->add(A, ##__VA_ARGS__ ); else PF(A, ##__VA_ARGS__)`

Using the above macros the code is simplified as follows:

```c++
void test( int n, char *args[] )
{
	BINIT( b, args );
	......
	RESPOND( b, "See args[1]: %d\r\n", n1 );
	RESPOND( b, "see args[2]: %f\r\n", n2 );
}
```

This command handler can be used either when executing dispatchConsole() or dispatchBuf() and use either the `PF()` function or the `BUF::add()` function respectively.

See `\examples\cliClass.ino` for an example.		

### MegunoLink Handlers() 

Command line interface can be very cumbersome to use since it requires extensive typing. A more intuitive interface can be built using the MegunoLink library. To be able to handle either text screen output or specially formatted MegunoLink outputs, ordinarily one could use two entries in the command table. For example, using

```c++
CMDTABLE table1[]
{
  ...
  {"func",  "description of command line function",	cliFunc },
  {"!func", "description of GUI MegunoLink function", mgnFunc },
  ...
  {NULL, NULL, NULL}
};
```

Because the logic of both `cliFunc()` and `mgnFunc()` are very similar, it is desired to use the same function to handle both case. To do so, one can use `exe.cmd1stUpper()` to discriminate if the user has entered a command starting with a lower case letter (i.e. command line) or upper case letter, implying a MegunoLink command. The following example shows how to use this:

```c++
void test( int n, char *args[] )
{
	BINIT( b, args );
	......
	if( exe.cmd1stUpper() )
		PR( "[TABLE|label|%d]\r\n", n1 );
	else
		RESPOND( b, "See args[1]: %d\r\n", n1 );
}
```

This way, the `CMDTABLE` is simplified with only one command entry, as:

```c++
	...
	{"func",  "description of function",	test }
	...
```