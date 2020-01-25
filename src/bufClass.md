---
typora-copy-images-to: doc
---

##                 Buffer Class

**BUF** class is a highly optimized string management class; it complements but does not replace the standard Arduino **String** class. 
Key features include:

- choice of allocation in the STACK or the HEAP
- formatted, *printf()* style of new buffers -- a safe version of *sprintf()*
- appending of new variables in a formatted fashion
- the usual **'='** and **'=+'** operators
- direct access of the 'char *' pointer of the string.

It is designed to manage RAM very efficiently with direct access to string pointers. In contrast, the **String** class includes very rich functionality, uses the heap extensively which can cause fragmentation. 

There are two **BUF** definitions:

- **BUF** string( size, [label] );
- **BUF**( string, size ) and the equivalent convenient macros of **B32**( string ), **B64**( string ), **B80**( string ), **B128**( string ) , **B256**( string ).

The first allocates in the HEAP the specified size. For example, `BUF s(1024)`, allocates 1024 bytes in the heap named `s`.  The second argument is optional, used for debugging: every time a BUF is allocated or de-allocated a reference to this label is printed.

The second uses an existing string pointer to use the BUF structure. For example, 

```c++
char cstr[1024]; 	// allocate a string of 1024 bytes
BUF s(cstr, 1024); 	// defines 's' using the previous allocation of 'cstr'
```

The convenient macros `B32,..., B256` use predefined sizes of the 'string' argument. More specifically, 

```C++
B80( s ); 			// expands to the following
char s_str[32]; 
BUF s( s_str, 32 );
```

All class methods ensure that the specified size is not exceeded.

### Typical Use of Methods

For the following summary, assume that `s` is allocated as `BUF s(80)`, or `B80(s)`

```
BUF s(80);
...
s.set( format, ... );	// fills buffer starting from the beginning
s.add( format, ... );	// one more additions to above
s.add( format, ... );
char *p = s.c_str();	// get the beginning of this buffer. Also returned by s.set()

s.rebase();				// start a new string base
s.set( format, ... );
....
char *q = s.c_str();	// rebased pointer to this string. Also reurned by new s.set()
```

In the above example, `s` is allocated of size of 80 bytes and then filled with the `set` and `add()` commands with formatted content. *Under no circumstances the maximum size of 80 is exceeded.*

A more advance use of this class allows to append multiple strings with own pointers. The example below shows how to create three formatted strings in the same BUF.

```c++
BUF s(256);
char *p1 = s.set( format, ... );	// pointer p1 can be used subsequently
s.append();							// re-base BUF after the 1st string
char *p2 = s.set( format, ... );	// pointer p2 can be used subsequently
s.append();
char *p3 = s.set( format, ... );
```

When `append()` is used, if the subsequent string does not fit in the allocated size, 256 for this example, the next string is 'wrapped around' to the origin; the first string `s1` and respective pointer `p1` are destroyed. The picture below is indicative of this wrapped around allocation:

![bufClassPic1](doc/bufClassPic1.jpg)

This behavior is very important for three reasons:

- conservation of RAM by 'packing' strings
- avoid fragmentation of multiple mallocs
- maintaining valid strings/pointers until more storage is needed.

**Note**: The `add()` method appends contents but does not wrap-around; it truncates the added string instead.  

### Class Reference

**s.set( format, ...)**

> This is identical to `sprintf()` but the content of `s` cannot exceed the maximum buffer size. 
> Returns a pointer to the formatted string.

 **s.append()**

> Rebases the next `set()` string to be after the current content. If the next string does not fit to the available size, the string is moved to the beginning of the BUF. See previous explanations.

 **s.init()**

> Rebases the next `set()` string to the beginning of the BUF. 

**s.add( format, ...)**

> Formats the specified argument exactly as `sprintf()` and appends the formatted string to the buffer without exceeding the maximum buffer size. Regardless of previous use of `append()`, `add()` never wraps-around and always respects the maximum BUF size.
> Returns a pointer to the formatted string as previously returned by `set()`.

**s.copy( source )**

> Identical to `strcpy()` but ensures not exceeding the maximum buffer size.
> Returns a pointer to the formatted string.

**s.quotes()**

> Replaces single quotes with double quotes. Very convenient to initialize JSON strings. Example:

    B80( js );
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

> The `c_str()` method, or the `!` operator return a pointer to the content of the buffer. 

> 

**s.length()** 

> Returns the `strlen()` of the current string

**s.size()** 

> Returns the max size of the buffer

