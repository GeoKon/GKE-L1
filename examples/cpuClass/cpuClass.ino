#include "cpuClass.h" 	// this includes Arduino.h
#include "macros.h"

#define EXAMPLE1

// ----------- class instantiation ------------------------
    CPU cpu;
// ---------------- main code -----------------------------
#ifdef EXAMPLE1
void setup()
{
    cpu.init();
    PF( "[%d] Heap now %ld (max=%ld)\r\n", __LINE__, cpu.heapUsedNow(), cpu.heapUsedMax() );
    
    for(;;)
    {
        PR("--- BEGINNING OF THE SETUP LOOP ---");
        cpu.prompt("1. Enter something (no response): ");
        CRLF();

        PF( "[%d] Heap now %ld (max=%ld)\r\n", __LINE__, cpu.heapUsedNow(), cpu.heapUsedMax() );

        BUF c('A');
        PF("[%d] Allocated c[%d]. Heap=%ld\r\n", __LINE__, c.maxsiz,cpu.heapUsedNow()  );
        BUF x("test");
        PF("[%d] Allocated test[%d]. Heap=%ld\r\n", __LINE__, x.maxsiz,cpu.heapUsedNow() );
        BUF y("test", 20 );
        PF("[%d] Allocated y[%d]. Heap=%ld\r\n", __LINE__, y.maxsiz, cpu.heapUsedNow() );
        y.free();
        PF("[%d] Dealloc y. Heap=%ld\r\n", __LINE__, cpu.heapUsedNow() );
        x.free();
        PF("[%d] Dealloc x. Heap=%ld\r\n", __LINE__, cpu.heapUsedNow() );
        c.free();
        PF("[%d] Dealloc c. Heap=%ld\r\n", __LINE__, cpu.heapUsedNow() );
        BUF b; 
        PF("[%d] Allocated b[%d]\r\n", __LINE__, b.maxsiz );
        b.set( cpu.prompt( "2a. Enter something again (<64): " ) );
        b.print("You entered: ");
        PF( "[%d] Heap now %ld (max=%ld)\r\n", __LINE__, cpu.heapUsedNow(), cpu.heapUsedMax() );
        CRLF();
        BUF s(100); 
        s.set( cpu.prompt( "2b. Enter something again (<100): " ) );
        s.print("You entered: ");
        PF( "[%d] Heap now %ld (max=%ld)\r\n", __LINE__, cpu.heapUsedNow(), cpu.heapUsedMax() );
        CRLF();
        
        s = cpu.prompt( "3. Enter something: " );
        PF("a. You entered %s\r\n", s.pntr );
        PF("b. You entered %s\r\n", !s );
        PF("c. You entered %s\r\n", s.c_str() );
        CRLF();
        
        PF( "See what you entered: %s\r\n", cpu.prompt( "4. Enter something: " ) );
        CRLF();

        s.set("Using string as sprintf %d and %.3f", 10, 30.45 );
        s.print("5. See s-set");
        s.add(". Adding 'more' numbers %d and %s", 56, "EOL");
        s.add(". Included 'quoted string' up to the end'");
        s.print("6a. See s-add");
        s.quotes();
        s.print("6b. See s-quotes");
        
        PR("7. Now testing the COUT pipe");
        COUT p;
        p << "This is an example" << "of using the pipe" << "";
        p << "The number is" << 10 << "and this is a float" << 1.245 << "";
        p << "Can also print hex values" << 'A' << "and" << '1' << "";
        CRLF();
            
        PF( "[%d] Heap now %d (max=%d)\r\n", __LINE__, cpu.heapUsedNow(), cpu.heapUsedMax() );
        if( *cpu.prompt("Press X<CR> to fall into loop. Else repeats" ) == 'X' ) 
            break;
    }
    PF( "[%d] Heap now %ld (max=%ld)\r\n", __LINE__, cpu.heapUsedNow(), cpu.heapUsedMax() );
    PR("Press button to see incrementing counter");
}

static int count=0;
void loop()
{
    if( cpu.buttonPressed() )
        PF( "Count = %d\r\n", count++ );
    if( count > 10 )
        cpu.die("happy!");
}
#endif

#ifdef EXAMPLE2
void sub( char *p )
{
    PR("Inside sub");
    PF("Received %s\r\n", p );
}
void setup()
{
    cpu.init();

    char *p;
    for(;;)
    {
        char *p = cpu.prompt("TEST: ");
        sub( p );
    }
}
void loop()
{
    ;    
}
#endif
