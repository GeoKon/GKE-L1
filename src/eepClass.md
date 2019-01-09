## HOW TO DO

- To modify WiFi parameters use `updateWiFiParms()`
- To modify a user parameter, use: `userstruct.X = new; eep.saveParms( USER_PARMS );`
- To increment Reboot Counter, use: `updateReboot()`

###Main Code###

	class structure with parameters
	class initialization myInit()
	
	void main()
	{
		...
		eep.registerUserParms( structure, size format )
		eep.registerInitCallbk( func );
		
		bool ok = eep.fetchParms();
		if( !ok )
		{
			eep.initWiFiParms(...)
			eep.initUserParms()
			eep.saveParms();
		}
		eep.updateRebootCount();
		PR( eep.getParms("prompt") );
	}


 Method                | Access EEPROM | Modifies Struct 
-----------------------|---------------|----------------+
  saveParms( se] )     |    yes        |          no
  fetchParms( sel )    |    yes        |          yes
  
  getParms( sel )      |    no         |          no
  initWiFiParms(...)   |    no         |          yes 
  initUserParms()      |    no         |          yes
  
  registerUserParms()  |   no          |         no
  registerInitCallBk() |    no         |          no
    
  updateWiFi()    mod&save | yes        |         yes    
  updateReboot()  mod&save | yes        |         yes
   
  formatUserParms(...)  |   no           |       no

 