#include "ads15Class.hpp"
#include "cpuClass.hpp"

CPU cpu;	
ADS15 ads( 0x48 );            // ADDR pin to GND

#ifdef EXAMPLE1    
void setup(void) 
{
    cpu.init();
    pinMode( 14, OUTPUT );

    ads.begin();
    ads.initChannel( 0, FS_4096mV );
    ads.initChannel( 1, FS_4096mV );
    ads.initChannel( 2, FS_4096mV );
    ads.initChannel( 3, FS_4096mV );
    ads.initChannel( 4, FS_4096mV );
     
    for(;; )
    {
        int v[5];
        for( int i=0; i<5; i++ )
        {
            ads.startConversion( i );
            v[i] = ads.readConversion();
        }
        for( int i=0; i<5; i++ )
            PF("%6d\t", v[i] );  
        for( int i=0; i<5; i++ )
            PF("%.3fV\t", ads.toVolts( i, v[i] ));   
        PF("\r\n");
        delay(1000);
    }
}
void loop()
{
		yield();
}
#endif

template <int MAX_DEPTH>
class CBUF
{
private:
    int nextv;                  // raotating index 0...maxsz-1
    int partial;                // number of values in circular queue 0...maxsz
    int maxsz;                  // max size of circular buffer
    int16_t values[MAX_DEPTH];
public:
    CBUF()
    { 
        reset();
        maxsz = 1;
    }
    void reset()
    {
        nextv=0; 
        partial=0;         
    }
    void init( int size )
    {
        if( size > MAX_DEPTH )
            size = MAX_DEPTH; 
        if( size <= 0 )
            size = 1;
        maxsz = size;
        nextv=0; 
        partial=0;   
    }
    void add( int16_t v )
    {
        values[ nextv++ ] = v;
        if( partial < maxsz )
            partial++;
        if( nextv >= maxsz )
            nextv = 0;
//        for( int i=0; i<maxsz; i++ )
//            PF( "%2d ", values[i] );
//        PF( "\r\n" );
    }
    int16_t average()
    {
        if( partial == 0 )
            return 0;
        if( partial == 1 )
            return values[0];
            
        int32_t sum = 0;
        for( int i=0; i<partial; i++ )
            sum += values[i];
        //PF( "Ave = %d\r\n",sum/partial); 
        return sum/partial;
    }
};
 
CBUF <100> q[3];  // 3-circular buffers, 0=A, 1=V1, 2=V2
int reading[5];

#include "Ticker.h"

#define A_CURRENT_ACS   0
#define A_CURRENT_REF   1
#define A_VOLTS1        2
#define A_VOLTS2        3
#define A_CURRENT       4

#define Q_CURRENT       0
#define Q_VOLTS1        1
#define Q_VOLTS2        2

class SCAN
{
    uint counter;               // increments every call
    uint mreading;              // how often to update readings
    uint mreadcount;            // counts 0...mreading-1
    bool iamready;              // true if mreading has been reached.
    bool scancomplete;          // true if all channels have started conversion
    
public:
    
    void init( int vdepth, int idepth, int mcount )
    {
        q[ Q_CURRENT ].init( idepth );
        q[ Q_VOLTS1  ].init( vdepth );
        q[ Q_VOLTS2  ].init( vdepth );
            
        counter  = 0;
        mreadcount = 0;
        mreading = mcount;
        iamready = false;
        scancomplete = false;
    }
    void reset()
    {
        for( int i=0; i<5; i++ )
            q[i].reset();
        mreadcount = 0;
        iamready = false;
        scancomplete = false;
    }
    bool ready()
    {
        bool retcode = iamready;
        iamready = false;
        return retcode;
    }
    void update()   // worst case computation delay is 12ms
    {        
         uint32_t T0 = millis();
         iamready = false;
        switch( counter % 5 )
        {
            case 0:
                    if( scancomplete )
                    {
                        q[ Q_CURRENT ].add( ads.readConversion() );   // get last channel
                        
                        mreadcount++;
                        if( mreadcount >= mreading )
                        {
                            mreadcount = 0;
                            reading[ A_VOLTS1 ] = q[ Q_VOLTS1  ].average();
                            reading[ A_VOLTS2 ] = q[ Q_VOLTS2  ].average();
                            reading[ A_CURRENT] = q[ Q_CURRENT ].average();
                            iamready = true;
                        }
                    }
                    ads.startConversion( A_CURRENT_ACS );
                    scancomplete = false;
                    break;
            case 1:
                    reading[ A_CURRENT_ACS ] = ads.readConversion();  
                    ads.startConversion( A_CURRENT_REF );
                    scancomplete = false;
                    break;
            case 2:
                    reading[ A_CURRENT_REF ] = ads.readConversion(); 
                    ads.startConversion( A_VOLTS1 );
                    scancomplete = false;
                    break;
            case 3:
                    q[ Q_VOLTS1 ].add( ads.readConversion() );   
                    ads.startConversion( A_VOLTS2 );
                    scancomplete = false;
                    break;
            case 4:
                    q[ Q_VOLTS2 ].add( ads.readConversion() );   
                    ads.startConversion( A_CURRENT );
                    scancomplete = true;
                    break;
        }
        counter++;
        if( millis() > (T0+13) )
            PF("***OVERFLOW***\r\n");
        yield();
    }
};
SCAN scan;
Ticker tic;

uint32_t T0;
bool tic_ready=false;

void setup(void) 
{
    cpu.init();

    ads.begin();
    ads.initChannel( A_CURRENT_ACS,     FS_4096mV );
    ads.initChannel( A_CURRENT_REF,     FS_4096mV );
    
    ads.initChannel( A_VOLTS1,     FS_4096mV );
    ads.initChannel( A_VOLTS2,     FS_4096mV );
    ads.initChannel( A_CURRENT,    FS_4096mV );
     
    scan.init( 100, 100, 2 );
    tic.attach_ms( 20, [](){ tic_ready=true; } );
    T0 = millis();
}
void loop()
{
    if( tic_ready ) // true every 20ms
    {
        scan.update();      // takes about 12ms
        tic_ready = false;
    }
        
    if( scan.ready() )      // true every 2x5x20 ms = 200ms
    {
        PF("%6dms: ", (millis()-T0) );
        for( int i=0; i<5; i++ )
            PF("%6d\t", reading[i] );  
        for( int i=0; i<5; i++ )
            PF("%.3fV\t", ads.toVolts( i, reading[i] ));   
        PF("\r\n");
        T0 = millis();     
    }
}
