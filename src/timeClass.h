#ifndef TIMECLASS_H
#define TIMECLASS_H

#include  <cpuClass.h>

// ---------------------------------- Class Time --------------------------------
enum event_t
{
  SECOND_BEGIN  = 0,
  MINUTE_BEGIN  = 1,
  HOUR_BEGIN    = 2,
  DAY_BEGIN     = 3,
  WEEK_BEGIN    = 4,
  MONTH_BEGIN   = 5,
  EVENT_END     = 6
  
  //MINUTE_END    = 2,
  //HOUR_END      = 4,
};
enum mask_t     // use to retrieve the time.
{
  SECOND_MASK = 0,
  MINUTE_MASK,
  HOUR_MASK,
  DAY_MASK,
  WDAY_MASK,
  MONTH_MASK,
  YEAR_MASK
};

class TIME
{
  private:
    int seconds_per_minute;     // set by Timer::init() used 
    int minutes_per_hour;
    int hours_per_day;

    int days;                   // increments every day forever
        
    int year;                   // current year
    int month;                  // current month 1...12
    int mday;                   // current day of the month 1...28[29][30][31]
    int wday;                   // day of the week 0=Sun...6=Sat
    int hours;                  // used by the setTime(), getTime()
    int minutes;
    int seconds;

    bool 	event[ EVENT_END ];
    //uint8_t stamp[ EVENT_END ];	// sec, min, or hr count for specific event
    
  public:
	uint32_t second_tics;  	    // updated by tm.update(). Reset every day at 00:00:00, or setTime(). max = 60*60*24
	uint32_t hour_tics;  		// updated by tm.update(). Reset every setTime()
									
    int     init(   int secpermin=60, int minperhr=60, int hrperday=24 );
            
    void    update();         // called by the Ticker() every second.

    bool    ready( event_t m, bool flag=true );
    void    reset( event_t m );
	
//  int		getEventTime( event_t m );

    void    setTime( int hrs=0, int mn=0, int sc=0 );
	void	setDate( int ye=0, int mo=0, int dy=0);
    
	int     getTime( mask_t m );

    String  getTimeString( void );
    String  getDateString( void );
    String  getDateVerbose( void );

// --------- used by the NTP only -----------------------------------------
    bool leapYear( int y );             // true or false if year is leap
    int daysInMonth( int y, int i );    // number of days in month i=1...12
    int findDayCode(int year1);         // day of the beginning of the year 0=Sunday
};
#ifndef tm_allocated
	extern TIME tm;
#endif

// ---------------------------------- Class ModuloSec -------------------------
/*      ModuloTic t( modsec );              // instantiate
 *      if( t.ready() )                     // trigger at (time % modsec)
 *      
 * Uses millis(). Does not require the timeClass() to be running
 * ----------------------------------------------------------------------------
 */
class ModuloTic
{
private:
    uint16_t modulo;
    uint32_t prevret;
public:
    ModuloTic( int seconds );
    bool ready();                   // true if mod!=0 OR mod
};

 // ---------------------------------- Class ModuloSec -------------------------
/*      ModuloSec t( tm );                  // instantiate
 *      if( t.ready( modsec, [offset] ) )   // trigger at (time % modsec)==offset
 *
 * Requires timer to be running.
 * Triggers only once, i.e. if called repeatetly in the same second, triggers
 * the first time and then returns false. 
 * ----------------------------------------------------------------------------
 */
class ModuloSec
{
private:
    uint16_t modulo;
    uint32_t prevret;
    uint32_t *timp;   // points to seconds from timeCalss
public:
    ModuloSec( TIME &t );
    bool ready( int mod, int offset=0 );		// true if mod!=0 OR mod
};

// ---------------------------------- Class ModuloSec -------------------------
/*      ModuloHrs t( tm );                  // instantiate
 *      if( t.ready( modsec, [offset] ) )   // trigger at (time % modsec)==offset
 * ----------------------------------------------------------------------------
 */
class ModuloHrs
{
private:
    uint16_t modulo;
    uint32_t prevret;
    uint32_t *timp;   // points to hours from timeClass
public:
    ModuloHrs( TIME &t );
    bool ready( int mod, int offset=0 );        // true if mod!=0 OR mod
};
// ---------------------------------- Class Tic --------------------------------
// ------------------------------ TO BE OBSOLETED ------------------------------
enum trigger_t
{
  TIC_ONCE = false,
  TIC_RETRIGGER = true
};
class Tic
{
  private:
    uint32_t Ttarget;
    uint32_t Tdelta;
    bool active;
  public:
    Tic( uint32_t delayms = 0 );			// one of them must be set to non-zero
    void start( uint32_t delayms = 0 );
    bool ready( trigger_t flag = TIC_ONCE );
};
// ---------------------------------- Class TmCount --------------------------------

class TmCount
{
  private:
    uint32_t tstart;
    uint32_t tdelta;
    uint32_t tmax;
    
  public:
    TmCount();
    void startT();
    void stopT();
    uint32_t deltaT();
    
    uint32_t deltaMax();  
    void resetMax(); 
};
#endif
