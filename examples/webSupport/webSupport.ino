#include "webSupport.h"

// ----------------------- allocation of base classes -------------------------
    CPU cpu;
    ESP8266WebServer server;

// ------------------ Allocation of locals -------------------------------------

char *ssid = "kontopidis2GHz";
char *pwd = "123456789a";
char *staticIP="";
int webport = 80;

void setup()
{
	cpu.init();
 	//initFS();             		// In webSupport.cpp -- initialize file system

	initWiFi( ssid, pwd );
	
    server.on("/", 
    [](){
	    server.send(200, "text/html", 
	        "<h3><a href=\"page1\">Click to go to page1</a></h3>"\
	        "<h3><a href=\"page2\">Click to go to page2</a></h3>");	
	});
    server.on("/page1", 
    [](){
        server.send(200, "text/html", "<h3><a href=\"page2\">Click to go to page2</a></h3>");   
    });
    server.on("/page2", 
    [](){
        server.send(200, "text/html", "<h3><a href=\"page1\">Click to go to page1</a></h3>");   
    });
		
	server.begin( webport );
	PF("HTTP server started at port %d\r\n", webport);
    PF( "Only serves two pages. Does not access the file system!\r\n" );
	initMDNS( "gke8266", webport );
	delay( 4000 );
}
void loop()
{
  	server.handleClient();
}
