#ifndef DS18CLASS_H
#define DS18CLASS_H

#include <OneWire.h>

typedef byte DevAddr[8];        // structure to hold DS18 address

#define MAX_DS18B20         4   // max number of sensors supported
#define DS18_NOT_PRESENT   -1
#define DS18_CRC_ERROR     -2
#define DS18_INVALID_STATE -3
#define INVALID_TEMP      -127

class DS18
{
public:
    DS18( OneWire *myds ) { ds = myds; } 			// associate this class with the OnwWire class

	
	int init( int ncount, bool debug = false );    // searches for sensors, fills private table. Returns devices found
    
	void start( int devid );                        // use thisID() to start with zero and increment
	void start( DevAddr *ap );						// start measurements of this given MAC
	
	DevAddr *getAddr( int idx ) 	{ return &list[ idx ]; }
	
    bool ready();    								// has conversion been completed?
    int read( bool debug = false );					// read temperature of thisID()
	
	float getDegreesC() 			{	return tempC[ idx ]; }
	float getDegreesC( int id ) 	{ 	return tempC[ id  ];  }

	float getDegreesF() 			{	return (tempC[idx] * 1.8) + 32;}
	float getDegreesF( int id ) 	{	return (tempC[id ] * 1.8) + 32;}

	bool error( int id ) 			{ return err[idx]; }
	bool error() 					{ return err[idx]; }

	
	
	int thisID()					{ return idx; }
    int nextID()					{ idx++; if( idx >= found ) idx = 0; return idx;}
	
    void simulate( int idx, float temp=0.0 );
	void simulOff( int idx );
	

private:
    OneWire *ds;
    DevAddr list[MAX_DS18B20];  // MAX sets of 8-byte addresses
	float tempC [MAX_DS18B20];	// temp holding of temperatures
	bool  err   [MAX_DS18B20];	// error while reading the n-th temperature
	
    int  found;                 // number of devices found by init()
    
    uint32_t Tend;
    int  idx;                   // current device addressed 0...found-1
    byte state;                 // current state 0, 1 or 2

    bool  simulON[MAX_DS18B20]; // simulation on or off for 0th temp
    float simTemp[MAX_DS18B20]; // simulated temperature
};

#define MAX_FILTER_IDs 4
class FLT
{
private:
    float smoothed  [ MAX_FILTER_IDs ];
    bool started    [ MAX_FILTER_IDs ];
    float coef;
    int maxids;
    
public:
    FLT( int mxids );
    void setCoef( float coef );
    float smooth( int id, float x );    
    float getValue( int id );
};


#endif
