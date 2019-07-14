#pragma once

// Decimal IIR Filter
// k  = smooth factor
// k1 = 1-k
// C  = smooth count (1/k)
// y[n+1] = k1*y[n] + k*u[n]
// percent of target = the percent of target value after nsteps
// percent of settling = the percent error of target after nsteps (1 - percent of target)

class IIR
{
private:
    float out;      // y(n) filter output
    bool first;     // true if filter needs to be initialized
    bool disabled;  // true if filter is disabled
    float k, k1;    // smooth factor k, and k1=(1-k)
public:
    IIR();
    void reset();	// resets filter to y[n] = u[n]

    // Primary initialization 
	void setSmoothFactor( float sf );

    // Alternative#1 initialization 
	void setSmoothCount( int smoothcount );

    // Alternative#2 initialization
    int setStepsToTarget( int nsteps, int pctoftarget );

	// Reports steps reuired to settle
	int getStepsToTarget( int pctoftarget );
    int filter( int input );			// sets inputs; returns output
	float filter( float input );			// sets inputs; returns output
    float getOutput();				// past return
};

