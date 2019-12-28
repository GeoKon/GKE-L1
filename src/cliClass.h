#pragma once
#include "bufClass.h"

/*
 * Example 1:
 *  in setup:       cli.init()
 *                  cli.prompt()
 *  in loop:        if( cli.ready() )
 *                      cli.gets();
 *                      cli.prompt();
 */

typedef struct
{
  char *cmd;
  char *help;
  void (*func)(int, char ** );

} CMDTABLE;

enum echo_t
{
  ECHO_ON=true,
  ECHO_OFF=false
};

class CLI
{
public:
    CLI( int nchar=80 );
    ~CLI();
    void init( echo_t echo1, const char *prompt=NULL, bool flush1=false );
    void prompt();
    
    bool ready();		// true is one or more characters are waiting in the serial queue
    char *gets();		// returns a pointer to a string received after \r detected
	
private:
    char *buf;      // pointer to user buffer and optional token allocation
    int  nbuf;      // size of the user buffer(minus token allocation)
    int nextchar;   // next character to get a character
   
    bool echo;      // flag to echo input characters or not
    const char *sprompt;  // pointer to const char* for prompt
    bool sflush;    // true to flush input before next ready(), or false to use type ahead

    char *bufx;     // pointer to end of received buffer with copy of previous input
    bool bfok;      // true, if bufx is valid
};

#define MAX_TENTRIES 10				// max number of command tables
#define MAX_TOKENS 10				// max number of tokens in a command line
#define MAX_INPCMD 80				// max size of a command line

// This macro is to be used in the main table. Declares printTables() as a help function
#define HELP(A) [](int n, char *arg[]){A->help(n,arg);}

class EXE
{
public:
    EXE();
	void initTables();
	void registerTable( CMDTABLE *t );
	void printTables( char *prompt = "" );
	void help( int n, char *arg[] );
	void brief( int n, char *arg[] );
		
	void dispatchConsole( const char *s  );			// must use printf()
	void dispatchBuf( const char *s, BUF &result );	// to be reprecated
	void dispatchBufPtr( const char *s, BUF *resptr );

	char *userCmd();							// returns pointer to user entered command
	bool cmd1stUpper();							// true if Meguno cmd
	bool missingArgs( int count );				// true if n<=count
	
private:
	
	int ntables;					// number of table entries
	CMDTABLE *table[MAX_TENTRIES];	// pointers to tables
	
    int ntokens;       				// number of tokens in a command line
	char *token[MAX_TOKENS];  		// pointers to tokens. All of them in the buffer below
	    
	char temp[ MAX_INPCMD ];		// temporary storage of user command line
	char *cmdhelp;					// temporary storage of command help
	
	int tokenize( char *userbuf, char *mytok[], int mxtok  );	// generic
};

#define BINIT( Bp, Arg)	BUF *Bp = (BUF *)Arg[0]
#define RESPOND( Bp, A, ... ) if( Bp ) Bp->add(A, ##__VA_ARGS__ ); else PF(A, ##__VA_ARGS__)

// Deprecate the following!
extern void errBufferedOnly();		// to be deprecated
extern void errMissingArgs();		// to be deprecated
extern char *missingArgs;
extern char *bufferedOnly;


