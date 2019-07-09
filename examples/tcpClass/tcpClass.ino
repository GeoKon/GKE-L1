#include <ESP8266WiFi.h>
#include <cpuClass.h>
#include "tcpClass.h"

const char* ssid = "kontopidis2GHz";
const char* password = "123456789a";

TCP tcp;
CPU cpu;

void setup()
{
	cpu.init();

	PF("Connecting to %s\r\n", ssid);
	
	WiFi.mode(WIFI_STA);
	
	IPAddress ip(192,168,0,120), gateway(192,168,0,1), subnet( 255,255,255,0 );
	WiFi.config( ip, gateway, subnet );
				
	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) 
	{
    	delay(500);
    	Serial.print(".");
  	}
  	PF("\r\nWiFi connected. IP=");
  	PR(WiFi.localIP());

  	tcp.init( 23, "cmd: ", true );
  	tcp.prompt();
}

// https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/server-examples.html
// https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/server-class.html?highlight=tcp%20server
// https://forum.arduino.cc/index.php?topic=474378.0

void loop()
{
	if (tcp.ready())
  	{
	    char *p = tcp.gets();
	    PF("Received: %s\r\n", p );
	    tcp.prompt();
  	}
}
