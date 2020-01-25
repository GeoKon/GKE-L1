## NMP Class
Named Parameter Class

## Reference of Methods

Parameter Registration

```c++
  void resetRegistry();
  bool registerParm( *pname, ptype, *ppntr, *info="", *format="" );

size_t getSize(); 				// size of the data structure saved
   int getParmCount();	 		// returns nparms. Useful to iterate
```

Parameter Access

```c++
 int getParmIndex   ( *pname );	 		// returns -1 if not found	

char* getParmName    ( int idx );		// returns 'type' 'd' 'f' 's' or 0
  int getParmType    ( int idx );		// returns 'type' 'd' 'f' 's' or 0
char* getParmInfo    ( int idx );		// returns 'info' or "unknown parm"	
char* getParmValueStr( int idx );		// returns index or -1

 bool setParmValue( *pname, *ppntr );	// assumes previously defined type
 bool setParmByStr( *uname, *s );		// assumes previously defined type
```

Printing Parameters

```c++
void printParm  	( char *name, BUF *bp=NULL );
void printAllParms 	( char *prompt="", BUF *bp=NULL );

void printMgnParm  	 ( char *channel, char *name );
void printMgnAllParms( char *channel=""  );
void printMgnInfo	 ( char *channel, char *value, char *info );
```

Allows naming of scattered variables to a single buffer area. Handles floats, integers and string variables,  aka parameters. This class is to be used in combination with the EEP Class to serialize and save  parameters. 

**registerParm( pname, ptype, pvalue, phelp )**

> Registers a parameter of a given type and value. Possible types are:
>--  'f' for floats (4-bytes)
>--  'd' for ints (4-bytes)
>--  's' for strings (up to 20-bytes including EOS)
>The `phelp` argument is optional "help string". If given it assumes this as the printing format for the value.
>The number of "spaces" for parameters is limited. If exceeded, the `ser.notify()` is called.

**resetRegistry()**
> Call to restart registration of parameters

**getSize()**

> Number of bytes required for serialization (aka encoding). Can be called at any time during or after parameter registration.

**getParmCount()**

> Number of parameters registered.

**setParmValue( pname, pvalue )**

> Modifies value of a parameter according to previously set type. It is expected that pvalue points to the new value of the appropriate type. Returns 0 if parameter not found.

**getParmType( pname )**

> Returns the type of a parameter. 0 if not found.

**getParmIndex( pname )**

> Returns the index of a parameter. -1 if not found.

**getParmString( index )**  

> Returns a String using the format **pname <formatted value>**
> Iterate **index** from 0...ser.nparms

**printParms( prompt )** 

> Prints all parameters as **pname=<formatted value>\r\n**

#####                                                                             ... work in progress!!!...

#### Typical Code using the NMP Class

```c++
	float f1 = 10.3;
	int i1 = 20;
	char sn[20] = "test";

... registration ...
	ser.registerParm( "volts", 'f', &f1, "%4.1f volts" );
	nmp.registerParm( "iter",  'd', &i1, "Number %d iterations" );
	nmp.registerParm( "S/N",   's', &sn, "%-16s" );

... set new values and print ...
	f1 = 1.23;							// usual way
	int i = 23;
	nmp.setParmValue( "iter", &i );	// alternative parm set

	nmp.printParms();

... serialize (i.e. encode) saveto EEPROM ...
	byte buf[200];
	int n nmp.encodeParms( buf, sizeof buf );	// n is the actual byte size
	eep.saveUserStruct( buf, n );
	-- or --
	nmp.encodeParms( buf, sizeof buf );	
	eep.saveUserStruct( buf, nmp.bcount() );

... if parameters are modified, the EEPROM is updated ...

	f1 = 45.56;
	i1 = 34;
	nmp.encodeParms( buf, sizeof buf );
	eep.saveUserStruct( buf, nmp.lastSz );
```

###Typical main() initialization using EEP and ser Classes###

```c++
	EEP eep;
	NMP nmp;

	class Globals
	{
		float f1;
		int i1;

		byte buf[200];
		int encsz;			// set by register() below

		void init()
		{
			f1 = 20.34;
			i1 = 10;
		}
		void register()
		{
			ser.resetRegistry();
			ser.registerParm( "volts", 'f', &f1, "%4.1f volts" );
			ser.registerParm( "iter",  'd', &i1, "Number %d iterations" );
			encsz = ser.bcount;
		}
		void save()
		{
			ser.encodeParms( buf, sizeof buf );
			eep.saveUserStruct( buf, encsz );
		}
		void fetch()
		{
			eep.fetchUserStruct( buf, encsz );
			ser.decodeParms( buf, sizeof buf );
		}
	} my;

	void main()
	{
		my.register();
		
		if( !eep.checkHeadParms( MAGIC, my.encsz ) )
		{
			eep.initHeadParms(...);		// not necessary to add ser.bcount() here
			eep.initWiFiParms(...);
			
			my.init();
			my.save();
		}
		my.fetch();
		....
		my.f1 = 100.1;					// modify user parm
		my.save();						// save to eeprom
	}
```
