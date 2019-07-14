#include <cpuClass.h>
#include <ticClass.h>
#include "iirClass.h"

CPU cpu;
IIR iir;

void setup()
{
	cpu.init();
	PF("Starting\r\n");
	iir.setSmoothCount( 10 );
	PF("Will settle to 90%% after %d steps\r\n", iir.getStepsToTarget( 90 ));
	
	iir.setStepsToTarget( 1, 90 );
	iir.setStepsToTarget( 9, 90 );
	iir.getStepsToTarget( 90 );
	
	iir.setStepsToTarget( 6, 99 );
	iir.getStepsToTarget( 99 );
	
	iir.setStepsToTarget( 13, 99 );
	iir.getStepsToTarget( 99 );
	
	iir.setStepsToTarget( 1, 75 );
	iir.setStepsToTarget( 3, 75 );
	iir.setStepsToTarget( 6, 99 );
	iir.setStepsToTarget( 13, 99 );
}

static int count = 0;
TICsec tic(1);

void loop1()
{
	int inp;
	
    if( tic.ready() )
    {
        if( count < 5 )
    		inp = 0;
	    else
		    inp = 100;
	
    	iir.filter( inp );
    	PF("inp:%5d out:%5d\r\n", inp, (int) iir.getOutput() );
    }
	count++;
}
void loop()
{
    float inp;
    
    if( tic.ready() )
    {
        if( count < 5 )
            inp = 0.0;
        else
            inp = 100.0;
    
        iir.filter( inp );
        PF("inp:%5.0f out:%5.2f\r\n", inp, iir.getOutput() );
    }
    count++;
}
