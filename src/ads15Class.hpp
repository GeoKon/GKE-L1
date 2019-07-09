#pragma once

#include "Arduino.h"
#include <Wire.h>

//    I2C ADDRESS/BITS
    #define ADS1115_ADDRESS                 (0x48)    // 1001 000 (ADDR = GND)

//    CONVERSION DELAY (in mS)
    #define ADS1115_CONVERSIONDELAY         (1L)
    #define ADS1115_CONVERSIONDELAY         (8L)

//    POINTER REGISTER
    #define ADS1115_REG_POINTER_MASK        (0x03)
    #define ADS1115_REG_POINTER_CONVERT     (0x00)
    #define ADS1115_REG_POINTER_CONFIG      (0x01)
    #define ADS1115_REG_POINTER_LOWTHRESH   (0x02)
    #define ADS1115_REG_POINTER_HITHRESH    (0x03)

//    CONFIG REGISTER
    #define ADS1115_REG_CONFIG_OS_MASK      (0x8000)
    #define ADS1115_REG_CONFIG_OS_SINGLE    (0x8000)  // Write: Set to start a single-conversion
    #define ADS1115_REG_CONFIG_OS_BUSY      (0x0000)  // Read: Bit = 0 when conversion is in progress
    #define ADS1115_REG_CONFIG_OS_NOTBUSY   (0x8000)  // Read: Bit = 1 when device is not performing a conversion

    #define ADS1115_REG_CONFIG_MUX_MASK     (0x7000)
    #define ADS1115_REG_CONFIG_MUX_DIFF_0_1 (0x0000)  // Differential P = AIN0, N = AIN1 (default)
    #define ADS1115_REG_CONFIG_MUX_DIFF_0_3 (0x1000)  // Differential P = AIN0, N = AIN3
    #define ADS1115_REG_CONFIG_MUX_DIFF_1_3 (0x2000)  // Differential P = AIN1, N = AIN3
    #define ADS1115_REG_CONFIG_MUX_DIFF_2_3 (0x3000)  // Differential P = AIN2, N = AIN3
    #define ADS1115_REG_CONFIG_MUX_SINGLE_0 (0x4000)  // Single-ended AIN0
    #define ADS1115_REG_CONFIG_MUX_SINGLE_1 (0x5000)  // Single-ended AIN1
    #define ADS1115_REG_CONFIG_MUX_SINGLE_2 (0x6000)  // Single-ended AIN2
    #define ADS1115_REG_CONFIG_MUX_SINGLE_3 (0x7000)  // Single-ended AIN3

    #define ADS1115_REG_CONFIG_PGA_MASK     (0x0E00)
    #define ADS1115_REG_CONFIG_PGA_6_144V   (0x0000)  // +/-6.144V range = Gain 2/3
    #define ADS1115_REG_CONFIG_PGA_4_096V   (0x0200)  // +/-4.096V range = Gain 1
    #define ADS1115_REG_CONFIG_PGA_2_048V   (0x0400)  // +/-2.048V range = Gain 2 (default)
    #define ADS1115_REG_CONFIG_PGA_1_024V   (0x0600)  // +/-1.024V range = Gain 4
    #define ADS1115_REG_CONFIG_PGA_0_512V   (0x0800)  // +/-0.512V range = Gain 8
    #define ADS1115_REG_CONFIG_PGA_0_256V   (0x0A00)  // +/-0.256V range = Gain 16

    #define ADS1115_REG_CONFIG_MODE_MASK    (0x0100)
    #define ADS1115_REG_CONFIG_MODE_CONTIN  (0x0000)  // Continuous conversion mode
    #define ADS1115_REG_CONFIG_MODE_SINGLE  (0x0100)  // Power-down single-shot mode (default)

    #define ADS1115_REG_CONFIG_DR_MASK      (0x00E0)  
    #define ADS1115_REG_CONFIG_DR_128SPS    (0x0000)  // 128 samples per second
    #define ADS1115_REG_CONFIG_DR_250SPS    (0x0020)  // 250 samples per second
    #define ADS1115_REG_CONFIG_DR_490SPS    (0x0040)  // 490 samples per second
    #define ADS1115_REG_CONFIG_DR_920SPS    (0x0060)  // 920 samples per second
    #define ADS1115_REG_CONFIG_DR_1600SPS   (0x0080)  // 1600 samples per second (default)
    #define ADS1115_REG_CONFIG_DR_2400SPS   (0x00A0)  // 2400 samples per second
    #define ADS1115_REG_CONFIG_DR_3300SPS   (0x00C0)  // 3300 samples per second

    #define ADS1115_REG_CONFIG_CMODE_MASK   (0x0010)
    #define ADS1115_REG_CONFIG_CMODE_TRAD   (0x0000)  // Traditional comparator with hysteresis (default)
    #define ADS1115_REG_CONFIG_CMODE_WINDOW (0x0010)  // Window comparator

    #define ADS1115_REG_CONFIG_CPOL_MASK    (0x0008)
    #define ADS1115_REG_CONFIG_CPOL_ACTVLOW (0x0000)  // ALERT/RDY pin is low when active (default)
    #define ADS1115_REG_CONFIG_CPOL_ACTVHI  (0x0008)  // ALERT/RDY pin is high when active

    #define ADS1115_REG_CONFIG_CLAT_MASK    (0x0004)  // Determines if ALERT/RDY pin latches once asserted
    #define ADS1115_REG_CONFIG_CLAT_NONLAT  (0x0000)  // Non-latching comparator (default)
    #define ADS1115_REG_CONFIG_CLAT_LATCH   (0x0004)  // Latching comparator

    #define ADS1115_REG_CONFIG_CQUE_MASK    (0x0003)
    #define ADS1115_REG_CONFIG_CQUE_1CONV   (0x0000)  // Assert ALERT/RDY after one conversions
    #define ADS1115_REG_CONFIG_CQUE_2CONV   (0x0001)  // Assert ALERT/RDY after two conversions
    #define ADS1115_REG_CONFIG_CQUE_4CONV   (0x0002)  // Assert ALERT/RDY after four conversions
    #define ADS1115_REG_CONFIG_CQUE_NONE    (0x0003)  // Disable the comparator and put ALERT/RDY in high state (default)

typedef enum
{
  GAIN_TWOTHIRDS    = ADS1115_REG_CONFIG_PGA_6_144V,
  GAIN_ONE          = ADS1115_REG_CONFIG_PGA_4_096V,
  GAIN_TWO          = ADS1115_REG_CONFIG_PGA_2_048V,
  GAIN_FOUR         = ADS1115_REG_CONFIG_PGA_1_024V,
  GAIN_EIGHT        = ADS1115_REG_CONFIG_PGA_0_512V,
  GAIN_SIXTEEN      = ADS1115_REG_CONFIG_PGA_0_256V
} adsGain_t;

typedef enum 
{
	FS_6144mV = 6144,
	FS_4096mV = 4096,
	FS_2048mV = 2048,
	FS_1024mV = 1024,
	FS_512mV = 512,
	FS_256mV = 256
} fullscale_t;

#define i2cwrite(A) Wire.write(A)
#define i2cread     Wire.read

class ADS15
{
protected:
    // Instance-specific properties
    uint8_t   	my_i2cAddress;
    int16_t 	my_config[6];		// configuration value per channel
    uint32_t 	my_tstart;			// millis() at the beginning of the conversion
    
    // Writes 16-bits to the specified destination register
    static void writeRegister(uint8_t i2cAddress, uint8_t reg, uint16_t value) 
    {
        Wire.beginTransmission(i2cAddress);
        i2cwrite((uint8_t)reg);
        i2cwrite((uint8_t)(value>>8));
        i2cwrite((uint8_t)(value & 0xFF));
        Wire.endTransmission();
    }
    
    // Writes 16-bits to the specified destination register
    static uint16_t readRegister(uint8_t i2cAddress, uint8_t reg) 
    {
        Wire.beginTransmission(i2cAddress);
        i2cwrite(ADS1115_REG_POINTER_CONVERT);
        Wire.endTransmission();
        Wire.requestFrom(i2cAddress, (uint8_t)2);
        return ((i2cread() << 8) | i2cread());  
    }
 	adsGain_t setFS( fullscale_t f )
    {
        switch( f )
        {
            default:
            case FS_6144mV: return GAIN_TWOTHIRDS; 
            case FS_4096mV: return GAIN_ONE;
            case FS_2048mV: return GAIN_TWO;
            case FS_1024mV: return GAIN_FOUR;
            case FS_512mV: return GAIN_EIGHT;
            case FS_256mV: return GAIN_SIXTEEN;
        }
    }
	
public:
    ADS15( int i2cAddress = 0x48 ) // ADDR pin to GND
    {
        my_i2cAddress = i2cAddress;
    }
	void begin() 
	{
		Wire.begin();
	}
    void initChannel( int channel, fullscale_t f ) 
    {
        // Start with default values
        uint16_t config = ADS1115_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
                        ADS1115_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
                        ADS1115_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                        ADS1115_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                        ADS1115_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
                        ADS1115_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)
    
        config |= setFS( f );         // Set PGA/voltage range

        // Set single-ended input channel
        switch (channel)
        {
            case (0): 	config |= ADS1115_REG_CONFIG_MUX_SINGLE_0;  break;
            case (1): 	config |= ADS1115_REG_CONFIG_MUX_SINGLE_1;  break;
            case (2): 	config |= ADS1115_REG_CONFIG_MUX_SINGLE_2;  break;
            case (3): 	config |= ADS1115_REG_CONFIG_MUX_SINGLE_3;  break;
			case (4): 	config |= ADS1115_REG_CONFIG_MUX_DIFF_0_1;  break; // AIN0 = P, AIN1 = N
			case (5): 	config |= ADS1115_REG_CONFIG_MUX_DIFF_2_3;  break; // AIN2 = P, AIN3 = N
        }    
        // Set 'start single-conversion' bit
        config |= ADS1115_REG_CONFIG_OS_SINGLE;
        my_config[ channel ] = config;
    }

    void startConversion( int channel )
    {
        // Write config register to the ADC
        writeRegister(my_i2cAddress, ADS1115_REG_POINTER_CONFIG, my_config[ channel ] );
        my_tstart = millis();
    }
    int16_t readConversion()
    {
        if( millis() < (my_tstart + ADS1115_CONVERSIONDELAY) ) // Wait for the conversion to complete
            delay( ADS1115_CONVERSIONDELAY );
        return readRegister(my_i2cAddress, ADS1115_REG_POINTER_CONVERT);
    }
	float toVolts( int channel, int reading  )
    {
        float frac = (float) reading / (float) 0x7FFF;
        float volts;
		
		int gain = my_config[ channel ] & ADS1115_REG_CONFIG_PGA_MASK;	// get the gain for this channel

        switch( (adsGain_t) gain )
        {
            default:            volts = 2.048 * 3 * frac; break;
            case GAIN_ONE:      volts = 4.096 * frac; break;
            case GAIN_TWO:      volts = 2.048 * frac; break;
            case GAIN_FOUR:     volts = 1.024 * frac; break;
            case GAIN_EIGHT:    volts = 0.512 * frac; break;
            case GAIN_SIXTEEN:  volts = 0.256 * frac; break;            
        }
        return volts;
    } 
	int16_t toADC( int channel, float volts  )
    {
        float frac;
		
		int gain = my_config[ channel ] & ADS1115_REG_CONFIG_PGA_MASK;	// get the gain for this channel

        switch( (adsGain_t) gain )
        {
            default:            frac = volts/(2.048 * 3.0); break;
            case GAIN_ONE:      frac = volts/4.096; break;
            case GAIN_TWO:      frac = volts/2.048; break;
            case GAIN_FOUR:     frac = volts/1.024; break;
            case GAIN_EIGHT:    frac = volts/0.512; break;
            case GAIN_SIXTEEN:  frac = volts/0.256; break;            
        }
		if( frac >= 1.00 )
			frac = 0.99996; // this is the 1-1/0x7FFFF
		if( frac <= -1.00 )
			frac = -0.99996;
		return (int16_t) (frac * 32767.0);
    } 	
};	