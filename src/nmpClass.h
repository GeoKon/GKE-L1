#pragma once

// See eepClass.md for use

//#include <EEPROM.h>
#include <bufClass.h>

// -------------------------- SERIALIZER ------------------------------

   #define MAX_USERPARMS 20			// max number of user parms
   #define USER_STR_SIZE 20			// max size of strings stored in eeprom

#define MAX_BUFSIZE 256				// max encoding buffer

typedef struct			// Header of serialized buffer, as written to EEPROM
{
	uint16_t ibytes;
	uint16_t iparms;
	uint32_t itypes;
	
} HEADER;
#define HEADER_SIZE sizeof( HEADER )

class NMP
{
private:
	struct
	{
		const char *name;		// userParm name
		const char *format;
		const char *info;		// 
		void *pntr;				// pointer to user value of this parm
		byte type;				// userParm 'f', 'd' or 's'
	
	} prm[MAX_USERPARMS];

	bool notify( char *s );
	char tempbuf[32];			// temp buffer to return parm value string

	void _printhandler ( BUF *bp, int i );
	void _megunohandler( int i, char *channel );

	uint16_t nbytes;			// bytes needed to encode
	uint16_t nparms;			// number of saved parms
	
public:	
	void 	resetRegistry();
	bool	registerParm( const char *pname, char ptype, void *ppntr, const char *info="", const char *format="" );
    
	size_t 	getSize(); 			// size of the data structure to be saved
	int  	getParmCount();	 	// returns nparms. Useful to iterate

	bool 	setParmValue( const char *pname, void *ppntr );	// assumes previously defined type
    bool	setParmByStr( const char *uname, char *s );		// assumes previously defined type
	
	int  	getParmIndex( const char *pname );	 	// returns -1 if not found	

const char* getParmName    ( int idx );				// returns 'type' 'd' 'f' 's' or 0
	int 	getParmType    ( int idx );				// returns 'type' 'd' 'f' 's' or 0
const char*	getParmInfo    ( int idx );				// returns 'info' or "unknown parm"	
	char*   getParmValueStr( int idx );				// returns index or -1

	int 	getParmType    ( const char *pname );	// returns 'type' 'd' 'f' 's' or 0
const char*	getParmInfo    ( const char *pname );	// returns 'info' or "unknown parm"
	char* 	getParmValueStr( const char *pname );	// returns value string or "unknown parm"
	
	void printParm  	( char *name, BUF *bp=NULL );
	void printAllParms 	( char *prompt="", BUF *bp=NULL );

	void printMgnParm  	 ( char *channel, char *name );
	void printMgnAllParms( char *channel=""  );
	void printMgnInfo	 ( char *channel, char *value, char *info );
};

