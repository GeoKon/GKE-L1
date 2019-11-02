#include <ESP8266WiFi.h> // only needed for RRI()
#include <string.h>
#include "bufClass.h"
#include "eepClass.h"

// -------------------------------- CLASS EEP --------------------------------------
	int EEP::notify( char *s )
	{
		PF( "*** EEP Class ERROR: %s ***\r\n", s );
		return -1;
	}
	void EEP::initHeadParms( uint16_t emagic, uint16_t nuser )
	{
		head.magic = emagic;
        head.headN = HEAD_PSIZE;
        head.userN = nuser;
		saveHeadParms();
	}
	bool EEP::checkEEParms( uint16_t emagic, uint16_t nuser )
	{
		fetchHeadParms();
		fetchWiFiParms();
		return  (head.reboots != 0 ) &&
				(head.magic == emagic) && 
				(head.headN == HEAD_PSIZE ) &&
				(head.userN == nuser ) &&
				(wifi.port != 0);
	}
	// If "" deletes value. If NULL or '*', leaves unmodified
    void EEP::initWiFiParms( char *myssid, 
                        char *mypwd, 
                        char *staticIP, 
                        int myport )    // initializes memory WiFi parms
    {
        if( myssid && (*myssid !='*') )
            strncpy( wifi.ssid, myssid, 16 );
        if( mypwd && (*mypwd !='*'))
            strncpy( wifi.pwd, mypwd, 16 );
        if( staticIP && (*staticIP !='*') )
            strncpy( wifi.stIP, staticIP, 16 );
        if( myport )
            wifi.port = myport;
		saveWiFiParms();
    }
    void EEP::saveHeadParms()
    {
        byte c;
        EEPROM.begin( HEAD_PSIZE + WIFI_PSIZE + USER_MAXSZ );
        EEPROM.put( 0, head );            
        EEPROM.end();                   
    }
    void EEP::saveWiFiParms()
    {
        EEPROM.begin( HEAD_PSIZE + WIFI_PSIZE + USER_MAXSZ );
        EEPROM.put( HEAD_PSIZE, wifi );                         // save the WiFi parm structure
        EEPROM.end();                   // commit
    }
    void EEP::saveUserStruct( byte *pntr, int actsiz )
    {
		//prnbuf("SAVE ", pntr, actsiz );
		if( actsiz > USER_MAXSZ )
		{
			notify( "USER_MAXSZ exceeded" );
			return;
		}
        byte c;
		byte *up = pntr;
		head.userN = actsiz;									// save this into header
		saveHeadParms();
		
		EEPROM.begin( HEAD_PSIZE + WIFI_PSIZE + USER_MAXSZ );		
		for( int i=0; i<actsiz; i++ )                       	// write user
			EEPROM.put( HEAD_PSIZE+WIFI_PSIZE+i, (c=*up++) );
        EEPROM.end();                   						// commit
    }
    int EEP::fetchHeadParms() 
    {
        EEPROM.begin( HEAD_PSIZE + WIFI_PSIZE + USER_MAXSZ );
        EEPROM.get( 0, head );                     				// fetch HEADER_MASK candidate
        EEPROM.end();                   						// commit
        return 0;
    }
    int EEP::fetchWiFiParms()
    {
        EEPROM.begin( HEAD_PSIZE + WIFI_PSIZE + USER_MAXSZ );
        EEPROM.get( HEAD_PSIZE, wifi );  
        EEPROM.end();    
        return 0;
    }
    int EEP::fetchUserStruct( byte *pntr, int maxsiz )
    {
		byte *p  = pntr;

		fetchHeadParms();										// get header
		if( maxsiz < head.userN )
			return notify( "Too small user buffer" );
		
        EEPROM.begin( HEAD_PSIZE + WIFI_PSIZE + USER_MAXSZ );
		int c;
		for( int i=0; i<head.userN; i++ )     
		{
			EEPROM.get( HEAD_PSIZE+WIFI_PSIZE+i, c );
			*p++ = c;
		}
        EEPROM.end(); 
		// prnbuf("FTCH ", pntr, head.userN );		
		return 0;
    }	
    void EEP::incrBootCount()
    {
        head.reboots++;
        saveHeadParms();
    }
    String EEP::getHeadString()
    {
        B128( s );
		s.set("Magic:%04x, Head_sz:%d, User_sz:%d, Boot_count:%d\r\n", 
                head.magic, head.headN, head.userN, head.reboots );
		return String( s.c_str() );
    }
    String EEP::getWiFiString()
    {
		B128( s );
		s.set("SSID:%s, ", 	 &wifi.ssid[0] );
		s.add("PWD:%s, ", 	 &wifi.pwd[0] );
		s.add("stIP:%s, ", 	 &wifi.stIP[0] );
		s.add("Port:%d\r\n", wifi.port );
		return String( s.c_str() );
    }
	void EEP::printHeadParms( char *prompt, BUF *bp )
	{		
		const char *hformat = "Magic:%04x, Head_sz:%d, User_sz:%d, Boot_count:%d\r\n";

		if( *prompt )
            PF( "%s\r\n", prompt );
		if( bp==NULL )
			PF( hformat, head.magic, head.headN, head.userN, head.reboots );
		else
			bp->add( hformat, head.magic, head.headN, head.userN, head.reboots );
	}
	void EEP::printWiFiParms( char *prompt, BUF *bp )
	{		
		const char *wformat = "SSID:%s, PWD:%s, StIP=%s, Port:%d, RSSI:%d\r\n";	
		if( *prompt )
            PF( "%s\r\n", prompt );
		if( bp==NULL )
			PF( wformat, &wifi.ssid[0], &wifi.pwd[0], &wifi.stIP[0], wifi.port, WiFi.RSSI() );
		else
			bp->add( wformat, &wifi.ssid[0], &wifi.pwd[0], &wifi.stIP[0], wifi.port, WiFi.RSSI() );
	}
	void EEP::printMgnWiFiParms( char *channel )
	{
		char *mformat;
		if( *channel )
			mformat = "{TABLE:%s|SET|%s|";
		else
			mformat = "%s{TABLE|SET|%s|";

		PF( mformat, channel,"reboots");PF( "%d| }\r\n", head.reboots );
		PF( mformat, channel, "ssid");	PF( "%s|Access Point}\r\n", wifi.ssid );
		PF( mformat, channel, "pwd");	PF( "%s|wpa2-psk}\r\n", wifi.pwd );
		PF( mformat, channel, "staticIP");	PF( "%s|if empty, use DHCP}\r\n", wifi.stIP );
		PF( mformat, channel, "port");	PF( "%d| }\r\n", wifi.port );		
	}
	bool EEP::setWiFiParm( char *name, char *value )
	{
		bool ok = true;
		
		if( *value == '*' )			// This corresponds to "" blank
			*value=0;
			
		if( strcmp(name, "ssid")==0 )
			strncpy( wifi.ssid, value, 16 );
		else if( strcmp(name, "pwd")==0 )
			strncpy( wifi.pwd, value, 16 );
		else if( strcmp(name, "staticIP")==0 )
			strncpy( wifi.stIP, value, 16 );
		else if( strcmp(name, "port")==0 )
			wifi.port = atoi( value );
		else if( strcmp(name, "reboots")==0 )
		{
			head.reboots = atoi(value);
			saveHeadParms();			// saves memory structures to eeprom
			return true;
		}	
		else
			ok = false;
		
		if( ok )
			saveWiFiParms();
		return ok;
	}
