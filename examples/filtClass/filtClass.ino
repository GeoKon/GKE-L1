#include "cpuClass.h"
#include "filtClass.h"

CPU cpu;
FILT filter;

void setup()
{
    //cpu.init();
    cpu.init( 115200, 13+NEGATIVE_LOGIC /* LED */, 0+NEGATIVE_LOGIC /* push button */ );

    for(;;)
    {
        int x = atoi( cpu.prompt( "Enter prop-delay: ") );
    
        filter.setPropDelay( x );
        float v;
        int j = 0;
        
        for(int i=0; i<20; i++ )
        {
            if( i==0 )
                filter.smooth( v=0.0 );
            else if( i==1 )
                filter.smooth( v=100.0 );
            else
                filter.smooth( v=0.0 );
            float sm = filter.getSmoothed();
            PF("In=%4.1f Out=%4.1f", v, sm );
            if( sm >= 1.0 )
                j++;
            else
                j = 0;
            if( j>0)
                PF(" Tail=%d\r\n", j );
            else
                PF("\r\n");
        }
    }
}

void loop()
{
    ;
}
