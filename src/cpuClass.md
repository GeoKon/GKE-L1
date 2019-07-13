##                 CPU Class

**CPU** encapsulates core MCU and I/O functionality, including efficient console I/O, control of the LED, sensing push-buttons, statistics of the heap. 
 
### Summary of functions

For the following summary, assume that `cpu` is allocated as `CPU cpu`.

**cpu.init** ( baud=115200, ledp=LED+NEGATIVE_LOGIC, button=BUTTON+NEGATIVE_LOGIC )
> Initializes the primary serial port, defines the logic of the LED and button.

**cpu.blink** ( times )
> Blinks the LED for specified number of times. ON/OFF intervals are 100ms and 200ms respectively.

**cpu.led** ( enum onoff_t onoff, times=1 )
> Turns LED on or off. The constants `ON` and `OFF` or `BLINK` can be used. In case of `BLINK` the ON/OFF intervals are the same as `blink()`

**cpu.button** ()
> Returns true or false depending if the button is pressed or not.

**cpu.buttonPressed** ()
> Returns true or false depending if the button is pressed and released. While the button is pressed, the LED is turned ON and when released, the LED is turned OFF. The following example show how this function should be used

    
    if( cpu.buttonPressed() ) 		// Example 1: wait until button is pressed and released
    {
    	do_something();
    	while( cpu.buttonPressed() )
    		;
    }   
    
    void loop()						// Example 2: inside the loop()
    {
    	if( cpu.buttonPressed() )
    		do_something();			// this is called only once on button pressed
    } 

**char *cpu.prompt** ( prompt )
> Prints the specified prompt and waits for a string terminated by RETURN. Returns a pointer to the string without the terminating RETURN. The string entered cannot exceed 79 characters and it is protected to this maximum size.

**cpu.heapUpdate()** 
**cpu.heapUsedNow()**
**cpu.heapUsedMax()**
> These routine is to be used to detect memory leaks. The heap size used is set when the class `CPU` is allocated. Thereafter, you may insert `cpu.heapUpdate()` in various part of the code to track the size of the heap. At any point you may print the current size of the heap using `cpu.heapUsedNow()` of the maximum heap size using `cpu.heapUsedMax()`.

**cpu.die** ( char *prompt, int pattern )
> Prints the prompt and the current heap size. Then, enters in an infinite blinking the LED with a pattern 100ms/200ms for `pattern` times, followed by 500ms of delay.

**sf** ( char *sp, size_t sL, const char *format, ... )
**SF** ( char *sp, char *format, ... )
> Safe versions of the `sprintf()` function. The second is a MACRO assumes the the string `sp` is previously declared as `char sp[size]`


##                 Macros

- **ASSERT** (condition) aborts if not true
- **PR **( text ) prints text (no CRLF)
- **PRN**( text ) prints text followed by CRLF
- **PF **( format, ...) same as printf()
- **PFN**( format, ...) same as printf() followed by CRLF
- **CRLF**()
 
##                 Console Pipe

- **COUT P** to allocate a new pipe stream
- **P << ""** streams CR/LF at the end of a stream
- **P << "literal" << ""** streams literal with space before, unless 1st
- **P << 10 << ""** streams integer with space before
- **P << 1.23 << ""** streams float with space before
- **P << c << ""** streams character in HEX with space before

