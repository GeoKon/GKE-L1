#include "cpuClass.h"
#include "webSupport.h"

// ----------------------- allocation of base classes -------------------------
    CPU cpu;
    ESP8266WebServer server;
    INI ini( cpu );
    
// ------------------ Allocation of locals -------------------------------------

char *ssid = "kontopidis2GHz";
char *pwd = "123456789a";
char *staticIP="";
int webport = 80;

void setup()
{
	cpu.init();
 
    ini.fs();
	ini.wifi( ssid, pwd );
	
    server.on("/", 
    [](){
	    server.send(200, "text/html", 
	        "<h3><a href=\"page1\">Click to go to page1</a></h3>"\
	        "<h3><a href=\"page2\">Click to go to page2</a></h3>");	
	});
    server.on("/page1", 
    [](){
        server.send(200, "text/html", "<h3><a href=\"page2\">Click to go to page2</a></h3>\
                                       <h3><a href=\"\\\">    Click to go to index</a></h3>");   
    });
    server.on("/page2", 
    [](){
        server.send(200, "text/html", "<h3><a href=\"page1\">Click to go to page1</a></h3>\
                                       <h3><a href=\"\\\">    Click to go to index</a></h3>");      
    });
    server.on("/dir", HTTP_GET,
    [](){
        //ini.showArgs();
        server.send(200, "text/plain", ini.fileList() );     
    });
	server.begin( webport );
	PF("HTTP server started at port %d\r\n", webport);
    PF( "Only serves two pages and /dir of the file system!\r\n" );
	
	ini.mdns( "gke8266", webport );
	delay( 4000 );
}
void loop()
{
  	server.handleClient();
}
