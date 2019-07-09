#include "cpuClass.h" 	// this includes Arduino.h
/*
        STACK BUFFER
 
     B64 s  OR  B64 s("Test") OR BUFT <512> s;
     s.set( "numb=%f, x=%d\r\n", 10.3, 40 );
     s.add( ... )
     s.copy( "This is a test" );     
     s.print( "prompt" );
     char *q = !s;
     size_t N = s.size();
     
        HEAP BUFFER

     MYBUF s(64)  OR  MYBUF s(64, "test")
 
 */

// ----------- class instantiation ------------------------
    CPU cpu;
// ---------------- main code -----------------------------
#define SHOW showHeap(__LINE__)
void showHeap( int line )
{
    PF( "[Line %d] Heap now %ld\r\n", line, cpu.heapUsedNow());    
}
// ---------------- main code -----------------------------

#define MYBUF BUFT <256>
void setup()
{
    cpu.init();  
    int loops = 0;  
    char t[40];
    
    for(;;)
    {
        CRLF();
        PR( SF(t, "------------- Loop count %d --------------\r\n", loops++ ) );
        SHOW;
        CRLF();
        
        char *xp = cpu.prompt("1. Enter something (no response): ");
        PF("You entered: %s\r\n", xp );
    
        // --------------------- c, x, y -------------------------
        SHOW;
        MYBUF c( 100, "A");
        SHOW;
        MYBUF x( 200, "X");
        SHOW;
        MYBUF y( 300, "Y" );
        SHOW;
        //y.~MYBUF();
        SHOW;
        //x.~MYBUF();
        SHOW;
        //c.~MYBUF();
        SHOW;

        // ------------------- b ----------------------------
        MYBUF b(100, "b"); 
        SHOW;
        xp = cpu.prompt( "\r\n2a. Enter something again (<64): " );
        SHOW;
        b.set( xp );
        b.print("You entered: ");
        CRLF();
        //b.~MYBUF();
        SHOW;
        
        // -------------------- s --------------------------
        MYBUF s(1000, "s");
        SHOW; 
        xp = cpu.prompt( "\r\n2b. Enter something again (<100): " );
        s.set( xp );
        s.print("You entered: ");
        CRLF();
        
        xp = cpu.prompt( "3. Enter something: " );
        s.copy( xp );
        PF("b. You entered %s\r\n", !s );
        PF("c. You entered %s\r\n", s.c_str() );
      
        s.set("Using string as sprintf %d and %.3f", 10, 30.45 );
        CRLF();
        s.print("5. See s-set ");
        s.add(". Adding 'more' numbers %d and %s", 56, "EOL");
        s.add(". Included 'quoted string' up to the end'");
        s.print("6a. See s-add    ");
        s.quotes();
        s.print("6b. See s-quotes ");
        //s.~MYBUF();
        SHOW;
    }
}

void loop()
{
    ;
}
