#include "timeClass.h"
#include "bufClass.h"

static char *weekdstr[]={"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static int days_in_month[]={0,31,28,31,30,31,30,31,31,30,31,30,31};
static char *monthstr[]={ " ",   "Jan",  "Feb",  "Mar",  "Apr",  "May",  "Jun",
                                 "Jul",  "Aug",  "Sep",  "Oct",  "Nov",  "Dec" };

//--------------------------------- PUBLIC ROUTINES -----------------------------------

int TIME::init(  int sperm, int mperh, int hperd )
{
  seconds_per_minute = sperm;
  minutes_per_hour = mperh;
  hours_per_day = hperd;
  
  second_tics = 0;
  hour_tics = 0;
  for( int i=0; i<EVENT_END; i++ )
  {
  	event[i] = false;
  	//stamp[i] = 0;
  }
//  stamp[ DAY_BEGIN ] = stamp[ DAY_END ] = 1;
//  stamp[ MONTH_BEGIN ] = 1;
} 
/*
 * ready[0] = true every sec
 * ready[1] = every min
 * 
 * enumerate 
 * MINUTE_END 59th second, just before 00
 * MINUTE_BEGIN just turned 00
 * user code:
 * if( tm.ready( MIN59, MIN00AUTO_RESET, MIN ) )
 *    to something;
 */
void TIME::update()      // called by the Ticker() every second
{
  second_tics++;
  seconds++;
  
  event[ SECOND_BEGIN ]=true;
  //stamp[ SECOND_BEGIN ]=seconds;
  
  if( seconds >=seconds_per_minute )
  {
    //event[ MINUTE_END ]=true;
    //stamp[ MINUTE_END ]=minutes;
    minutes++;
    seconds = 0;
    event[ MINUTE_BEGIN ]=true;
    //stamp[ MINUTE_BEGIN ]=minutes;
  }
  if( minutes >=minutes_per_hour )
  {
    //event[ HOUR_END ]=true;
    //stamp[ HOUR_END ]=hours;
    hours++;
    minutes = 0;
	hour_tics++;
    event[ HOUR_BEGIN ]=true;    
    //stamp[ HOUR_BEGIN ]=hours;
  }
  if( hours >= hours_per_day )
  {
    //event[ DAY_END ]=true;     // new day 00:00
    //stamp[ DAY_END ]=mday;
    days++;
    mday++;
    if( mday > daysInMonth( year, month ) )
    {
      mday=1;
      month++;
      if( month>12 )
      {
        month = 1;
        year++;
      }
    }
    wday = (wday+1) % 7;
    hours = 0; 
	second_tics = 0;	

    event[ DAY_BEGIN ]=true;     // new day 00:00
    //stamp[ DAY_BEGIN ]=mday;
    if( wday == 0 )              // if Sunday 00:00
      event[ WEEK_BEGIN ] = true;
    if( mday == 1 )              // if 1st day of the month 00:00
      event[ MONTH_BEGIN ] = true;
  }
}
bool TIME::ready( event_t m, bool freset )
{
  bool flag;
  flag = event[ m ];
  if( freset )
    event[m] = false;
  return flag;
}
/*
int TIME::getEventTime( event_t m )
{
	return (int) stamp[m];	
}
unsigned long TIME::secondTics()
{
  return second_tics;
}
*/
void TIME::reset( event_t m )
{
  event[m] = false;
}
void TIME::setTime( int hrs, int mn, int sc )
{
  seconds = sc  % seconds_per_minute;
  minutes = mn  % minutes_per_hour;
  hours   = hrs % hours_per_day;
  second_tics = seconds;
  hour_tics = hours;
} 
void TIME::setDate( int ye, int mo, int md )
{
  if( md )        // do not change year/month/day if zero or not provided
    mday = md;
  if( mo )
    month = mo;
  if( ye )
    year = ye;
  
  // compute week day
  int sum = 0;
  for ( int i=1; i < month; i++ )
    sum += daysInMonth( year, i);         // this compensates for leap year
  sum += mday-1;                          // sum is now number of days since Jan 1st
  int daycode = findDayCode( year );     // this is the first wday of the year
  wday = (sum+daycode)%7;
//  prn("daycode=%d [%s], wday=%d [%s]\r\n", 
//      daycode, weekdstr[daycode],wday, weekdstr[wday] );
} 
int TIME::getTime( mask_t type )
{
  switch( type )
  {
    default:
    case SECOND_MASK: return seconds;
    case MINUTE_MASK: return minutes;
    case HOUR_MASK:   return hours;
    case WDAY_MASK:   return wday;
    case DAY_MASK:    return mday;

    case MONTH_MASK:  return month;    
    case YEAR_MASK:   return year;
  }
}
String TIME::getTimeString( void )
{
  //String s("",40);
  B64 s;
  s.set( "%02d:%02d:%02d", hours, minutes, seconds );
  return String( !s );
}
String TIME::getDateString( void )
{
  //String s("",40);
  B64 s;
  s.set( "%02d/%02d/%02d", month, mday, year );
  return String(!s);
}
String TIME::getDateVerbose( void )
{
  B64 s;  
  s.set( "%s %s %02d, %4d", 
    weekdstr[ wday >6  ?6 :wday  ],
    monthstr[ month>12 ?12:month ],
    mday,
    year );
  return String (!s );
}

//--------------------------------- PRIVATE ROUTINES -----------------------------------

bool TIME::leapYear( int y )
{
  return ( (y%4 == false) && ((y%100 != false) || (y%400 == false)) );
}
int TIME::daysInMonth( int y, int i )
{
  
  if( (i<1) || (i>12) )
    return 0;
  if( (i==2) && leapYear( y ) )
    return days_in_month[2]+1;
  return days_in_month[i];
}
int TIME::findDayCode(int year1)
{
  int daycode;
  int d1, d2, d3;
  
  d1 = (year1 - 1.)/ 4.0;
  d2 = (year1 - 1.)/ 100.;
  d3 = (year1 - 1.)/ 400.;
  daycode = (year1 + d1 - d2 + d3) %7;
  return daycode;
}
/*
char * TIME::getWeekDay()
{
  return weekdstr[ wday % 7 ];
}
int TIME::toWeekDay( String s )
{
  for(int i=0; i<7; i++ )
    if( s.equalsIgnoreCase( weekdstr[i] ) )
      return i;
}
*/
// ------------------------------ ModuloSec Class ------------------------------------------
static bool moduloHelper( uint32_t sec, int timemod, int timeoff, uint32_t *timeprev )
{
    if( (sec % timemod) == timeoff )                   // modulo triggered
    {
        if( *timeprev != sec )
        {
            *timeprev = sec;
            return true;
        }
        else                // this trigger has already happen
            return false;
    }
    else
    {
        *timeprev = 0;
        return false;
    }
}
ModuloTic::ModuloTic( int interval )
{
  modulo = interval;
  prevret = 0;
}
bool ModuloTic::ready()
{
    return moduloHelper( millis()/1000, modulo, 0, &prevret );
}
// ------------------------------ ModuloSec Class ------------------------------------------
ModuloSec::ModuloSec( TIME &t )
{
  modulo = 0;
  prevret = 0;
  timp = &t.second_tics;                                    // pointer to tm.seconds
}
bool ModuloSec::ready( int modarg, int offset )
{
	uint32_t sec = *timp;		        	                // use the seconds tic from timer
	return moduloHelper( sec, modarg, offset, &prevret );
}
// ------------------------------ ModuloHrs Class ------------------------------------------
ModuloHrs::ModuloHrs( TIME &t )
{
  modulo = 0;
  prevret = 0;
  timp = &t.hour_tics;                                      // pointer to tm.seconds
}
bool ModuloHrs::ready( int modarg, int offset )
{
    uint32_t hrs = *timp;                                   // use the seconds tic from timer
    return moduloHelper( hrs, modarg, offset, &prevret );
}

// ------------------------------ Tic Class ------------------------------------------
// ----------------------------- TO BE OBSOLETED -------------------------------------
Tic::Tic( uint32_t delayms )
{
  Tdelta = delayms;
  active = false;
}
#define TMAX 0xFFFFFFFF

void Tic::start( uint32_t delayms )
{
  	uint32_t now;
	if( delayms == 0 )				// backwards compatibility
		delayms = Tdelta;
	
	Tdelta = delayms;				// save this for Retrigger
	now = millis();
	if( (now+2*delayms) < TMAX )	// even if we check from delayms..to...2xdelayms later, we will not overlfow
		Ttarget = now+delayms;
	else							// millis() WILL wrap around. 
		Ttarget = delayms;			// so we might be off by 3*delayms worst case, until it becomes  true
		
   active = true;
}
bool Tic::ready( trigger_t retrigger )
{
	uint32_t now = millis();
	if( !active )
		return false;
	
	// if active
	if( now >= Ttarget )
	{
		if( retrigger == TIC_RETRIGGER )
		  	start( Tdelta );
		else
		  	active = false;
		return true;
	}
	return false;
}
// -------------------------------------
TmCount::TmCount()
{
  tstart = millis();
  tdelta = 0;
  tmax = 0;
}
void TmCount::startT()
{
  tstart = millis();
}
void TmCount::stopT()
{
  tdelta = millis()-tstart;
  if( tdelta > 32000 )    // bogus
    tdelta = 0;
  if( tdelta > tmax )
    tmax = tdelta;
}
uint32_t TmCount::deltaMax()
{
  return tmax;
}
void TmCount::resetMax()
{
  tmax = 0;
}

uint32_t TmCount::deltaT()
{
  return tdelta;
}
