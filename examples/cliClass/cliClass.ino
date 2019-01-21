//  #define EXAMPLE0          // Tests simple gets() and prompt
//  #define EXAMPLE1          // Minimal setup() interaction example
    #define EXAMPLE2          // non-blocking interaction example
//  #define EXAMPLE3          // heap test

#include "cliClass.h"       // includes cpuClass.h and ESP8266WiFi.h

// --------------------- allocation of classes ------------------------------------
    CPU cpu;
    CLI cli;
// ---------------------------------- EXAMPLE0 ------------------------------------

#ifdef EXAMPLE0         // Tests simple gets() and prompt

void setup(void) 
{
  cpu.init();
  cli.init( ECHO_ON, "cmd: " );    // by default, ECO is OFF
  cli.prompt();
}
void loop()
{
  if( cli.ready() )
  {
    PF("Entered 1: [%s]\r\n", cli.gets() );
    
    Buf bf;
    bf.set( cli.gets() );    // copy to buffer
    PF("Entered 2: [%s]\r\n", bf.pntr );

    Buf bx( cli.gets() );    // copy to buffer
    PF("Entered 3: [%s]\r\n", bx.pntr );
    
    cli.prompt();
  }
}
#endif

// ---------------------------------- EXAMPLE1 ------------------------------------

#ifdef EXAMPLE1         // Tests simple gets() and prompt

EXE exe;        
#define DCL_PF Buf &bf          // not really used
#define RESPONSE Serial.printf

void fnc1( int n, char **arg, DCL_PF )
{
  RESPONSE("In function FUNC1\r\n" );
}
void fnc2( int n, char **arg, DCL_PF  )
{
  RESPONSE("In function FUNC2\r\n" );
  for( int i=0; i<n; i++ )
    RESPONSE("arg[%d] = %s\r\n", i, arg[i] );
}
void help( int n, char **arg, DCL_PF )
{
    exe.printHelp( (n<=1) ? (char *)"" : arg[1] );
}
CMDTABLE t1[]
{
  {"h",    "[select] help",                         help },
  {"func1", "Calls function-1",                      fnc1 },
  {"func2", "arg1 arg2 ... argN. Call function-2",   fnc2 },
  {NULL, NULL, NULL}
};

void setup(void) 
{
    cpu.init();
   
    exe.registerTable( t1 );
    exe.printTables( "See Table" );  
    
    for(;;)
    {
        char *p = cpu.prompt("Enter command: " );   // p point to local buffer in cpu class
        exe.dispatch( p );
    }
}
void loop()
{
    ;
}
#endif

// ---------------------------------- EXAMPLE2 ------------------------------------

#ifdef EXAMPLE2         // Tests simple gets() and prompt

EXE exe;
        
#define RESPONSE(...) exe.respond(__VA_ARGS__)

void fnc1( int n, char **arg )
{
  RESPONSE("In function FUNC1\r\n" );
}
void fnc2( int n, char **arg  )
{
  RESPONSE("In function FUNC2\r\n" );
  for( int i=0; i<n; i++ )
    RESPONSE("arg[%d] = %s\r\n", i, arg[i] );
}
void fnc3( int n, char **arg )
{
  RESPONSE("In function FUNC3\r\n");
}
void fnc4( int n, char **arg  )
{
  RESPONSE("In function FUNC4\r\n" );
  for( int i=0; i<n; i++ )
    RESPONSE("arg[%d] = %s\r\n", i, arg[i] );
}
void help( int n, char **arg )
{
    exe.help( n, arg );
}

CMDTABLE t1[]
{
  {"h",    "[select] help",                         help },
  {"func1", "Calls function-1",                      fnc1 },
  {"func2", "arg1 arg2 ... argN. Call function-2",   fnc2 },
  {NULL, NULL, NULL}
};
CMDTABLE t2[]
{
  {"fn3", "void function3()", fnc3 },
  {"fn4", "void function4()", fnc4 },
  {NULL, NULL, NULL}
};

void setup(void) 
{
    cpu.init();
    
    exe.registerTable( t1 );
    exe.registerTable( t2 );
    exe.printTables( "See all tables" );  // instead of PF, you can optionally specify a Buf

    cli.init( ECHO_ON, "cmd:" );
    PR("Main Loop");
    cli.prompt();
}
void loop()
{
    if( cli.ready() )
    {
        char *cmd = cli.gets();             // cmd points to the heap allocated by CLI
        PF("Command entered: %s\r\n", cmd );

        exe.dispatch( cmd );                // using Serial.printf(); unbuffered version

        PF("--- Buffered version ---\r\n");
        
        Buf response(200);                  // allocate a response buffer
        exe.dispatch( cmd, &response );     // use the buffered version
        response.print();                   // display the buffer

        PF("Maxsiz=%d, used:%d\r\n", response.maxsiz, response.length() );
        cli.prompt();
    }
}
#endif

// ---------------------------------- EXAMPLE1 ------------------------------------

#ifdef EXAMPLE11         // Tests simple gets() and tokenizer
#define RESPONSE PF     // not used
void setup(void) 
{
  cpu.init();
  PF("Tests tokenize()\r\n");
  
  cli.init( ECHO_ON, "cmd: " );    // by default, ECO is OFF
  cli.prompt();
}
void loop()
{
  if( cli.ready() )
  {
    PF("Received [%s]\r\n", cli.gets() );

    cli.tokenize();
    cli.printArgs( "Tokens", cli.nargs(), cli.args() );

//  Alternative way
//    char *tokx[10];
//    int n = cli.tokenize( cli.gets(), &tokx[0], 10 );            
//    cli.printArgs( "Tokens", n, tokx );
    
    cli.prompt();
  }
}
#endif
// ---------------------------------- EXAMPLE3 ------------------------------------

#ifdef EXAMPLE3                 // Tests heap()
#define RESPONSE PF             // not used

String sub1()
{
    cpu.heapUpdate();
    PF( "3. Entering sub1 %d\r\n", cpu.heapUsedNow() );
    
    String s("", 2000); // this allocates 2000 bytes
    
    cpu.heapUpdate();
    PF( "4. Just allocated 2k %d\r\n", cpu.heapUsedNow() );
    s.set("content of s");
    
    PF( "5. After allocating 20000 inside sub1 %d\r\n", cpu.heapUsedNow() );
    s="test";
    PF( "6. reusing s with 5 bytes %d\r\n", cpu.heapUsedNow() );

    return s;
}
void test()
{
    PF( "2. Entering test() %d\r\n", cpu.heapUsedNow() );
    PF( "7. Calling sub1() and printing result[%s]\r\n", sub1().c_str() );
    PF( "8. Still inside test() after printing %d\r\n", cpu.heapUsedNow() );
}
void setup(void) 
{
    cpu.init();
    PF( "1. Starting setup() %d\r\n", cpu.heapUsedNow() );
    test();
    PF( "9. back to main() %d\r\n", cpu.heapUsedNow() );
}
void loop()
{
    ;
}
#endif

// ------------- attempt to compute stack size -----------------------------

//static int i=0;         // counter or recursive iterations
//static int imax = 100;  // max loop counter
//static uint32_t smax;   // base of the stack
//static uint32_t smin;   // depth of the stack
//
//void fillStack()
//{
//    uint32_t code = 0x12345678;    // pattern
//    
//    if( i==0 )
//        smax = (uint32_t) &code;
//    if( i>=imax )
//    {
//        smin = (uint32_t) &code;
//        return;
//    }
//    i++;
//    fillStack();
//} 
//void showStack()
//{
//    PF("Min=%d, Max=%d\r\n", smin, smax );
//    byte *p;
//    uint32_t *q;
//    p = (byte *)smin;
//    q = (uint32_t *)smin;
//    PR("Per Byte");
//    for( int i=0; i<100; i++ )
//    {
//        PF("%d Mem[%d] = %02X\r\n", i, p, *p );
//        p++;
//    }
//    PR("Per U32");
//    for( int i=0; i<100; i++ )
//    {
//        PF("%d Mem[%d] = %08X\r\n", i, q, *q );
//        q += 8;
//    }    
//}
//#define STACK() printStack( __FILE__, __LINE__ )
//extern void printStack( char *f, int d );
//void printStack( char *f, int d )
//{
//    uint32_t *q;
//    int i;
//    q = (uint32_t *)smin;
//    for( i=0; i<100; i++ )
//    {
//        if( *q != 0x12345678 )
//            break;
//        q += 8;
//    }    
//    char *fn = rindex( f, '\\' );   // find the last \
//    if( fn== NULL )
//        fn = f;
//    PF("%s[%d]: Stack depth is %dx32 bytes\r\n", fn, d, i );
//}
