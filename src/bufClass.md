##                 Buffer Class

**BUF** class is a highly optimized class; it complements but does not replace the standard Arduino **String** class. 
Key features include:

- choice of allocation in the STACK or the HEAP
- formatted, *printf()* style of new buffers -- a safe version of *sprintf()*
- appending of new variables in a formatted fashion
- the usual **'='** and **'=+'** operators
- direct access of the 'char *' pointer of the string.

It is primarily used for simple string manipulations of equations and concatenations. In contrast, the **String** class includes very rich functionality, uses the heap extensively which can cause fragmentation. 

There are two different **BUF** definitions:

    - BUF( size, ["string"] )    
    - BUFT << size >> (["string"]), and the equivalent convenient defines of 
    	- B32
    	- B64
    	- B80

The first allocates in the HEAP the specified size and if the second argument `string` is given, it initializes the buffer with it.  
The second is a TEMPLATE of specified size allocating memory in the stack. If the argument `string` is given, the buffer is initialized with it; otherwise the buffer is initialized to 'empty'. The convenient defines `B32, B64` and `B80` are predefined sizes of this template.

All class methods ensure that the specified size is not exceeded.

### Summary of functions

For the following summary, assume that `s` is allocated as `BUF s(80)`, or `B80 s`

**s.set( format, ...)**
> This is identical to `sprintf()` but the content of `s` cannot exceed the maximum buffer size.

**s.add( format, ...)**
> Formats the specified argument exactly as `sprintf()` and appends the formatted string to the buffer without exceeding the maximum buffer size.

**s.copy( source )**
> Identical to `strcpy()` but ensures not exceeding the maximum buffer size.

**s.quotes()**
> Replaces single quotes with double quotes. Very convenient to initialize JSON strings. Example:

    B80 js;
    char *fn = "John";
    char *ln = "Smith";
    js.add( "x = {'fname':'%s', ", fn );
    js.add( "'lname':'%s'};", ln );
    js.quotes();
    
    results to 
    x = { "fname": "John", "lname": "Smith" };

**s.print( prompt )**
> Prints `prompt` followed by the content of the buffer. If the buffer does not includes a \n at the end, a CRLF is inserted.

**char *p = s.c_pntr();**
**char *p = !s;**
> The `c_str()` method, or the `!` operator return a pointer to the content of the buffer. 

**s.length()** 
> Returns the `strlen()` of the buffer

**s.size()** 
> Returns the max size of the buffer

