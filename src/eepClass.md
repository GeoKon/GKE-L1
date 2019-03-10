## How to Use the EEP Class
EEP stands for EEPROM.

**To just read EEPROM**
> - fetchHeadParms(), checkHeadParms(...)
> - fetchWiFiParms()

**To set and save to EEPROM**
> - initHeadParms(...), saveHeadParms()
> - initWiFiParms(...), saveWiFiParms()
> - updateBootCount(...)

**To get a String or Print parameters**
> - getHeadString(), printHeadParms( prompt )
> - getWiFiString(), printWiFiParms( prompt )
  
### Rules ###
- At any point of time, the code has consistent parameters as the EEPROM
- To use/read a parameter, just refer to the data structure
- To modify a parameter, modify the data structure, then use eep.saveXXXParms()
 
###Typical code###

	... initialization ...
		if( !eep.checkHeadParms(...) )
		{
			eep.initHeadParms(...);	// sets header parameters
			eep.initWiFiParms(...);	// sets WiFi parameters
		}
		eep.incrBootCount();		// with no argument autoincrements bootcount
	
	... use any of the WiFi parameters ...
		int myport = eep.head.port;

	... to modify a WiFi parameter ...
		eep.head.port = 8080;
		strcpy( eep.head.ssid, "newSSID" );
		eep.saveWiFiParms();

## Reference of the EEP Class

**eep.initHeadParms( magic=MAGIC, n-user=0 )**
> Initializes HEAD. Optionally user specifies `magic` and number of user parameters. Does not change The `reboot` counter. Saves to EEPROM. If you want to zero the reboot counter, use,

		eep.head.reboot = 0;
		eep.saveHeadParms();
> Setting `reboot` counter to zero, forces `eep.checkEEParms()` (see below) to be `false`.

**eep.initWiFiParms( ssid, pwd, stip, port )**
> Initializes the WiFi parameters and saves the to EEPROM. If WiFi params were previously fetched, NULL leaves parameter unchanged.

**eep.fetchHeadParms(), eep.fetchWiFiParms()** 
> Fetches header parameters and populates the `eep.head.xxx` data structure
> Fetches WiFi parameters

**eep.checkEEParms( magic=MAGIC, n-user=0 )** 
> Fetches header parameters and checks magic code and number of user parameters. Returns `TRUE` if correct.
 
**eep.saveHeadParms(), eep.saveWiFiParms()**
> saves the header and WiFi Parameters 
	
**eep.incrBootCount()**
> Increments boot count and saves the new value.

**eep.saveUserStruct( byte *b, actsize )**
> Saves a user specified blob of a given size. The actual size is saved in the header as `eep.head.userN`

**eep.fetchUserStruct( byte *b, maxbuf )**
> Retrieves a used blob according to previously header `eep.head.userN` Assumes that `maxbuf` is greater than required -- otherwise calls `eep.notify()`. Returns error code.

**virtual int eep.notify( char *, int )**
By default, prints a string. Can be replaced by another notification method. Used by above routines if an unexpected error occurs.

--------------

## Reference of the SER Class

SER stands for "Serialize". Allows serialization of scattered variables to a single buffer area. Handles floats, integers and string variables,  aka parameters. This class is to be used in combination with the EEP Class to save serialized parameters. 

**ser.registerParm( pname, ptype, pvalue, phelp )**
> Registers a parameter of a given type and value. Possible types are:
>--  'f' for floats (4-bytes)
>--  'd' for ints (4-bytes)
>--  's' for strings (up to 20-bytes including EOS)
>The `phelp` argument is optional "help string". If given it assumes this as the printing format for the value.
>The number of "spaces" for parameters is limited. If exceeded, the `ser.notify()` is called.

**ser.resetRegistry()**
> Call to restart registration of parameters

**ser.nbytes**
> Number of bytes required for serialization (aka encoding). Can be called at any time during or after parameter registration.

**ser.nparms**
> Number of parameters registered.

**ser.setParmValue( pname, pvalue )**
> Modifies value of a parameter according to previously set type. It is expected that pvalue points to the new value of the appropriate type. Returns 0 if parameter not found.

**ser.getParmType( pname )**
> Returns the type of a parameter. 0 if not found.

**ser.getParmIndex( pname )**
> Returns the index of a parameter. -1 if not found.

**ser.getParmString( index )**  
> Returns a String using the format **pname <formatted value>**
> Iterate **index** from 0...ser.nparms

**ser.printParms( prompt )** 
> Prints all parameters as **pname=<formatted value>\r\n**

**ser.encodeParms( byte *p, maxsize, nparms=ALL_PARMS )**
> Serializes the first `nparms` into buffer `p` of `maxsize`. Calls `ser.notify()` if insufficient storage area and returns 0. Otherwise returns the number of bytes used from buffer `p`. The argument `nparms` can be greater than the registered parameters -- in this case all parameters are serialized. 
> The buffer is structured using a header of
> 
>- byte size used for parameters (uint16_t)
>- number of parameters included (uint16_t)
>- parm types bit mask 1=int, 2=float, 3=string (uint32_t)
>
> Use **ser.encodedSize( p )** and **ser.encodedParms( p )** to see the bytes size and number of parameters respectively. 

**ser.decodeParms( byte *p )**
> Decodes the first `nparms` from buffer `p` into user space.

###Typical Code using the SER Class###

		float f1 = 10.3;
		int i1 = 20;
		char sn[20] = "test";

	... registration ...
		ser.registerParm( "volts", 'f', &f1, "%4.1f volts" );
		ser.registerParm( "iter",  'd', &i1, "Number %d iterations" );
		ser.registerParm( "S/N",   's', &sn, "%-16s" );

	... set new values and print ...
		f1 = 1.23;							// usual way
		int i = 23;
		ser.setParmValue( "iter", &i );	// alternative parm set

		ser.printParms();

	... serialize (i.e. encode) saveto EEPROM ...
		byte buf[200];
		int n ser.encodeParms( buf, sizeof buf );	// n is the actual byte size
		eep.saveUserStruct( buf, n );
		-- or --
		ser.encodeParms( buf, sizeof buf );	
		eep.saveUserStruct( buf, ser.bcount() );

	... if parameters are modified, the EEPROM is updated ...
	
		f1 = 45.56;
		i1 = 34;
		ser.encodeParms( buf, sizeof buf );
		eep.saveUserStruct( buf, ser.lastSz );

###Typical main() initialization using EEP and ser Classes###

		EEP eep;
		SER ser;

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
