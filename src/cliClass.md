``

	void setup()
	{
		CLI cli;						// create a cli instance
		cli.init( ECHO, "prompt" );
		.....
		cli.prompt();
	}
	void loop()
	{
		if( cli.ready() )
		{
			char *s = cli.gets();		// break into tokens
			
			EXEC x( 400 or PRINTF );
			x.tokenize( s-<this allocates Buf );	
			x.dispatch( table ); OR
			x.dispatch( ToT );
			if( x.error() )
				....

			clients called (n,args)
			use 
		}
	}

```