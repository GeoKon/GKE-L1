#ifndef FILTCLASS_H
#define FILTCLASS_H

#define MAX_FILTER_IDs 4	// max vector size to smooth

class FILT
{
public:
    FILT( int mxids=1 );					// user specified vector size
    void setIIRCoef( float coef );			// sets filter coef x[n]=a*x[n-1]+(1-a)u[n]
    void setPropDelay( int samples );		// number of samples to 1% of magnitude
	float smooth( float x, int id=0 );    	// smooth the i-th element
	float *Vsmooth( float v[] );			// smooth the vector;
    float getSmoothed( int id=0 );
	float *getVSmoothed();
	
private:
    float smoothed  [ MAX_FILTER_IDs ];
    bool  started   [ MAX_FILTER_IDs ];
    float coef;
	bool  zerocoef;							// true if coef = 0.0
    int maxids;
};


#endif
