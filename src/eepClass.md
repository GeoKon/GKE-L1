## EEP Class
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

