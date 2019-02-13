#include <cpuClass.h>
#include "ds18Class.h"
#include <OneWire.h>

//float DS18::toDegreesF( int C ) { return (C * 1.8) + 32; }
//float DS18::toDegreesC( int F ) { return (F - 32.0) * 0.555555556; }

void DS18::simulate( int id, float t )
{
    simulON[ id ] = true;
    simTemp[ id ] = t;
}
void DS18::simulOff( int id )
{
    simulON[ id ] = false;
    simTemp[ id ] = INVALID_TEMP;
}
int DS18::init( int ncount, bool debug )
{
    ds->reset_search();
    found = 0;
    for( int id=0; (id<MAX_DS18B20) && (id<ncount); id++ )
    {
        byte *addr = (byte*) &list[id];
        
        // Check for device present
        if ( !ds->search(addr) )
            break;
                              
        // Check is this device has the right CRC
        if( OneWire::crc8(addr, 7) != addr[7] ) 
        {
            if( debug )
                PF( "Dev%d bad CRC\r\n", id);
            break;
        }
        
        // check if DS18B20
        int type = *addr;
        if(  !(type == 0x10 // DS18S20
            || type == 0x28 // DS18B20
            || type == 0x22 // DS1822
            || type == 0x3B // DS1825 
            ) )
        {
            if( debug )
                PF("Dev:%d unsupported type:%02x\r\n", id, type );
            break;
        }
        if( debug )
        {
            PF("SENSOR:%d TYPE:%02X ID:", id, type );
            for( int i = 0; i < 8; i++) 
                PF("%02X", addr[i] );
            PF("\r\n");
        }
		simulON[ id ] = false;
        found++;
        delay(750 );
    }
    state   = 0;
    idx     = 0;
    if( debug )
		PF("%d devices found\r\n", found );
	return found;
}

void DS18::start( int devid )  
{
    if( state !=0 )             // do nothing if state is not zero
        return;
            
    idx = devid;
    
    if( idx >= found )          // just in case there is an error somewhere
        idx = 0;
    byte *addr = (byte *) &list[ idx ];

    ds->reset();
    ds->select( addr );
    ds->write(0x44, 1);          // 0x44 is STARTCONVO
    uint32_t T0 = millis();      // get timestamp
    Tend = T0+750;               // 12 bits delay worst case
    
    state = 1;
	err[idx] = 0;
}
bool DS18::ready()
{
    if( (state==1) && (millis()>Tend) )
    {
        state = 2;
        return true;
    }
    else
        return false;
}
int DS18::read( bool debug )
{
    if( state != 2 )
    {
        state = 0;
        return err[idx] = DS18_INVALID_STATE;
    }
    if( simulON[ idx ] )
    {
        tempC[idx] = simTemp[ idx ];
        state = 0;
        return err[idx] = 0;
    }

    byte *addr = (byte *) &list[ idx ];
    byte data[12];
    tempC[ idx ] = INVALID_TEMP;   // invalid reading in case of an error
        
    if( !ds->reset() )
    {
        if( debug ) 
            PF("Dev %d not present\r\n", idx);
        state = 0;
        return err[idx] = DS18_NOT_PRESENT;
    }
    // Scratchpad locations
    //    #define TEMP_LSB        0
    //    #define TEMP_MSB        1
    //    #define HIGH_ALARM_TEMP 2
    //    #define LOW_ALARM_TEMP  3
    //    #define CONFIGURATION   4
    //    #define INTERNAL_BYTE   5
    //    #define COUNT_REMAIN    6
    //    #define COUNT_PER_C     7
    //    #define SCRATCHPAD_CRC  8

    ds->select(addr); 
    ds->write (0xBE);                // 0xBE is READSCRATCH
     
    for ( int i = 0; i < 9; i++) 
        data[i] = ds->read();
 
    if( OneWire::crc8(data, 8) != data[8] )
    {
        if( debug )
            PF("Dev %d CRC error", idx );
        state = 0;
        return err[idx] = DS18_CRC_ERROR;
    }
        
    int16_t raw = (data[1] << 8) | data[0];     // TEMP LSB and MSB

    byte type_s = *addr;
    // PF("TYPE=%02X CFG=%02X\r\n", type_s, data[4] & 0x60);
    
    if (type_s==0x10)                           // only applicable to DS18S20
    {
        raw = raw << 3; 
        if (data[7] == 0x10) 
            raw = (raw & 0xFFF0) + 12 - data[6];
    } 
    else 
    {
        byte cfg = (data[4] & 0x60);            // check CONFIGURATION [4]
        
        if (cfg == 0x00) raw = raw & ~7;        // 9 bit resolution, 93.75 ms
        else if (cfg == 0x20) raw = raw & ~3;   // 10 bit res, 187.5 ms
        else if (cfg == 0x40) raw = raw & ~1;   // 11 bit res, 375 ms
    }
    tempC[ idx ] = (float)raw / 16.0;
    state = 0;
    return err[idx] =0;
}

//-------------------------------------------------------------------

FLT::FLT( int mxids )
{
    if( mxids > MAX_FILTER_IDs )
        mxids = MAX_FILTER_IDs;
    maxids = mxids;
    setCoef( 0.0 );
}
void FLT::setCoef( float co )
{
    coef = co;
    for( int i=0; i<maxids; i++ )
        started[i]=false;
}
float FLT::smooth( int id, float value )
{
    if( started[id] )
    {
        return smoothed[ id ] = coef * smoothed[ id ] + (1.0-coef) * value;
    }
    else
    {
        started[id] = true;
        return smoothed[ id ] = value;
    }
}
float FLT::getValue( int id )
{
    return smoothed [ id ];
}
