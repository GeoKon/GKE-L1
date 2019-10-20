 /*
  06/01/2016
  Author: Makerbro
  Platforms: ESP8266
  Language: C++
  File: SSD1306.cpp
  ------------------------------------------------------------------------
  Description: 
  SSD1306 OLED Driver Library.
  ------------------------------------------------------------------------
  Please consider buying products from ACROBOTIC to help fund future
  Open-Source projects like this! We'll always put our best effort in every
  project, and release all our design files and code for you to use. 
  https://acrobotic.com/
  ------------------------------------------------------------------------
  License:
  Released under the MIT license. Please check LICENSE.txt for more
  information.  All text above must be included in any redistribution. 
*/

// GKE: NO MODIFICATIONS TO THIS FILE

#include "oledClass.h"

OLED::OLED(uint8_t sda, uint8_t scl, uint8_t address ) 
{
	_sda = sda;
	_scl = scl;
	_address = address;
	_offset = 0;            // set by the init()
}
#define sendcommand sendCommand
void OLED::init( dsptype_t tp, const uint8_t* font )
{
	Wire.begin(_sda, _scl);
  	Wire.setClock(700000);
	if( tp==OLED096 ) 
    {
        _offset = 0; initLarge();
    }
	else if( tp==OLED091 )
	{
	    _offset = 0; initSmall();
	}
    else
    {
        _offset = 2; initLarge();
    }    

    m_font = font;				// select font
  	m_font_width = pgm_read_byte(&m_font[0]);
  	m_size = tp;
    clearDisplay();
}
void OLED::initLarge()
{
	sendCommand(0xAE);            //display off
	sendCommand(0xA6);            //Set Normal Display (default)
	sendCommand(0xAE);            //DISPLAYOFF
	
	sendCommand(0xD5);            //SETDISPLAYCLOCKDIV
	sendCommand(0x80);            // the suggested ratio 0x80
	
	sendCommand(0xA8);            //SSD1306_SETMULTIPLEX
	sendCommand(0x3F);
	//sendCommand(0x1F);
	sendCommand(0xD3);            //SETDISPLAYOFFSET
	sendCommand(0x0);             //no offset
	
	sendCommand(0x40|0x0);        //SETSTARTLINE
	sendCommand(0x8D);            //CHARGEPUMP
	sendCommand(0x14);
	
	sendCommand(0x20);            //MEMORYMODE
	sendCommand(0x00);            //0x0 act like ks0108
	// sendCommand(0x02);            //0x0 act like ks0108 ------------------------------
	sendCommand(0xA1);            //SEGREMAP   Mirror screen horizontally (A0)
	sendCommand(0xC8);            //COMSCANDEC Rotate screen vertically (C0)

	sendCommand(0xDA);            //0xDA
	sendCommand(0x12);            //COMSCANDEC
	//sendcommand(0x02);          // com pin HW config, sequential com pin config (bit 4), disable left/right remap (bit 5)
	
	sendCommand(0x81);            //SETCONTRAST
	sendCommand(0xCF);            //
	
	sendCommand(0xd9);            //SETPRECHARGE 
	sendCommand(0xF1); 
	sendCommand(0xDB);            //SETVCOMDETECT                
	sendCommand(0x40);

	sendCommand(0x2e);          	// Disable scroll
	sendCommand(0xA4);            //DISPLAYALLON_RESUME        
	sendCommand(0xA6);            //NORMALDISPLAY             

	sendCommand(0x20);            //Set Memory Addressing Mode
	sendCommand(0x00);            //Set Memory Addressing Mode ab Horizontal addressing mode
}
void OLED::initSmall()
{
    sendcommand(0xae);		      // display off
  	sendcommand(0xa6);          // Set Normal Display (default)

    sendcommand(0xAE);        	// DISPLAYOFF
    sendcommand(0xD5);        	// SETDISPLAYCLOCKDIV
    sendcommand(0x80);        	// the suggested ratio 0x80
    sendcommand(0xA8);        	// SSD1306_SETMULTIPLEX
    sendcommand(0x1F);
    sendcommand(0xD3);        	// SETDISPLAYOFFSET
    sendcommand(0x00);         	// no offset
    sendcommand(0x40 | 0x0);  	// SETSTARTLINE
    sendcommand(0x8D);        	// CHARGEPUMP
    sendcommand(0x14);          // 0x014 enable, 0x010 disable

    sendcommand(0x20);          // com pin HW config, sequential com pin config (bit 4), disable left/right remap (bit 5),
    sendcommand(0x02);          // 0x12 //128x32 OLED: 0x002,  128x32 OLED 0x012
    sendcommand(0xa1);          // segment remap a0/a1
    sendcommand(0xc8);          // c0: scan dir normal, c8: reverse
    
    sendcommand(0xda);
    sendcommand(0x02);          // com pin HW config, sequential com pin config (bit 4), disable left/right remap (bit 5)
    sendcommand(0x81);
    sendcommand(0xcf);          // [2] set contrast control
    sendcommand(0xd9);
    sendcommand(0xf1);          // [2] pre-charge period 0x022/f1
    sendcommand(0xdb);
    sendcommand(0x40);          // vcomh deselect level

    sendcommand(0x2e);          // Disable scroll
    sendcommand(0xa4);          // output ram to display
    sendcommand(0xa6);          // none inverted normal display mode
    sendcommand(0xaf);          // display on
}

void OLED::sendCommand(unsigned char command)
{
  Wire.beginTransmission(_address);    // begin I2C communication
  Wire.write(SSD1306_Command_Mode);           // Set OLED Command mode
  Wire.write(command);
  Wire.endTransmission();                       // End I2C communication
}

void OLED::setBrightness(unsigned char Brightness)
{
   sendCommand(SSD1306_Set_Brightness_Cmd);
   sendCommand(Brightness);
}

void OLED::setTextXY(unsigned char row, unsigned char col)
{
    sendCommand(0xB0 + row);                          //set page address
    sendCommand(0x00 + _offset + (m_font_width*col & 0x0F));    //set column lower addr ---------------
    sendCommand(0x10 + ((m_font_width*col>>4)&0x0F)); //set column higher addr
}

void OLED::displayOn( bool onoff )
{
	if( onoff )
		sendcommand(SSD1306_Display_On_Cmd);        //display on
	else
		sendcommand(SSD1306_Display_Off_Cmd);		//display off	
}
void OLED::clearDisplay()
{
  unsigned char i,j;
  sendCommand(SSD1306_Display_Off_Cmd);     //display off
  for(j=0;j<8;j++)
  {    
    setTextXY(j,0);    
    {
      for(i=0;i<16;i++)  //clear all columns
      {
        putChar(' ');    
      }
    }
  }
  sendCommand(SSD1306_Display_On_Cmd);     //display on
  setTextXY(0,0);    
}

void OLED::sendData(unsigned char Data)
{
     Wire.beginTransmission(_address); // begin I2C transmission
     Wire.write(SSD1306_Data_Mode);            // data mode
     Wire.write(Data);
     Wire.endTransmission();                    // stop I2C transmission
}

bool OLED::putChar(unsigned char ch)
{
    if (!m_font) return 0;
    //Ignore non-printable ASCII characters. This can be modified for
    //multilingual font.  
    if(ch < 32 || ch > 127) 
    {
        ch = ' ';
    }    
    for(unsigned char i=0;i<m_font_width;i++)
    {
       // Font array starts at 0, ASCII starts at 32
       sendData(pgm_read_byte(&m_font[(ch-32)*m_font_width+m_font_offset+i])); 
    }
    return 1;
}

static char b21( char c )
{
  char d = 0;
  
  if( c&1)
    d |= 0b00000011;
  if( c&2 )
    d |= 0b00001100;
  if( c&4 )
    d |= 0b00110000;
  if( c&8 )
    d |= 0b11000000;
  return d;
}

void OLED::clearRow( int X )
{
	setTextXY( X, 0 );
	for( int i=0; i<16; i++ )
    {	    
		for(int j=0;j<m_font_width;j++) 	//m_font_width
			sendData( 0 );
	}
} 

const char * OLED::displayRow( int linecode, int X, const char *string )
{
    const char *sp;					
	char data;
	char newlin[ 20 ];
	
	bool doubleline  = linecode & O_DOUBLE_LINE;
	bool doublewidth = linecode & O_DOUBLE_WIDTH;
	bool centered    = linecode & O_CENTERED;
	bool singleline  = !(doubleline || doublewidth );
	
	sp = string;
	
	memset( newlin, ' ', 17 );  // create a blank line with spaces
	int siz = strlen( sp );
	int width = doublewidth ? 8 : 16;
	
	if( centered )
	{
		if( siz >=width )
			strncpy( newlin, sp, width );
		else					// if siz=12, 16-12=4, half each side
			strncpy( newlin+(width-siz)/2, sp, siz );
	}
	else						// copy, but only valid chars to preserve the spaces
		strncpy( newlin, sp, siz);
	newlin[width] = 0;
	
    setTextXY( X, 0 );
	sp = newlin;
    while( data = *sp++ )
    {
        for(int i=0;i<m_font_width;i++) 	//m_font_width
        {
	      char ch = pgm_read_byte( &m_font[(data-32)*m_font_width+m_font_offset+i]); 
		  if( doubleline )
			  sendData( b21(ch) );
		  if( doublewidth )
			  sendData( b21(ch) );
		  if( singleline )
			  sendData( ch );
        }
    }
	if( doubleline )
    {
		setTextXY(X+1, 0);
		sp = newlin;;
		while( data = *sp++ )
		{
			for(int i=0;i<m_font_width;i++)	
			{
			  char ch = pgm_read_byte(&m_font[(data-32)*m_font_width+m_font_offset+i])>>4;
			  sendData( b21(ch) );
			  if( doublewidth )
				  sendData( b21(ch) );
			}
		}
	}
	return string;
}

	// no /a /b /v formating allowed here.
	const char *OLED::dsp( int row, int col, const char *format, ... )
	{
		const char *format1 = format;
		if( *format < 0x20 )	// ignore control codes
			format1++;
			
		va_list ap;
		va_start( ap, format );
		vsnprintf( temp, sizeof( temp ), format, ap );
		va_end( ap );
		
		setTextXY( row, col );

		char *sp = temp;
		while( char data = *sp++ )
		{
			for(int i=0;i<m_font_width;i++) 	//m_font_width
			{
			  char ch = pgm_read_byte( &m_font[(data-32)*m_font_width+m_font_offset+i]); 
			  sendData( ch );
			}
		}
		return (const char *)temp;
	}
	
	const char *OLED::dsp( control_t X )
	{
		switch( X )
		{
			case O_LED096:
				init( OLED096 );
				break;
			case O_LED130:
				init( OLED130 );
				break;
			case O_CLEAR:
				clearDisplay();
				break;
			case O_BRIGHT0:
				displayOn( false );
				break;
			case O_BRIGHT1:
				displayOn();
				setBrightness( 1 );	
				break;
			case O_BRIGHT2:
				displayOn();
				setBrightness( 128 );	
				break;
			case O_BRIGHT3:
				displayOn();
				setBrightness( 255 );	// 45, 90, ... 225
				break;
		}
		sprintf( temp, "FUNC(%d)", (int) X );
		return (const char*) temp;
	}
		
	const char *OLED::dsp( int X, const char *format, ... )
	{
		if( *format == 0 )		// clear this row
		{
			clearRow( X );
			return "FUNC(CLN)";
		}
		const char *format1 = format;
		int code = 0;
		if( *format == '\t' )
		{
			code |= O_CENTERED;
			format1  = format+1;
		}
		switch( *format1 )
		{
			case '\v':
				code |= O_DOUBLE_LINE;
				format1++;
				break;
			case '\b':
				code |= (O_DOUBLE_LINE + O_DOUBLE_WIDTH);
				format1++;
				break;
			case '\a':
				format1++;
				break;
		}			
		va_list ap;
		va_start( ap, format );
		vsnprintf( temp, sizeof( temp ), format1, ap );
		va_end( ap );
	
		return displayRow( code, X, (const char *)temp );
	}

// THERE IS A BUG IN DSP() ROW 0 or 2 when displaying \b