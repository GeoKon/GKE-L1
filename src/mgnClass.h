/*

## 					MegunoLink Interface

- use `mgn.init()` with NULL BUF to enable direct console PF()
- use non NULL BUF to accumulate commands; then use `mgn.print()` to print them

##### Source code is shown below

```
*/
#pragma once
#include "bufClass.h"

class MGN
{
    int idx;
    BUF *bpnt;
    char channel[32];		// used to store optional Meguno channel
	
public:    
	MGN(); 

	// provide a BUF sufficient to accumulate all responses back to caller
	void init( const char *chan = "" );
    void init( BUF *bp, const char *chan = "" );

    // ------------------- TABLES -------------------------------------
    void tableSet( const char *cname, const char *value, const char *descr="" );
    void tableSet( const char *cname, float value, const char *descr="" );
    void tableSet( const char *cname, int value, const  char *descr="" );
    void tableSet( const char *cname, bool value, const char *descr="" );
	void tableGet( const char *cname );
    void tableClear( const char *cname="" );
    
    // ------------------- TIME PLOT -----------------------------------
    void tplotData( const char *series, float value );
    void tplotYRange( float minv, float maxv );
    void tplotY2Range( float minv, float maxv );
    void tplotSet( const char *prop );
    void tplotClear();
    
    // ------------------- CONTROLS -------------------------------------
    void controlSetText( const char *cname, const char *value ); // text box
    void controlSetValue( const char *cname, float value );
    void controlSetValue( const char *cname, int value ); // progress bar
    void controlSetCheck( const char *cname, bool onoff );
    void controlSetProperty( const char *cnameprop ); // text box

    // -------------------- Buffer Management -----------------------------
    void clear();
    char *getBuf();
    void print();
};
/*
```
*/