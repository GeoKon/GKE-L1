#include "cpuClass.h" 	// this includes Arduino.h

CPU cpu;

void setup()
{
    cpu.init();
    PF( "[%d] Heap now %ld (max=%ld)\r\n", __LINE__, cpu.heapUsedNow(), cpu.heapUsedMax() );
    
    for(;;)
    {
        PR("--- BEGINNING OF THE SETUP LOOP ---");
        cpu.prompt("1. Enter something (no response): ");
        CRLF();
        
        Buf s(100); 
        s.set( cpu.prompt( "2. Enter something again: " ) );
        s.print("You entered: ");
        CRLF();
        s = cpu.prompt( "3. Enter something: " );
        PF("a. You entered %s\r\n", s.pntr );
        PF("b. You entered %s\r\n", !s );
        PF("c. You entered %s\r\n", s.c_str() );
        CRLF();
        
        cpu.prompt( "4. Enter something: " ).print("See what you entered: ");
        CRLF();

        s.set("Using string as sprintf %d and %.3f", 10, 30.45 );
        s.print("5. See s-set");
        s.add(". Adding more numbers %d and %s", 56, "EOL");
        s.print("6. See s-add");
        
        PR("7. Now testing the COUT pipe");
        COUT p;
        p | "This is an example" | "of using the pipe" | "";
        p | "The number is" | 10 | "and this is a float" | 1.245 | "";
        p | "Can also print hex values" | 'A' | "and" | '1' | "";
        CRLF();
            
        PF( "[%d] Heap now %d (max=%d)\r\n", __LINE__, cpu.heapUsedNow(), cpu.heapUsedMax() );
        if( *cpu.prompt("Press X<CR> to fall into loop. Else repeats" ).pntr == 'X' ) 
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
