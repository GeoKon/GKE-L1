#pragma once

#include "cpuClass.h"
#include <ESP8266WebServer.h>

// ----------------- EXPORTED FUNCTIONS --------------------------

#define T_FILEIO  1
#define T_REQUEST 2
#define T_JSON    4
#define T_ARGS    8
#define T_ACTIONS 16

class INI
{
private:
	CPU *cpu;
	String 	formatBytes(size_t bytes);
		
public:
	
	INI( CPU &mycpu );
	
	bool 	wifi( 	char *ssid="kontopidis2GHz", 
						char *pwd="123456789a", 
						int tmoutsec=0, 
						char *staticIP=NULL  );	// timeout 0 means "forever"
	void 	fs(); 
	void 	mdns( char *name, int port); 
	String 	fileList();
};

class WEB
{
private:
	ESP8266WebServer *server;
	CPU *cpu;
		
	String  getContentType(String filename);

	
public:
	WEB( CPU &mycpu, ESP8266WebServer &myserver );
	int gtrace;

	void 	showArgs( );
	void 	showJson( String json );

	bool 	handleFileRead(String path);
};

