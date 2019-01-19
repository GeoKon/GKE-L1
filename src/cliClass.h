#ifndef CLICLASS_H
#define CLICLASS_H

#include <cpuClass.h>   // needed for the Buf class
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
  void (*func)(int, char **, Buf & );

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

#define HELP(A) [](int, char **, Buf&){A.help("");}

class EXE
{
public:
    EXE();									
	void registerTable( CMDTABLE *t );
	void printTables( char *prompt = "" );
	void getTables  ( char *prompt, Buf &temp );
	void printHelp( char *mask = "" );
	void getHelp( char *mask, Buf &temp );
	
	void dispatch( char *s );				// must use printf()
	void dispatch( Buf &cmd );				// must use printf()
	
	void dispatch( char *s,  Buf &result );
	void dispatch( Buf &cmd, Buf &result );

private:
	int ntables;					// number of table entries
	CMDTABLE *table[MAX_TENTRIES];	// pointers to tables
	
    int ntokens;       				// number of tokens in a command line
	char *token[MAX_TOKENS];  		// pointers to tokens. All of them in the buffer below
	    
	char temp[ MAX_INPCMD ];		// temporary storage of user command line
	
	int tokenize( char *userbuf, char *mytok[], int mxtok  );	// generic
};

#endif
