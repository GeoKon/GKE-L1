#include "cpuClass.h" 	        // this includes Arduino.h

// ----------- class instantiation ------------------------
    CPU cpu;
// ---------------- main code -----------------------------

void setup()
{
    cpu.init();
    PF("Press button\r\n");
}
void setup1()
{
    cpu.init();
    PF( "[%d] Heap now %ld (max=%ld)\r\n", __LINE__, cpu.heapUsedNow(), cpu.heapUsedMax() );
    
    for(;;)
    {
        CRLF();
        PRN("--- BEGINNING OF THE SETUP LOOP ---");
        cpu.prompt("** Enter something (no response): ");

        PF( "[%d] Heap now %ld (max=%ld)\r\n", __LINE__, cpu.heapUsedNow(), cpu.heapUsedMax() );

        PRN("Blinking LED 5-times");
        cpu.blink( 5 );

        PRN("** Testing the COUT pipe");
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
    PRN("Press button to see incrementing counter");
}

static int count=0;
void loop()
{
    if( cpu.buttonPressed() )
        PF( "Count = %d\r\n", count++ );
    if( count > 10 )
        cpu.die("happy!", 3 );
}
void loop1()
{
    int k;
    if( k = cpu.buttonReady() )
        PF( "KEY = %d\r\n", k );
}
