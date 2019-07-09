#pragma once

class TICBASE
{
protected:
    uint32_t    prevtic;            // millis()/ms after ready() is true
    uint32_t    readytic;           // millis()/ms set every time ready() is called
    uint32_t    readycount;
    int ms;
   
public: 
    TICBASE()
    {
        prevtic     = 0;
        readycount  = 0;
    }
    bool ready()
    {
        readytic = millis()/ms;
        if( prevtic != readytic )
        {
            prevtic = readytic;
            readycount++;
            return true;
        }
        return false;
    }
    bool countMod( int d, int r=0 )
    {
        return (readycount % d)==r;
    }
    bool overrun()
    {
        return (millis()/ms) != prevtic;    
    }
};

class TICsec : public TICBASE
{
public: 
    TICsec( int sec )
    {
        ms = 1000*sec;
		TICBASE();
    }
};

class TICms : public TICBASE
{
public: 
    TICms( int msec )
    {
        ms = msec;
		TICBASE();
    }
};

// class TICms
// {
    // uint32_t    prevtic;            // millis()/ms after ready() is true
    // uint32_t    readytic;           // millis()/ms set every time ready() is called
    // uint32_t    readycount;
    // int ms;
    
// public: 
    // TICms( int msec )
    // {
        // ms = msec;
        // prevtic     = 0;
        // readycount  = 0;
    // }
    // bool ready()
    // {
        // readytic = millis()/ms;
        // if( prevtic != readytic )
        // {
            // prevtic = readytic;
            // readycount++;
            // return true;
        // }
        // return false;
    // }
    // bool countMod( int d, int r=0 )
    // {
        // return (readycount % d)==r;
    // }
    // bool overrun()
    // {
        // return (millis()/ms) != prevtic;    
    // }
// };

class PROF
{
#define DELAY_COUNTERS 5   
	int delays[ DELAY_COUNTERS ];
    int counts[ DELAY_COUNTERS+1 ];
    uint32_t T0, T1;    
public:
    PROF( int i0=5, int i1=25, int i2=50, int i3=75, int i4=100 )
    {
        delays[0]=i0;
        delays[1]=i1;
        delays[2]=i2;
        delays[3]=i3;
		delays[4]=i4;
        reset();
    }
    void start()
    {
        T0 = millis();    
    }
    void reset()
    {
        for(int i=0; i<DELAY_COUNTERS+1; i++ ) 
            counts[i] = 0;  
    }
    void profile()
    {
        uint32_t D = millis()-T0;
        
        int i;
		for( i=0; i<DELAY_COUNTERS; i++ )	// 0...4
		{
			if( D<delays[i] )
			{
				counts[i]++;
				break;
			}
		}
		if( i==DELAY_COUNTERS )				// 5
			counts[ DELAY_COUNTERS ]++;
    }
    void report( bool prf = false )
    {
        uint32_t D = millis()-T0;
        //PF("Delay %d\r\n", D );
        if( prf )
		{
			for(int i=0; i<DELAY_COUNTERS; i++)
				PF( "[<%dms]:%d\t", delays[i], counts[i] );
			
			PF( "[>=%dms]:%d\r\n", delays[DELAY_COUNTERS-1], counts[DELAY_COUNTERS] );
		}
    }
};

#ifdef TEST // TEST PROGRAM FOR ABOVE
#include <cpuClass.h>

typedef TICms <250> TIC250ms;
typedef TICms <1000> TIC1000ms;

CPU cpu;

uint32_t T0;

void setup() 
{
	cpu.init();
	T0 = millis();
}
// All tasks need to be executed in time LESS than the smallest TIC250ms
TIC250ms t( 4 );
void loop() 
{
	if( t.ready() )
	{
		uint32_t T1 = millis();
		
		PF("every 1sec %ld\r\n", millis()-T0 );     // every loop
		if( t.count(2)) 					        // every 2nd loop
	    {
	        PF("every 2sec %ld. ", millis()-T0 );
            delay( 240 );
            PF("Elapsed = %d\r\n", millis()-T1 );
	    }
		if( t.count( 10 ) )				            // every 10th loop
		{
		    PF("60th %ld\r\n", millis()-T0 );
            delay( 1010 );
		}

        if( t.overrun() )
            PF("OVERUN\r\n");
	}
}
#endif
