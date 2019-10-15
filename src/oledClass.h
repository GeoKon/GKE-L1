/*--------------------------------------------------------------------
  06/01/2016, Author: Makerbro, Platforms: ESP8266, Language: C++
  https://acrobotic.com/
  
  09/10/2019, Re-written using the dsp() drivers by George Kontopidis
  
  License:
  Released under the MIT license. Please check LICENSE.txt for more
  information.  All text above must be included in any redistribution. 
  --------------------------------------------------------------------*/

#pragma once

#include "Arduino.h"
#include <pgmspace.h>
#define OLEDFONT(name) static const uint8_t name[]
#include "Wire.h"
#include "fonts/font8x8W.h"
// #include "fonts/font8x8.h"
// #include "fonts/font5x7.h"

#define SSD1306_Command_Mode          0x80
#define SSD1306_Data_Mode             0x40
#define SSD1306_Display_Off_Cmd       0xAE
#define SSD1306_Display_On_Cmd        0xAF
#define SSD1306_Normal_Display_Cmd    0xA6
#define SSD1306_Set_Brightness_Cmd    0x81

enum dsptype_t		// used by oled.init()
{
    SMALL = 0,
    OLED091 = 0,    // SSD1306 128x32 oled
    LARGE = 1,
    OLED096 = 1,    // SSD1306 128x64 oled
    BIG = 2,
    OLED130 = 2     // SH1106 128x64 oled
};
enum control_t		// used by oled.dsp()
{
	O_CLEAR   = -1,
	O_BRIGHT0 = -2,
	O_BRIGHT1 = -3,
	O_BRIGHT2 = -4,
	O_BRIGHT3 = -5,
	O_LED96   = -96,
	O_LED130  = -130
};
#define O_DOUBLE_LINE  1
#define O_DOUBLE_WIDTH 2
#define O_CENTERED 	   4
	
class OLED 
{
private:
    uint8_t _sda, _scl, _address, _offset;
	
	const uint8_t* m_font;      // Current font.
    uint8_t m_font_offset = 2;  // Font bytes for meta data.
    uint8_t m_font_width;       // Font witdth.
    bool  	m_size;		 	    // true=128x64, false=128x32
	char temp[20];				// string used for dsp() returns
    
    void initLarge();
	void initSmall();
    void setTextXY(unsigned char Row, unsigned char Column);
    bool putChar(unsigned char c);
	void sendCommand(unsigned char command);
    void sendData(unsigned char Data);
	
	const char *displayRow( int linecode, int X, const char *string);

public:
    OLED(uint8_t sda=4, uint8_t scl=5, uint8_t address=0x3c );
    void init( dsptype_t type, const uint8_t* font = font8x8W );

    void clearDisplay();
    void displayOn( bool c=true );
    void setBrightness(unsigned char Brightness);

	const char *dsp( control_t c );
	const char *dsp( int row, int col, const char *format, ... );	
	const char *dsp( int row, const char *format, ... );   
	
	void clearRow( int x );
};
