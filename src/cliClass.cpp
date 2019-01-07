#include "cpuClass.h"
#include "cliClass.h"

// ------------------------- ALLOCATION OF CLI INSTANCE ------------------------------
//  CLI cli;
// ------------------------- ALLOCATION OF BUF INSTANCE ------------------------------
//  Buf rbuf( 400 );    // CLI Response buffer. Use RESPONSE rbuf.add
// ------------------------------ CLI CLASS ------------------------------------------

CLI::CLI( int nchar, int ntokens )
{
    buf  = new char [ nchar + ntokens * sizeof( char *) ];
    nbuf = nchar;
    bufx = buf+nbuf/2-1;

    char *p = &buf[ nchar ];
    tok = (char **)p;
    ntok = ntokens;

    init( ECHO_OFF, "", false );
}
CLI::~CLI()
{
    delete [] buf;
}
void CLI::init( echo_t echo1, const char *prompt1, bool flush )
{
    echo    = echo1;
    sprompt = (prompt1!=NULL) ? prompt1 : "";
    nextchar = 0;
    sflush  = flush;

    bfok = false;   
    atok = 0;       // no tokens
}
bool CLI::ready()
{
    int c;
    
    for(; Serial.available();)
    {
        c = Serial.read();  
        if( c<0 )           // check if a character is waiting
          return false;
        
        yield();

        if( c=='\r' )
        {
          buf[ nextchar ] = 0;
          if( nextchar < (nbuf/2-2) )            // is there a room to copy input?
          {
            strcpy( bufx, buf );
            bfok = true;
          }
          else
            bfok = false;

          nextchar = 0;
          if( echo ) Serial.print( "\r\n" );
          return true;
        }
        else if( c==0x1B )  // flush characters already entered
        {
            for(; nextchar>0; nextchar-- )
                Serial.print( "\x08 \x08" );    
            buf[ 0 ] = 0;
        }
        else if( (c==('A'-0x40)) && bfok )  // append with old buffer
        {
            strcpy( buf+nextchar, bufx );
            Serial.print( buf+nextchar );
            nextchar = strlen( buf );
        }
        else if( c=='H'-0x40 )
        {
          if( nextchar>0 )
          {
            nextchar--;
            if( echo ) Serial.print( "\x08 \x08" );
          }
        }
        else
        {
          buf[ nextchar++ ] = c;
          if( nextchar > nbuf-2 )
            nextchar = nbuf-2;
          if( echo ) Serial.print( (char)c );
        }
    }
}             
void CLI::prompt()  // optional if prompt is not needed
{
    if( *sprompt )
        Serial.print( sprompt );
    if( sflush )
        while( Serial.available() )
            Serial.read();
}
char *CLI::gets()
{
  return buf;
}

int CLI::tokenize()                            // tokenize internal gets()
{
    return atok = tokenize( buf, tok, ntok );
}
char **CLI::args()  // can be called AFTER tokenize();
{
    return &tok[0];
}
int CLI::nargs()
{
    return atok;
}
// Call function indicated in the command table
// Use after you have tokenized string
bool CLI::dispatch( COMMAND *t )    
{
    COMMAND *row=t;
    bool done = false;
    char *cmnd = tok[0];
    for( ; row->cmd ; row++ )
    {
      if( strcmp( row->cmd, cmnd ) == 0 )
      {
        (*row->func)( atok, &tok[0] );
        done = true;
        break;
      }
    }
    return done;    // true=command found; false=not found
}
bool CLI::exec( COMMAND *cm )                   // use internal buffer
{
    if( *buf==0 )                               // empty string
        return true;

    tokenize();
    return dispatch( cm );
}
bool CLI::exec( COMMAND *cm, const char *s )    // use external buffer
{
    strncpy( buf, s, nbuf-1 );
    buf[ nbuf-1 ]=0;
    return exec( cm );
}

// -------------------------------------------------------------------
// Generic tokenizer, Does not depend on privates of this class
int CLI::tokenize( char *userbuf, char *mytok[], int mxtok  )
{
  int nt;
  char *p = strtok( userbuf, " ");
  for( nt=0; (nt<mxtok) && (p!=NULL); nt++ )
  {
    mytok[ nt ] = p;
    //Serial.printf("[%d]=%s\r\n", nt, p );
    p = strtok( NULL, " " );   // break 
  }
  // Serial.printf("DONE\r\n");
  return nt;
}
// Generic print command table
//void CLI::printTable( COMMAND *t, char *prompt1 )
//{
//  COMMAND *row = t;
//  if( *prompt1 )
//    Serial.printf("%s\r\n", prompt1 );
//  for( int i=0; row->cmd ; i++, row++ )
//  {
//    Serial.print( row->cmd );
//    Serial.print( "\t" );
//    Serial.println( row->help );
//  }
//}

// Generic print command table
void CLI::printTable( COMMAND *t, char *sel )
{
  COMMAND *row = t;
  int N = 0;
  if( sel!=NULL )
    N =strlen( sel );
  for( int i=0; row->cmd ; i++, row++ )
  {
    if( (N<=0) || ( strncmp( row->cmd, sel, N )==0 ) )
        Serial.printf( "%s\t%s\r\n", row->cmd, row->help );
  }
}
// Generic print command arguments
void CLI::printArgs( char *prompt1, int n, char *a[] )
{
  if( *prompt1 )
    Serial.printf("%s [%d tokens]\r\n", prompt1, n );
  for( int i=0; i<n; i++ )
    Serial.printf( "arg[%d]=\"%s\"\r\n", i, a[i] );
}

// ------------------------- Command Line Handler (multiple tables) ---------------------

void interact( COMMAND *table )
{
    cli.prompt();
    for(;;)
    {
        if( cli.ready() ) 
        {
            rbuf.init();                            // initialize CLI buffer. Needed for responses, not for cli.gets()
            int ok = cli.exec( table );             // this uses the INTERNAL buffer of CLI
            if( !ok )
                rbuf.set("Cmnd \"%s\" not found\r\n", *cli.args() );
            rbuf.print();        
            cli.prompt();
        }
    }
}

void interact( COMMAND *ToT[] )
{
    char s[80];                                     // input buffer
    cli.prompt();
    for(;;)
    {
        if( cli.ready() ) 
        {
            rbuf.init();                                      // initialize CLI buffer. Needed for responses, not for cli.gets()
            execTables( ToT, strncpy( s, cli.gets(), 80 ) );    // scan an execute all tables
            rbuf.print();
            cli.prompt();
        }
    }
}
void execTables( COMMAND *ToT[], const char *s )
{
    int ok;
#ifndef MULTI
    rbuf.init();                             // initialize CLI buffer. Needed for responses, not for cli.gets()

    for( int it = 0; ToT[it]!=NULL; it++ )
    {
        ok = cli.exec( ToT[it], s );        // executed OK?
        if( ok )
            break;                          // results now are placed into buffer
        // else, continue looping
    }
    if( !ok )
        rbuf.set("Cmnd \"%s\" not found\r\n", *cli.args() );
#else
    ok = cli.exec( etable );           // this uses the INTERNAL buffer of CLI
    if( !ok )
        rbuf.set("Cmnd \"%s\" not found\r\n", *cli.args() );
#endif
}
void printToT( COMMAND *ToT[] )
{
    COMMAND **p;
    for( int it=0; ToT[it] != NULL; it++ )
    {
        p = &ToT[it];           // point to first table
        COMMAND *row = p[0];
        for( int i=0; row->cmd; i++, row++ )
            PF( "%s\t%s\r\n", row->cmd, row->help );
    }
}
