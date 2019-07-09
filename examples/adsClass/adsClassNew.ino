#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <cpuClass.h>

typedef enum 
{
	FS_6144mV = 6144,
	FS_4096mV = 4096,
	FS_2048mV = 2048,
	FS_1024mV = 1024,
	FS_512mV = 512,
	FS_256mV = 256
} fullscale_t;

class ADS : public Adafruit_ADS1015
{
public: 
	ADS(uint8_t i2cAddress)
	{
	   m_i2cAddress = i2cAddress;
	   m_conversionDelay = ADS1115_CONVERSIONDELAY;
	   m_bitShift = 0;
	   setFS( FS_4096mV ); // equivalent to GAIN_ONE 
	}	
	void setFS( fullscale_t f )
	{
		switch( f )
		{
			default:
			case FS_6144mV: m_gain = GAIN_TWOTHIRDS; break;
			case FS_4096mV: m_gain = GAIN_ONE;break;
			case FS_2048mV: m_gain = GAIN_TWO;break;
			case FS_1024mV: m_gain = GAIN_FOUR;	break;		
			case FS_512mV: m_gain = GAIN_EIGHT;break;
			case FS_256mV: m_gain = GAIN_SIXTEEN;break;
		}
	}
	float toVolts( int reading )
	{
		float frac = (float) reading / (float) 0x7FFF;
		switch( m_gain )
		{
			default:			return 2.048 * 3 * frac;
			case GAIN_ONE:  	return 4.096 * frac;
			case GAIN_TWO:  	return 2.048 * frac;
			case GAIN_FOUR:  	return 1.024 * frac;
			case GAIN_EIGHT: 	return 0.512 * frac;
			case GAIN_SIXTEEN: 	return 0.256 * frac;			
		}
	}
};
/*
 * ADDR to GND 0x48
 */
ADS ads( 0x48 );     
CPU cpu;

void setup(void) 
{
  	cpu.init();
  
  	PR("Getting single-ended readings from AIN0..3");
  	PR("ADC Full Scale = 4.096V (max allowed is 3.3V)");
    setFS( FS_4096mV ); 
    ads.begin();
}

void loop(void) 
{
  int16_t adc0, adc1, adc2, adc3;

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);
  //PF("%6d %6d %6d %6d\r\n", adc0, adc1, adc2, adc3);
  PF( "%6d %5.3fV\r\n", adc1, ads.toVolts( adc1 ) );
  delay(2000);
}
