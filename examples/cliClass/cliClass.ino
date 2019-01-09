//#define EXAMPLE0          // Tests simple gets() and prompt
//#define EXAMPLE1          // Tests simple gets() and tokenizer
//#define EXAMPLE2          // Tests heap()
//#define EXAMPLE3          // Tests simple cli.exec() using printf()
  #define EXAMPLE4          // Tests simple cli.exec() using RESPONSE buffer
//#define EXAMPLE5          // tests interact() with RESPONSE with local table

#include "cliClass.h"       // includes cpuClass.h and ESP8266WiFi.h

extern COMMAND table[];     // forward reference

// --------------------- allocation of classes ------------------------------------

CPU cpu;
CLI cli;
Buf rbuf( 400 );            // CLI Response buffer. Used by interact() and execTables()
                            // Use RESPONSE rbuf.add

// ---------------------------------- EXAMPLE0 ------------------------------------

#ifdef EXAMPLE0         // Tests simple gets() and prompt
#define RESPONSE PF     // not used

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
    PF("[%s]\r\n", cli.gets() );
    cli.prompt();
  }
}
#endif
// ---------------------------------- EXAMPLE1 ------------------------------------

#ifdef EXAMPLE1         // Tests simple gets() and tokenizer
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
// ---------------------------------- EXAMPLE2 ------------------------------------

#ifdef EXAMPLE2                 // Tests heap()
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
// ---------------------------------- EXAMPLE3 ------------------------------------

#ifdef EXAMPLE3         // Tests simple CONSOLE CLI using printf()
#define RESPONSE PF     // response by all CLI handlers

void setup(void) 
{
    cpu.init();
    cli.init( ECHO_ON, "cmd: " );    // by default, ECO is OFF

    cli.prompt();
    for(;;)
    {
        if( cli.ready() )
        {
            int ok = cli.exec( table );           // executed OK?
            if( !ok )
                PF("Cmnd \"%s\" not found\r\n", *cli.args() );
            cli.prompt();
         }
    }
}
void loop()
{
    yield();
}
#endif
// ---------------------------------- EXAMPLE4 ------------------------------------

#ifdef EXAMPLE4                 // Tests simple cli.exec() using RESPONSE buffer
#define RESPONSE rbuf.add       // not used

void setup(void) 
{
  cpu.init();
  PF("Tests exec()\r\n");
  cli.init( ECHO_ON, "cmd: " );    // by default, ECO is OFF
  cli.prompt();
}
void loop()
{
  if( cli.ready() )
  {
    PF("Received [%s]", cli.gets() );
    rbuf.init();
    bool e = cli.exec( &table[0] );
    if( !e )
        RESPONSE("Cmnd %s not found\r\n", *cli.args() );
    rbuf.print();
    cli.prompt();
  }
}
#endif

// ---------------------------------- EXAMPLE5 ------------------------------------

#ifdef EXAMPLE5             // tests interact() with RESPONSE
#define RESPONSE rbuf.add   // as allocated in cliClass

void setup(void) 
{
  cpu.init();
  cli.init( ECHO_ON, "cmd: " );    // by default, ECO is OFF
  interact( &table[0] );
}
void loop()
{
    yield();
}
#endif

// -------------- dispatch table and functions -----------------

static void help( int n, char **arg)
{
    if( n>1 )
        cli.printTable( &table[0], arg[1] );
    else
        cli.printTable( &table[0], "" );
}
static void func1( int n, char **arg)
{
  RESPONSE("In function FUNC1\r\n" );
}
static void func2( int n, char **arg )
{
  RESPONSE("In function FUNC2\r\n" );
  for( int i=0; i<n; i++ )
    RESPONSE("arg[%d] = %s\r\n", i, arg[i] );
}
class Func
{
  public:
  static void fun3( int n, char **x) {RESPONSE("Func3\r\n");}
  void fun4() {RESPONSE("Func4\r\n");}
};
Func ft;

COMMAND table[]
{
  {"h",     "[sel] help", help  },
  {"func1", "void function1()", func1 },
  {"func2", "void function2()", func2 },
  {"func3", "static class function", ft.fun3 },
  {"fnc4", "class wrapped in lambda", [](int n, char **x){ft.fun4();}  },
  {NULL, NULL, NULL}
};
