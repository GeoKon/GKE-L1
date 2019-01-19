##                 CPU Class

Includes core MCU functionality like control of the LED, push-buttons and 
statistics of the heap. It also includes a convenient set of I/O routines
to print formatted variables and get console strings.
  
- **CPU cpu** class allocation. Typically allocated globally.
- **cpu.init( baud, led, button )** initialzies serial port
- **cpu.led( BLINK|ON|OFF, [times])** controls the LED
- **cpu.button()** true or false depending if button is pressed
- **cpu.buttonPressed()** true if pressed; waits until released
- **cpu.prompt()** prints a prompt and waits for string response. Returns 'char *'
to a string locally allocated.

**Heap**

- **cpu.heapUpdate()** updates heap max
- **cpu.heapUsedNow()** indicates amount of heap used
- **cpu.heapUsedMax()** indicates max amount of heap used

**Macros**

- **ASSERT(condition)** aborts if not true
- **PR( text )** prints text with CRLF at the end
- **PN( text )** prints text as is
- **PF( format, ...)** same as printf()
 
##                 Buffer Class

**Buf** class is a highly optimized class; it complements the **String** class. 
Key features include:

- formatted, *printf()* style of new buffers -- a safe version of *sprintf()*
- appending of new variables in a formatted fashion
- the usual **'='** and **'=+'** operators
- direct access of the 'char *' pointer of the string.

**Buf** uses either the local stack for allocations of less than 16-bytes or
the heap for larger sizes. In fact, **Buf s('c')** uses only two bytes from the
stack eliminating the extensive use of the heap. Heap is only used for large 
buffers greater than 16 characters. 

It is primarily used for simple string manipulations of equations and concatenations.
In contrast, the **String** class includes very rich functionality, but uses the 
heap extensively which can cause fragmentation. 
 
- **Buf s([size])** declares a new buffer area. **size** defaults to 64 characters
including the \0 at the end.
- **Buf s(0..2)** allocate just two bytes of \0. **Buf s('c')** allocates 'c\0'
- **Buf s("string", [size])** allocate a buffer containing a string; allocation is 
to the size of the string, unless followed by a specific size.
- **s.free()** to free previously allocated heap space

- **s.pntr** to access the allocated buffer. Same is **!s**. Same as **s.c_str()**
- **s.init()** to initialize the buffer for subsequent **s.add()** operations
- **s.length()** same as strlen( !s )
- **s.maxsiz** max size of allocated buffer
 
- **s=string** or **s = q** sets s equal to **string**
- **s+=string** appends **string**
- **s.set( format, ...)** formats and sets contents
- **s.add( format, ... )** formats and adds contents
- **s.print()** prints current buffer. Analogous to PN()

##                 Console Pipe

- **COUT P** to allocate a new pipe stream
- **P << ""** streams CR/LF at the end of a stream
- **P << "literal" << ""** streams literal with space before, unless 1st
- **P << 10 << ""** streams integer with space before
- **P << 1.23 << ""** streams float with space before
- **P << c << ""** streams character in HEX with space before

