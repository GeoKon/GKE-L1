// -------------- DEPENDENCIES FROM LIBRARIES ----------------

#include "webSupport.hpp"
#include <ESP8266mDNS.h>
#include <FS.h>

// --------------------- THIS MODULE --------------------------

	INI::INI( CPU &mycpu )
	{
		cpu = &mycpu;
	}

	bool INI::wifi( char *ssid, char *pwd, int tmout_sec, char *staticIP )
	{
	  PF( "Connecting to %s (%s)...\r\n", ssid, pwd );
	  
	  //WiFi.disconnect();
	  WiFi.mode(WIFI_STA);
	//  if( hname )
	//    if( *hname )
	//        WiFi.hostname( hname );
	 
	//  IPAddress ip; ip.fromString("192.168.0.102");
	//  IPAddress ip1; ip1.fromString("192.168.0.1");
	//  byte x[]={192,168,0,104};
	//  WiFi.config( x, ip1, IPAddress(255,255,255,0) );

		if( staticIP )
			if( *staticIP )
			{
				IPAddress ip, gateway(192,168,0,1), subnet( 255,255,255,0 );
				ip.fromString( staticIP );
				WiFi.config( ip, gateway, subnet );
			}
	  
	  WiFi.begin( ssid, pwd );
	  for( int i=0; (i<tmout_sec*2)||(tmout_sec==0); i++  )
	  {
		if( WiFi.status() == WL_CONNECTED )
		{
			cpu->led( OFF );
			WiFi.hostname("GKE_Station");	// set the wifi hostname
	  
			PF( "\r\nConnected to %s. Hostname is %s. ", WiFi.SSID().c_str(), WiFi.hostname().c_str() );
			PN( "IP Address " );
			PR( WiFi.localIP() );
			return true;
		}
		PF( "." );
		if( (i%40)==39 )
		  PF("\r\n");
		delay(500);
		if( i%2 ) cpu->led( ON );
		else cpu->led( OFF );
	  }
	  cpu->led( OFF );
	  PF( "\r\nCould not connect to %s\r\n", ssid );
	  return false;
	}

	void INI::mdns( char *name, int port)
	{
		if (!MDNS.begin( name )) 
			cpu->die("Error setting up MDNS responder!", 3 );

	// Add service to MDNS-SD

		MDNS.addService("GKETCP", "tcp", port );
		MDNS.addService("GKEUDP", "udp", 5353);
		
		PF("mDNS responder started. Ping for %s.local:%d\r\n", name, port);	
	}
	
// -------------------- 1a. FILE SYSTEM SUPPORT --------------------------------
	void INI::fs()
	{
	  if (SPIFFS.begin() == false ) 
		cpu->die("Unable to activate SPIFFS", 1);
	  
	  Dir dir = SPIFFS.openDir("/");
	  while (dir.next()) 
	  {    
		String fileName = dir.fileName();
		size_t fileSize = dir.fileSize();
		// if( gtrace & T_FILEIO )
		  // PF( "FS File: %s, size: %d\r\n", fileName.c_str(), formatBytes(fileSize).c_str() );
	  }
	}
	String INI::fileList()
	{
		Dir dir = SPIFFS.openDir("/");
		//String output("",500);
		BUF output(512);
		while (dir.next()) 
		{    
			String fileName = dir.fileName();
			size_t fileSize = dir.fileSize();
			output.add( "%s (%s)\r\n", fileName.c_str(), formatBytes(fileSize).c_str() );
		}  
		return String (!output);
	}
// ======================= 3. SUPORT UTILITIES ===========================

	WEB::WEB( CPU &mycpu, ESP8266WebServer &myserver )
	{
		cpu = &mycpu;
		server = &myserver;
	}
	
	String WEB::getContentType(String filename)
	{
	  if(server->hasArg("download")) return "application/octet-stream";
	  
	  else if(filename.endsWith(".htm")) return "text/html";
	  else if(filename.endsWith(".html")) return "text/html";
		
	  else if(filename.endsWith(".css")) return "text/css";
	  else if(filename.endsWith(".js"))  return "application/javascript";
	  else if(filename.endsWith(".png")) return "image/png";
	  else if(filename.endsWith(".gif")) return "image/gif";
	  else if(filename.endsWith(".jpg")) return "image/jpeg";
	  else if(filename.endsWith(".ico")) return "image/x-icon";
	  else if(filename.endsWith(".xml")) return "text/xml";
	  else if(filename.endsWith(".pdf")) return "application/x-pdf";
	  else if(filename.endsWith(".zip")) return "application/x-zip";
	  else if(filename.endsWith(".gz"))  return "application/x-gzip";
	  return "text/plain";
	}

	//format bytes
	String INI::formatBytes(size_t bytes)
	{
	  if (bytes < 1024)
	  {
		return String(bytes)+"B";
	  } else if(bytes < (1024 * 1024))
	  {
		return String(bytes/1024.0)+"KB";
	  } else if(bytes < (1024 * 1024 * 1024))
	  {
		return String(bytes/1024.0/1024.0)+"MB";
	  } else 
	  {
		return String(bytes/1024.0/1024.0/1024.0)+"GB";
	  }
	}
	void WEB::showArgs( )
	{
	  cpu->heapUpdate();
	  if( gtrace & T_REQUEST )
	  {
		PR( "*** request: "+ server->uri() );
	  }
	  if( gtrace & T_ARGS )
	  {
		int N=server->args();
		PN("=== arg: ");
		PR( N );
		
		if( N>0 )
		{
		  for( int i=0; i<N; i++ )
			PF( "Key:%s, Value:%s\r\n", !server->argName(i), !server->arg(i) );
		}
	  }
	}
	void WEB::showJson( String json )
	{
	  cpu->heapUpdate();
	  if( gtrace & T_JSON )
	  {
		PN( "--- " ); PR( json );
	  }
	}

	bool WEB::handleFileRead(String path)
	{
		if(path.endsWith("/")) 
			path += "index.htm";
		String contentType = getContentType(path);
		String pathWithGz = path + ".gz";
		
		if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path))
		{
			if(SPIFFS.exists(pathWithGz))
				path += ".gz";
		
			File file = SPIFFS.open(path, "r");
			server->streamFile(file, contentType);
			file.close();
			return true;
		}
		return false;
	}


	
	