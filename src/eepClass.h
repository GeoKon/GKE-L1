#ifndef EEPCLASS_H
#define EEPCLASS_H
// See eepClass.md for use

/* ---------------------------------------------------------------------------------                      
 *    HOW TO DO...                   
 * 
 * To modify WiFi parameters:   updateWiFiParms()
 * To modify a user parameter:  userstruct.X = new; eep.saveParms( USER_PARMS );
 * To increment Reboot:         updateReboot()
 
 * Main Code   
 *                              class structure with parameters
 *                              class initialization myInit()
 *                              
 *                              main()
 *                                  eep.registerUserParms( structure, size format )
 *                                  eep.registerInitCallbk( func );
 *                                  bool ok = eep.fetchParms();
 *                                  if( !ok )
 *                                  {
 *                                      eep.initWiFiParms(...)
 *                                      eep.initUserParms()
 *                                      eep.saveParms();
 *                                  }
 *                                  eep.updateRebootCount();
 *                                  PR( eep.getParms("prompt") );
 *                                  
 *                       Access EEPROM       Modifies Struct
 * saveParms( se] )         yes                  no
 * fetchParms( sel )        yes                  yes
 * 
 * getParms( sel )          no                   no
 * initWiFiParms(...)       no                   yes 
 * initUserParms()          no                   yes
 * 
 * registerUserParms()      no                   no
 * registerInitCallBk()     no                   no
 *   
 * updateWiFi()    mod&save yes                 yes    
 * updateReboot()  mod&save yes                 yes
 *  
 * formatUserParms(...)     no                  no
 * --------------------------------------------------------------------------------------
 */

#include <EEPROM.h>
#include <cpuClass.h>

#define EEP_MAGIC ('K'*256+'O')

#define ALLOCSTR 200        // number of String size used by getXX() routines

enum select_t               // used in saveParms(), getParms(), fetchParms()
{
    HEAD_PARMS = 1,
    WIFI_PARMS = 2,
    USER_PARMS = 4,
     ALL_PARMS = 7        
};
    
class EEP
{
private:
    byte *userp;            // pointer to user parms and format
    char *userf;
    void (*callbk)();       // callback to initialize parameters

public:
    struct head_t
    {
        uint16_t magic;
        uint16_t wifiN;    // number of WiFi parameters 
        uint16_t userN;    // number of user parameters saved (in bytes)
        uint16_t reboots;  // increments avery time reboots
        
    } head, head0;         // head0 is used by fetch()
    
    #define HEAD_SIZE 8            // sizeof( head_t ) causes an error in eepTables()
    #define USER_SIZE head.userN

    struct wifi_t
    {
        
        char ssid[16];
        char pwd [16];
        char stIP[16];
        uint16_t port;

    } wifi;
    #define WIFI_SIZE 3*16+2   // sizeof( wifi_t )

    EEP();
    void registerUserParms( void *p, int N, char *format );
    void registerInitCallBk( void (*p)() );
    void saveParms( select_t select=ALL_PARMS );     // saves memory structures to eeprom
    int fetchParms( select_t select1=ALL_PARMS );    // fetches to memory; assumes consistent structures
     
    // if "" deletes value. If NULL, leaves unmodified
    void initWiFiParms( char *myssid="kontopidis2GHz", 
                        char *mypwd="123456789a", 
                        char *staticIP="", 
                        int myport=80 );    // initializes memory WiFi parms
    void initUserParms();
    void updateWiFiParms( char *myssid=NULL, char *mypwd=NULL, char *staticIP=NULL, int myport=0 );    // initializes memory WiFi parms
    void updateRebootCount();
    String getParmString( char *prompt, select_t select=ALL_PARMS );          // fetch and print User parms
    void printParms(  char *prompt, select_t select=ALL_PARMS ); 
     // GENERIC -- does not use class members
    char *formatUserParm( char *format, void *up, char *temp, int tN );
 
};

extern EEP eep;

#endif
