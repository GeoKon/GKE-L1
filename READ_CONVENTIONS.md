Library Files
--------------
** Overview **

- Organized in two levels L1 and L2
- Classes in L1 do not depend on each other (i..e no circulr dependencies) -- with one exception, everyone depends on `cpuClass.h`
- Classes in L2 do not depend on each other (i.e. no circular dependencies), but they do depend on declarations of one or more classes of L1.
- A .cpp (and .h) file may include more than one class
- Classes typically use the following files,
	- .cpp file with class definitions
	- .h file with class declarations
	- .md brief class markdown documentation
	- .ino file with Unit level test code
- Classes are instantiated (allocated) outside of the libraries, typically in the main program

** Naming Conventions **
- Library files are names as `xyzClass` where `xyz` is typically 2, 3 or 4 lower case letters
- Classes are typically declared in uppercase; 3 or 4 letters, such as `class XYZ`
- variables are in lowercase; underline separators are rarely used
- methods/functions follow the camel notation starting with a lower case word, typically a verb.
- macros, defines and enumerations are all in uppercase. Included in the `.h` files

** Include Conventions **
- library .h files 
	- are properly guarded with `#ifndef... #define... #endif`
	- include just the minimum necessary for declaring classes or constants
	- exported class definitions; externsl functions or variables to be used by other modules.
- library .cpp files
	- always include the associated .h file
	- include "just the minimum necessary" header files from other libraries or modules
	- declare forward references used within the module

** Utility Macros **
- `PR()` equivalent to Serial.println(); appends a `RETURN` at the end of the object
- `PN()` equivalent to Serial.print()
- `PF()` equivalent to Serial.printf()
- `ASSERT( condition )` if condition is false, causes a halt with the LED blinking and prints to the serial port the file and line number of the `condition`.

All above have been included in `macros.h` which in turn is included in `cpuClass.h`

Code Design Patterns
--------------------
To best conform to the Arduino `setup()` and `loop()` paradigm, the following design pattern is used:

- Instantiation/allocation of classes as globals or in setup()
- Classes include
	- Constructors for the I/O pins that are not user modifiable during the course of the program
	- `init()` function to define all local parameters needed for the class. Typically calles in `setup()`
	- `soSomething()` non-blocking functions, which can be called either during `setup()` or `loop()`. Such functions can be used to <i>trigger</i> something, or <i>start</i> and asynchronous operation
	- boolean `ready()` function to indicate that the class as completed something triggered previously. 
	- functions to retrieve internal class variables with results.
- The following example illustrates the above conventions

```
	CLI cli;		// instantiate the CLI class
	void setup()
	{	....
		cli.init( ECHO_ON, "cmd: ");	 // initialize local variables
		....
		cli.prompt();					// print previous defined prompt
	}
	void loop()
	{	...
		if( cli.ready() )
			char *p = gets();			// get string
			... do something with p ...
			cli.prompt()				// print prompt after completion
		...
	}
```		
One more example to demonstrate the same design pattern with a timers

```
	ModuleTic tic(2);	// Allocation of trigger class
						 // Triggers every 2 seconds
	void loop()
	{	...
		if( tic.ready() )
			... do something ...
		...
	}
```

Applications
------------
** Overview **
- Applications use the L1 or L2 libraries
- Applications are also layered. The library `WEBLIB` includes common modules used in applications included in `\examples`.
- if more than one application share common pieces of code, such code must be included in an appropriate library and the applications must be placed as `\examples'

** Naming Conventions **
- Applications typically consist of multiple files. They all must start with an Uppercase letter and common root. For example:
	- `ExampleWEB.ino`
	- uses/links with other modules `ExampleAPP.cpp` and `ExampleSTA.cpp`

** Include Conventions **
- Applications share a common header file which defines
	1. all library includes needed for all application files
	2. the extern classes declared in the main module.
	3. the exported functions from the other modules needed for each-other
- The common header file does not include forward references needed for application modules; these are included in the module itself.
- all application modules must include the common header file
- the common header file must be sufficient -- no other includes should necessary in the application modules

** Typical Directory Structure **
The following example shows the tree structure used for applications
```
	\WEBLIB
		|-\src						source code of application libraries
		|   |- SimpleSRV.cpp
		|   |- SimpleSRV.h
		|   |- SimpleSRV.md
		|
		|-\apps
		|	|-\ExampleWEB			application using common files in \src
		|	|	|- ExampleWEB.ino
		|	|	|- ExampleWEB.h
		|	|	|- ExampleSTA.cpp
		|	|	|- ExampleAP.cpp
		|	|
		|	|-\OtherApp				another app using common files in \src
		|	|	|- OtherApp.ino
		|	|	|- ...
``` 
