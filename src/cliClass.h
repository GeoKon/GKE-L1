#ifndef CLICLASS_H
#define CLICLASS_H

#include <cpuClass.h>   // needed for the Buf class
/*
 * Example 1:
 *  in setup:       cli.init()
 *                  cli.start()
 *  in loop:        if( cli.ready() )
 *                      cli.gets();
 *                      cli.start();
 */
typedef struct
{
  char *cmd;
  char *help;
  void (*func)(int, char **);

} COMMAND;

enum echo_t
{
  ECHO_ON=true,
  ECHO_OFF=false
};

#define MAX_TOK 10
class CLI
{
private:
    char *buf;      // pointer to user buffer and optional token allocation
    int  nbuf;      // size of the user buffer(minus token allocation)
    int nextchar;   // next character to get a character
    
    char **tok;     // pointer to token buffer
    int ntok;       // max size of token pointers. Last token is always NULL;
    int atok;       // actual tokens
    
    bool echo;      // flag to echo input characters or not
    const char *sprompt;  // pointer to const char* for prompt
    bool sflush;    // true to flush input before next ready(), or false to use type ahead

    char *bufx;     // pointer to end of received buffer with copy of previous input
    bool bfok;      // true, if bufx is valid
    
public:
    CLI( int nchar=80, int ntokens=10 );
    ~CLI();
    void init( echo_t echo1, const char *prompt=NULL, bool flush1=false );
    void prompt();
    
    bool ready();		// true is one or more characters are waiting in the serial queue
    char *gets();		// returns a pointer to a string received after \r detected
    
    int tokenize();
    char **args();     // get pointer to 1st argument
    int nargs();       // get number of tokens

    bool dispatch( COMMAND *t );
    bool exec( COMMAND *t );                    // tokenize and dispatch. 
    bool exec( COMMAND *t, const char *s );	    // copies s to internal buffer, then dispatches

// Generic functions -- not requiring the Private variables

    int tokenize( char *userbuf, char *mytok[], int mxtok  );
    void printTable( COMMAND *t, char *select="" );
    void printArgs ( char *prompt1, int n, char *a[] );
};
// ---------------- exported symbols ----------------

extern void interact ( COMMAND *ToT[] );
extern void interact( COMMAND *t );
extern void printToT( COMMAND *ToT[] );
extern void execTables( COMMAND *ToT[], const char *s );

extern CLI cli;
extern Buf rbuf;
#endif
