#include "bufClass.h"     // this includes Arduino.h
#include "cpuClass.h" 	// this includes Arduino.h

//#define MYBUF BUFT <256>
#define MYBUF BUF

// ----------- class instantiation ------------------------

    CPU cpu;
    
// ---------------- diagnostic code -----------------------
#define SHOW showHeap(__LINE__)
void showHeap( int line )
{
    PF( "[Line %d] Heap now %ld\r\n", line, cpu.heapUsedNow());    
}

// ---------------- main code -----------------------------
void setup()
{
    cpu.init();  
    int loops = 0;  
    char t[40];
    
    for( int i=0;;i++)
    {
        if( i==0 )
            cpu.heapUpdate();
        SHOW;
        
        CRLF();
        PR( SF(t, "------------- Loop count %d --------------\r\n", loops++ ) );
        CRLF();
        SHOW;
        
        // --------------------- Section 1 -------------------------
        
        char *xp = cpu.prompt("1. Enter something (no response): ");
        PF("You entered: %s\r\n", xp );
    
        SHOW;
        MYBUF c( "@C", 100 );
        SHOW;
        MYBUF x( "@X", 200 );
        SHOW;
        MYBUF y( "@Y", 300 );
        SHOW;
        y.~MYBUF();
        SHOW;
        x.~MYBUF();
        SHOW;
        c.~MYBUF();
        SHOW;

        // --------------------- Section 2 -------------------------

        xp = cpu.prompt( "\r\n2. Enter something again (<64): " );

        MYBUF b( "@B", 100 ); 
        SHOW;
        b.set( xp );
        b.print("You entered: ");
        b.~MYBUF();
        SHOW;
        
        // --------------------- Section 3 -------------------------

        xp = cpu.prompt( "\r\n3. Enter something again (<100): " );
        MYBUF s( "@S", 1000 );
        SHOW; 
        s.set( xp );
        s.print("You entered: ");
        
        // --------------------- Section 4 -------------------------
        xp = cpu.prompt( "4. Enter something: " );
        
        SHOW;
        s.copy( xp );
        SHOW;
        PF("b. You entered %s\r\n", !s );
        SHOW;
        PF("c. You entered %s\r\n", s.c_str() );
        SHOW;
        
        // for some reason heap increases here!      
        
        s.set("Using string as sprintf %d and %.3f", 10, 30.45 );
        CRLF();
        
        // --------------------- Section 5 -------------------------
        SHOW;
        s.print("5. See s-set ");
        s.add(". Adding 'more' numbers %d and %s", 56, "EOL");
        s.add(". Included 'quoted string' up to the end'");
        s.print("5a. See s-add    ");
        s.quotes();
        s.print("5b. See s-quotes ");
        s.~MYBUF();
        SHOW;
    }
}

void loop()
{
    ;
}
