#include "filtClass.hpp"
#include "cpuClass.h"
#include <math.h>
// ------------------------------ IIR Class ---------------------------

IIR::IIR()		
{
    setIIRCoef( 0.00 );
}
void IIR::setIIRCoef( float co )
{
    coef = co;
	zerocoef = (co == 0.00);
    started=false;
	smoothed = 0.0;
	PF("Coef %4.3f\r\n", coef);
}
void IIR::setPropDelay( int samples )
{
	
	setIIRCoef( computeIIRCoef( samples ) );
}
void IIR::smooth( float *pvalue )
{
    float value = *pvalue;
	smoothed = coef * smoothed + (1.0-coef) * value;
	*pvalue = smoothed;
	
	// if( started )
    // {
        // if( zerocoef )
			// *pvalue = value;
		// else
			// *pvalue = coef * smoothed + (1.0-coef) * value;
    // }
    // else
    // {
        // started = true;
		// *pvalue = (smoothed = value);
    // }
}

// ------------------------------ FILT Class ---------------------------
FILT::FILT( int mxids )		
{
    if( mxids > MAX_FILTER_IDs )
        mxids = MAX_FILTER_IDs;
    maxids = mxids;
    setIIRCoef( 0.00 );
}
void FILT::setIIRCoef( float co )
{
    coef = co;
	zerocoef = (co == 0.00);
    for( int i=0; i<maxids; i++ )
        started[i]=false;
}
void FILT::setPropDelay( int samples )
{
	setIIRCoef( computeIIRCoef( samples ) );
}
float FILT::smooth( float value, int id )
{
    if( started[id] )
    {
        if( zerocoef )
			return smoothed[ id ] = value;
		else
			return smoothed[ id ] = coef * smoothed[ id ] + (1.0-coef) * value;
    }
    else
    {
        started[id] = true;
        return smoothed[ id ] = value;
    }
}
float *FILT::Vsmooth( float value[] )
{
    for( int i=0; i<maxids; i++ )
        smooth( i, value[i] );
	return &smoothed[0];
}
float FILT::getSmoothed( int id )
{
    return smoothed [ id ];
}
float *FILT::getVSmoothed()
{
    return &smoothed [0];
}
// ------------------------------ Compute Coeff ---------------------------
/*
	filter x[n]=a*x[n-1] + (1-a)u[n]
	Impulse response is y[n] = (1-a) (a^n) *u[n]
	To reach 1% of 100 magnitude: 0.01 = (1-a) * a^n
	or -2 = log(1-a) + n*log(a)
	or -2-log(1-a) = n*log(a)
	This is solved iteratively using:
		a0 = 0
		a1 = 10^( (-2-log(1-a)) / n )
*/
float computeIIRCoef( int samples )
{
	if( samples <=0 )
		return 0.0;

	float a0, a1, Sa, ex;
	a0 = 0.0;
	Sa = samples;

	for( int i=0; i<10; i++ )	// up to 10 interations
	{
		ex = (-2.0 - log10( 1.0 - a0 ))/Sa;
		a1 = pow( 10.0, ex );
		if( (fabs( a0-a1 )<= 0.005) )
			break;
		a0 = a1;		
	}
	return (a1>=1.00)? 0.999 : a1;	
}