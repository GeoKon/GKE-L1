#include <Arduino.h>
#include <math.h>

#include "macros.h"
#include "iirClass.h"

// Decimal IIR Filter
// k  = smooth factor
// k1 = 1-k
// C  = smooth count (1/k)
// y[n+1] = k1*y[n] + k*u[n]
// percent of target = the percent of target value after nsteps
// percent of settling = the percent error of target after nsteps (1 - percent of target)

/*
private:
    float out;      // y(n) filter output
    bool first;     // true if filter needs to be initialized
    bool disabled;  // true if filter is disabled
    float k, k1;    // smooth factor k, and k1=(1-k)
public:
*/
    IIR::IIR()
    { 
		setSmoothFactor( 1.0 );
    }
    void IIR::reset()	// resets filter to y[n] = u[n]
    {
		setSmoothFactor( 1.0 );
    }
    // Primary initialization 
	void IIR::setSmoothFactor( float sf )
    {    
		first = true;
        if( sf == 1.0 )
        {
            disabled = true;
            k = 1.0;
            k1 = 0.0;
			out = 0.0;
        }
        else
        {
            disabled = false;
            k = sf;
            k1 = 1.0-k;
            out = 0.0;
        }
    }
    // Alternative#1 initialization 
	void IIR::setSmoothCount( int smoothcount )
    {    
		setSmoothFactor( smoothcount<=1 ? 1.0 : 1.0 / (float)smoothcount );
		return;
	}	
    // Alternative#2 initialization
    int IIR::setStepsToTarget( int nsteps, int pctoftarget )
    {
    	float logf = log2( 1.0 - ((float) pctoftarget)/100.0 );
    	float alpha = pow( 2, logf/(nsteps+1) );
    	setSmoothFactor( 1.0-alpha );
       	PF("N=%d to settle %d%%, a=%f, k=%f\r\n", nsteps, pctoftarget, alpha, k );
	}
	// Reports steps required to settle
	int IIR::getStepsToTarget( int pctoftarget )
	{
		float logf = log2( 1.0 - ((float) pctoftarget)/100.0 );
    	float alpha = 1.0 - k;
    	float nsteps = logf/log2(alpha) - 1.0;
		PF("Reaches %d%% of target in %.1f steps\r\n", pctoftarget, nsteps);
		return (int) nsteps;
	}
    int IIR::filter( int v )			// sets inputs; returns output
    {
		if( disabled || first )
            out = (float) v;
        else
            out = k1*out + k*(float)v;
        first = false;
        return (int) out;
    }
	float IIR::filter( float v )			// sets inputs; returns output
    {
		if( disabled || first )
            out = v;
        else
            out = k1*out + k*v;
        first = false;
        return out;
    }
    float IIR::getOutput()				// past return
    {
        return out;
    }

