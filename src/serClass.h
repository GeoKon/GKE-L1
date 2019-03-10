#ifndef SERCLASS_H
#define SERCLASS_H

// See eepClass.md for use

#include <EEPROM.h>
#include <cpuClass.h>

// -------------------------- SERIALIZER ------------------------------
#define LSHIFT(A) A=(A<<2)&(~3)

#define MAX_USERPARMS 10			// max number of user parms
#define USER_STR_SIZE 20			// max size of strings stored in eeprom

static char *BAD_ENCODING = "Bad Encoding";
static char *PARM_OVERRUN = "Parm Overrun";
static char *INSUF_BUFFER = "Insufficient Buffer";
static char *BAD_PARM_TYPE = "Bad Parm Type";
static char *INSUF_NPARMS = "Insufficient nParms";

#define MAX_BUFSIZE 256				// max encoding buffer

typedef struct						// buffer header
{
	uint16_t ibytes;
	uint16_t iparms;
	uint32_t itypes;
} HEADER;

#define HEADER_SIZE sizeof( HEADER )

class SER
{
private:
	struct
	{
		const char *name;		// userParm name
		const char *help;		// print format
		void *pntr;				// pointer to user value of this parm
		byte type;				// userParm 'f', 'd' or 's'
	
	} prm[MAX_USERPARMS];

public:	

	uint16_t nbytes;			// bytes needed to encode
	uint16_t nparms;			// number of saved parms
	uint32_t ntypes;			// bit mask
	
public:	
	SER();
	void 	resetRegistry();
	bool	registerParm( const char *pname, char ptype, void *ppntr, const char *format="" );
    void 	registryStatus( char *prompt="" );
	
	bool 	setParmValue( const char *pname, void *ppntr );
    bool	setParmByStr( const char *uname, char *s );
	
	int 	getParmType ( const char *pname );
	int 	getParmIndex( const char *pname );
	String 	getParmString( int ind );
	
 	bool encodeParms( byte *p, int maxsiz, int np = MAX_USERPARMS );
	bool decodeParms( byte *p );
	
	void printParms( char *prompt="" );
	void printBuf( char *prompt, byte *base );	// diagnostics
	bool notify( char *s );
};
extern SER ser;

#ifdef IGNOREME
				#define EEP_MAGIC ('K'*256+'O')
				#define CHECK PF

				#define HEAD_PSIZE 8            	// sizeof( head_t )
				#define WIFI_PSIZE 3*16+2   		// sizeof( wifi_t )
				#define USER_MAXSZ 200				// max byte size user parms allowed

				class EEP
				{
				public:	
					struct head_t					// ACCESSIBLE HEAD STRUCTURE
					{
						uint16_t magic;
						uint16_t headN;    			// byte count of this HEAD
						uint16_t userN;    			// byte count of this WIFI
						uint16_t reboots;  			// increments avery time reboots
					} head;         				
					
					struct wifi_t					// ACCESSIBLE WIFI STRUCTURE
					{
						char ssid[16];
						char pwd [16];
						char stIP[16];
						uint16_t port;
					} wifi;
					
					void initHeadParms(				// Initializes HEAD. Forces MAGIC and BOOTCOUNT to zero.
						int emagic = EEP_MAGIC,		// Saves parms
						int nuser = 0,
						int bootcount = -1);

					bool checkHeadParms( 			// fetches header, checks for magic & size
						int emagic = EEP_MAGIC,
						int nuser = 0 );
						
					void initWiFiParms( 					// initializes memory WiFi parms
						char *myssid="kontopidis2GHz", 		// if "" deletes value. If NULL, leaves unmodified
						char *mypwd="123456789a", 			// Saves into EEPROM
						char *staticIP="", 
						int myport=80 );    
						
					void incrBootCount();					// Changes the boot count and updates the EEPROM
						
					void saveHeadParms();			// saves memory structures to eeprom
					void saveWiFiParms();			// saves memory structures to eeprom
					
					void saveUserStruct( byte *pntr, int actsiz );			// saves memory structures to eeprom
					int fetchUserStruct( byte *pntr, int maxsiz );
						
					int fetchHeadParms();			// fetches to memory; assumes consistent structures. Returns error code
					int fetchWiFiParms();
					
					// void updateWiFiParms( char *myssid=NULL, char *mypwd=NULL, char *staticIP=NULL, int myport=0 );    // initializes memory WiFi parms
					// String getParmString( char *prompt, select_t select=ALL_PARMS );          // fetch and print User parms
					
					String getHeadString();
					String getWiFiString();
					
					void printHeadParms( char *prompt="" );			// prints the structure data. Does not fetch()
					void printWiFiParms( char *prompt="" );
					
					int notify( char *s );
				};
#endif

		#ifdef IGNORE
		class EEP1
		{
		private:
			void (*callbk)();       		// callback to initialize user parameters

			char *name[MAX_USERPARMS];		// userParm name
			byte type [MAX_USERPARMS];		// userParm 'f', 'd' or 's'
			void *pntr[MAX_USERPARMS];		// pointer to user value of this parm
			uint16_t nparms;						// number of saved parms

		public:	
			uint16_t userbcount;				// number of user parameters saved (in bytes)
			
									// ACCESSIBLE HEAD STRUCTURE
			struct head_t
			{
				uint16_t magic;
				uint16_t headN;    			// byte count of this HEAD
				uint16_t wifiN;    			// byte count of this WIFI
				uint16_t reboots;  			// increments avery time reboots
				
			} head, head0;         			// 'head0' is used by fetch(). If OK, copies to 'head'
			
			struct wifi_t					// universal structure for wifi parameters
			{
				char ssid[16];
				char pwd [16];
				char stIP[16];
				uint16_t port;
			
			} wifi;

			EEP();
											// OPTIONAL USER PARM REGISTRATION
			void resetUserStruct();
			void registerUserParm( const char *pname, char ptype, void *ppntr );
			int  setUserParmValue( const char *pname, void *ppntr );
			int  getUserParmType ( const char *pname );
			void registerUserInit( void (*p)() );

			void initHeadStruct();			// Initializes HEAD. Forces MAGIC and BOOTCOUNT to zero.
			
			void initWiFiStruct( 					// initializes memory WiFi parms
				char *myssid="kontopidis2GHz", 		// if "" deletes value. If NULL, leaves unmodified
				char *mypwd="123456789a", 			// Does not write then into EEPROM
				char *staticIP="", 
				int myport=80 );    
				
			void initUserStruct();			// Calls registed user function to change the values of user defaults

			void updateRebootCount();		// Changes the boot count and updates the EEPROM
			
			void saveHeadParms();			// saves memory structures to eeprom
			void saveWiFiParms();			// saves memory structures to eeprom
			void saveUserParms();			// saves memory structures to eeprom
			
			int fetchHeadParms();			// fetches to memory; assumes consistent structures. Returns error code
			int fetchWiFiParms();
			int fetchUserParms();
			
			// void updateWiFiParms( char *myssid=NULL, char *mypwd=NULL, char *staticIP=NULL, int myport=0 );    // initializes memory WiFi parms
			// String getParmString( char *prompt, select_t select=ALL_PARMS );          // fetch and print User parms
			
			String getHeadString();
			String getWiFiString();
			String getUserString();
			
			void printHeadStruct( char *prompt="" );			// prints the structure data. Does not fetch()
			void printWiFiStruct( char *prompt="" );
			void printUserStruct( char *prompt="" );
			
			//void printUserParms();
		};
		#endif


#endif
