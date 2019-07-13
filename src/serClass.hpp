#pragma once

// See eepClass.md for use

#include <EEPROM.h>
#include <bufClass.h>
#include <cpuClass.h>

// -------------------------- SERIALIZER ------------------------------
#define LSHIFT(A) A=(A<<2)&(~3)

#define MAX_USERPARMS 20			// max number of user parms
#define USER_STR_SIZE 20			// max size of strings stored in eeprom

static char *BAD_ENCODING = "Bad Encoding";
static char *PARM_OVERRUN = "Parm Overrun";
static char *INSUF_BUFFER = "Insufficient Buffer";
static char *BAD_PARM_TYPE = "Bad Parm Type";
static char *INSUF_NPARMS = "Insufficient nParms";

#define MAX_BUFSIZE 256				// max encoding buffer

typedef struct						// buffer header
{
	uint16_t ibytes;
	uint16_t iparms;
	uint32_t itypes;
} HEADER;

#define HEADER_SIZE sizeof( HEADER )

class SER
{
private:
	struct
	{
		const char *name;		// userParm name
		const char *help;		// print format
		void *pntr;				// pointer to user value of this parm
		byte type;				// userParm 'f', 'd' or 's'
	
	} prm[MAX_USERPARMS];

public:	

	uint16_t nbytes;			// bytes needed to encode
	uint16_t nparms;			// number of saved parms
	uint32_t ntypes;			// bit mask
	
public:	
	SER();
	void 	resetRegistry();
	bool	registerParm( const char *pname, char ptype, void *ppntr, const char *format="" );
    void 	registryStatus( char *prompt="" );
	
	bool 	setParmValue( const char *pname, void *ppntr );
    bool	setParmByStr( const char *uname, char *s );
	
	int 	getParmType ( const char *pname );
	int 	getParmIndex( const char *pname );
	B80 	getParmString( int ind );
	
 	bool encodeParms( byte *p, int maxsiz, int np = MAX_USERPARMS );
	bool decodeParms( byte *p );
	
	void printParms( char *prompt="" );
	void printBuf( char *prompt, byte *base );	// diagnostics
	bool notify( char *s );
};

