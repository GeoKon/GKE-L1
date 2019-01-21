#include "cliClass.h"		// this already includes cpuClass.h

// ------------------------------ CLI CLASS ------------------------------------------

	CLI::CLI( int nchar )
	{
		buf  = new char [ nchar ];
		nbuf = nchar;
		bufx = buf+nbuf/2-1;

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
		memset( buf, 0, nbuf );		// initialize to zero
		sflush  = flush;
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
			  if( echo ) PN( "\r\n" );
			  return true;
			}
			else if( c==0x1B )  // flush characters already entered
			{
				for(; nextchar>0; nextchar-- )
					PN( "\x08 \x08" );    
				buf[ 0 ] = 0;
			}
			else if( (c==('A'-0x40)) && bfok )  // append with old buffer
			{
				strcpy( buf+nextchar, bufx );
				PN( buf+nextchar );
				nextchar = strlen( buf );
			}
			else if( c=='H'-0x40 )
			{
			  if( nextchar>0 )
			  {
				nextchar--;
				if( echo ) PN( "\x08 \x08" );
			  }
			}
			else
			{
			  buf[ nextchar++ ] = c;
			  if( nextchar > nbuf-2 )
				nextchar = nbuf-2;
			  if( echo ) PN( (char)c );
			}
		}
	}             
	char *CLI::gets()
	{
	  return buf;
	}
	void CLI::prompt()  // optional if prompt is not needed
	{
		if( *sprompt )
			PN( sprompt );
		if( sflush )
			while( Serial.available() )
				Serial.read();
	}

// ------------------------------ EXE CLASS ------------------------------------------

	EXE::EXE(  )
	{
		ntables = 0;
		ntokens = 0;
	}
	void EXE::registerTable( CMDTABLE *t )
	{
		if( ntables < MAX_TENTRIES )
			table[ ntables++ ] = t;
	}
	void EXE::printTables( char *prompt )
	{
		if( *prompt )
			PF( "%s\r\n", prompt );
		for( int i=0; i<ntables; i++)
		{
			CMDTABLE *row = table[ i ];
			for( int j=0; row->cmd ; j++, row++ )
				PF( "%s\t%s\r\n", row->cmd, row->help );
		}
	}
	// void EXE::getTables( char *prompt, Buf &bf )
	// {
		// bf.init();
		// if( *prompt )
			// bf.add( "%s\r\n", prompt );
		// for( int i=0; i<ntables; i++)
		// {
			// CMDTABLE *row = table[ i ];
			// for( int j=0; row->cmd ; j++, row++ )
				// bf.add( "%s\t%s\r\n", row->cmd, row->help );
		// }
	// }

	// -------------------------------------------------------------------
	// Generic tokenizer, Does not depend on privates of this class
	int EXE::tokenize( char *userbuf, char *mytok[], int mxtok  )
	{
	  int nt;
	  char *p = strtok( userbuf, " ");
	  for( nt=0; (nt<MAX_TOKENS) && (p!=NULL); nt++ )
	  {
		mytok[ nt ] = p;
		//PF("[%d]=%s\r\n", nt, p );
		p = strtok( NULL, " " );   // break 
	  }
	  // PF("DONE\r\n");
	  return nt;
	}
	void EXE::respond( const char *fmt, ... )
	{
		// va_list va;
		// va_start(va, fmt);
		// char buf[vsnprintf(NULL, 0, fmt, va) + 1];
		// vsprintf(buf, fmt, va);
		// va_end(va);
		
		va_list va;
		va_start(va, fmt);
		if( cmdbf==NULL ) 
			vprintf( fmt, va);		// print directly
		else
		{							// buffer directly
			char *p = cmdbf->pntr;
			int N = strlen(p);
			vsnprintf( p+N, cmdbf->maxsiz-N-1, fmt, va );
			*(cmdbf->pntr + cmdbf->maxsiz-1)=0;			// append EOS
		}	
		va_end(va);
	}
	void EXE::dispatch( Buf &bf )  
	{
		dispatch( bf, NULL );
	}
	void EXE::dispatch( char *s )  
	{
		Buf bf(0);					// dummy buffer
		dispatch( s, NULL );
	}
	void EXE::dispatch( Buf &bf, Buf *result )  
	{
		return dispatch( bf.pntr, result );
	}
	void EXE::dispatch( char *s, Buf *result )    
	{
		strncpy( temp, s, MAX_INPCMD-1 );			
		temp[ MAX_INPCMD-1 ] = 0;
		ntokens = tokenize( temp, token, MAX_TOKENS );	// inserts \0's into s
		
		// PF("ntokens = %d\r\n", ntokens );
		// for( int i=0; i<ntokens; i++ )
			// PF("token[%d] = %s\r\n", i, token[i] );
	
		char *cmnd = token[0];
		if( (ntokens==0) || (*cmnd == 0) )			// empty command
			return;
		
		cmdbf = result;						// this is a pointer to user buffer
			
		for( int i=0; i<ntables; i++)	// each table, one at a time
		{
			for( CMDTABLE *row=table[i]; row->cmd ; row++ )
			{
			  if( strcmp( row->cmd, cmnd ) == 0 )
			  {
				// if( result )
					// result->init();
				(*row->func)( ntokens, token );
				return;
			  }
			}
		}
		respond( "[%s] not found\r\n", cmnd );
	}
	
	void EXE::help( int n, char *arg[] )
	{
		char *mask;
	    mask = (n<=1) ? (char *)"" : arg[1];
		// if( cmdbf )
				// cmdbf->init();
		for( int i=0; i<ntables; i++)
		{
			CMDTABLE *row = table[ i ];
			//PF("Mask is %s, table %d\r\n", mask, i );
			
			for( int j=0; row->cmd ; j++, row++ )
				if( (strncmp( row->cmd, mask, strlen(mask)) == 0) || (*mask==0) )
				{	//PF( "{%s\t%s}\r\n", row->cmd, row->help );
					respond( "%s\t%s \r\n", row->cmd, row->help );
				}
				
		}
	}
