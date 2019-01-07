#ifndef WEBSUPPORT_H
#define WEBSUPPORT_H

#include <cpuClass.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>

// ----------------- EXPORTED FUNCTIONS --------------------------

#define T_FILEIO  1
#define T_REQUEST 2
#define T_JSON    4
#define T_ARGS    8
#define T_ACTIONS 16

	bool 	initWiFi( char *ssid="kontopidis2GHz", char *pwd="123456789a", int tmoutsec=0, char *staticIP=NULL  );	// timeout 0 means "forever"
	void 	initFS(); 
	void 	initMDNS( char *name, int port); 
	             
	String 	formatBytes(size_t bytes);
	void 	showArgs( );
	void 	showJson( String json );
	
	bool 	handleFileRead(String path);
 	extern int gtrace;
#endif
