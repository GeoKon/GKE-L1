#pragma once
#include <macros.h>

#include <cpuClass.h>   // needed for snf() and BUF definitions

// Convenient macro to save into the BUF buf if initialized
#define BSAVE( format, ...) if(bpnt) bpnt->add( format, ##__VA_ARGS__ )

class MGN
{
    int idx;
    BUF *bpnt;
    char channel[20];		// used to store optional Meguno channel
	
public:    
	MGN() 
    { 
        idx = 0;
        bpnt = NULL;
		channel[0]=0;
    }
	// provide a BUF sufficient to accumulate all responses back to caller
    void init( BUF *bp, const char *chan = "" )
    {
		bpnt = bp;			// set the pointer for all functions
        bpnt->init();		// initialize buffer
		idx = 0;			// number of bytes written
		
		if( *chan )
			SF( channel, ":%s", chan );
		else
			channel[0] = 0;
    }
    // ------------------- TABLES -------------------------------------
    void tableSet( const char *cname, const char *value, const char *descr="" )
    {
        BSAVE("{TABLE%s|SET|%s", channel, cname);
        if( *value ) BSAVE("|%s", value );
        if( *descr ) BSAVE("|%s", descr );
        BSAVE("}\r\n");
    }
    void tableSet( const char *cname, float value, const char *descr="" )
    {
        char s[32];
        sprintf(s,"%.3f", value);
        tableSet( cname, s, descr );
    }
    void tableSet( const char *cname, int value, const  char *descr="" )
    {
        char s[32];
        sprintf(s,"%d", value);
        tableSet( cname, s, descr );
    }
    void tableSet( const char *cname, bool value, const char *descr="" )
    {
        tableSet( cname, value?"True":"False", descr );
    }
	void tableGet( const char *cname )
    {
        if( *cname )
			BSAVE("{TABLE%s|GET|%s}\r\n", channel, cname);
		else
			BSAVE("{TABLE%s|GET}\r\n", channel);
    }
    void tableClear( const char *cname="" )
    {
        BSAVE("{TABLE%s|CLEAR", channel );
        if( *cname ) BSAVE("|%s", cname );
        BSAVE("}\r\n");
    }
    
    // ------------------- TIME PLOT -----------------------------------
    //
    void tplotData( const char *series, float value )
    {
        BSAVE("{TIMEPLOT%s|D|%s|T|%.3f}\r\n", channel,series, value );
    }
    void tplotYRange( float minv, float maxv )
    {
        BSAVE("{TIMEPLOT%s|yrange|%.3f|%.3f}\r\n", channel,minv, maxv );
    }
    void tplotY2Range( float minv, float maxv )
    {
        BSAVE("{TIMEPLOT%s|y2range|%.3f|%.3f}\r\n", channel,minv, maxv );
    }
    void tplotSet( const char *prop )
    {
        BSAVE("{TIMEPLOT%s|SET|%s}\r\n", channel,prop); 
    }
    void tplotClear()
    {
        BSAVE("{TIMEPLOT%s|CLEAR}\r\n", channel); 
    }
    
    // ------------------- CONTROLS -------------------------------------
    //
    void controlSetText( const char *cname, const char *value ) // text box
    {
        BSAVE("{UI%s|SET|%s.Text=%s}\r\n", channel,cname, value); 
    }
    void controlSetValue( const char *cname, float value )
    {
        BSAVE("{UI%s|SET|%s.Value=%.3f}\r\n", channel,cname, value); // numeric up/down
    }
    void controlSetValue( const char *cname, int value ) // progress bar
    {
        BSAVE("{UI%s|SET|%s.Value=%d}\r\n", channel,cname, value);
    }
    void controlSetCheck( const char *cname, bool onoff )
    {
        BSAVE("{UI%s|SET|%s.Checked=%s}\r\n", channel,cname, onoff?"True":"False");
    }
    void controlSetProperty( const char *cnameprop ) // text box
    {
        BSAVE("{UI%s|SET|%s}\r\n", channel,cnameprop); 
    }
    // -------------------- Buffer Management -----------------------------
     
    void clear()
    {
		bpnt->init();
    }
    char *getBuf()
    {
    	return bpnt->c_str();
    }
    void print()
    {
    	bpnt->print();
    }
};

#ifdef TEST_FOR_MGNCLASS
#define V mg.sv

int main()
{
    pf("Hello World");
    BUF t(32);
    pf( "%s", t.add("George %d", 29) );

    pf( "%s %s %s", V( 10 ), V( 10.23f ), V( true ));
    pf( "%s %s", V( 30.4578, 0), V( 30.4578, 3 ));    

    mg.start();
    mg.tableSet("Time", "[Now()]");

    mg.tableSet("Voltage", 20.3f, "mV");
    mg.tableSet("Current", 2, "A");
    mg.tableClear( "Voltage");
    
    mg.tplotData( "Voltage", 29);
    mg.tplotData( "Current", 30.34);
    mg.tplotYRange( -10, 23.45);
    mg.tplotSet( "Title=Garbage");

        
    mg.controlSetText( "button", "ACTIVE");
    mg.controlSetValue( "bar", 20);
    mg.controlSetValue( "bar", 20.345f);
    mg.controlSetCheck( "trace", true );
    mg.end();
    
    return 0;
}
#endif
