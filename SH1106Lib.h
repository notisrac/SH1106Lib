/***************************************************************************
  This is a library for the SH1106 I2C OLED display
  These displays use I2C to communicate, 2 pins are required to  interface

  Written by https://github.com/notisrac
  MIT license, all text above must be included in any redistribution
 ***************************************************************************/

#pragma once

#ifndef _SH1106LIB_H
#define _SH1106LIB_H


#include <Arduino.h>
#include <TinyPrint.h>

#define I2C_HARDWARE 1
#define I2C_TIMEOUT 10
#define I2C_MAXWAIT 10
#define I2C_PULLUP 1
#define I2C_FASTMODE 1
#define SDA_PORT PORTC
#define SDA_PIN 4 // = A4
#define SCL_PORT PORTC
#define SCL_PIN 5 // = A5

// region display_constants
#define SH1106_I2C_ADDRESS   0x3C // 011110+SA0+RW - 0x3C or 0x3D

#define SH1106_LCDWIDTH                  128
#define SH1106_LCDHEIGHT                 64

#define SH1106_SETCONTRAST 0x81
#define SH1106_DISPLAYALLON_RESUME 0xA4
#define SH1106_DISPLAYALLON 0xA5

#define SH1106_NORMALDISPLAY 0xA6
#define SH1106_INVERTDISPLAY 0xA7
#define SH1106_DISPLAYOFF 0xAE
#define SH1106_DISPLAYON 0xAF

#define SH1106_SETDISPLAYOFFSET 0xD3
#define SH1106_SETCOMPINS 0xDA

#define SH1106_SETVCOMDETECT 0xDB

#define SH1106_SETDISPLAYCLOCKDIV 0xD5
#define SH1106_SETPRECHARGE 0xD9

#define SH1106_SETMULTIPLEX 0xA8

#define SH1106_SETCOLUMNADDRLOW 0x00
#define SH1106_SETCOLUMNADDRHIGH 0x10

#define SH1106_SETSTARTLINE 0x40

#define SH1106_MEMORYMODE 0x20
//#define SH1106_COLUMNADDR 0x21
#define SH1106_PAGEADDR   0xB0

#define SH1106_COMSCANINC 0xC0
#define SH1106_COMSCANDEC 0xC8

#define SH1106_SEGREMAP 0xA0

#define SH1106_CHARGEPUMP 0x8D

#define SH1106_EXTERNALVCC 0x1
#define SH1106_SWITCHCAPVCC 0x2

#define SH1106_MESSAGETYPE_COMMAND 0x80
#define SH1106_MESSAGETYPE_DATA 0x40

#define SH1106_READMODIFYWRITE_START 0xE0
#define SH1106_READMODIFYWRITE_END 0xEE

#define SH1106_MAXSEGMENTSPERWRITE 16
#define SH1106_ROWHEIGHT 8

#define SH1106_NUMBEROF_SEGMENTS 128
#define SH1106_NUMBEROF_PAGES 8

// endregion display_constants

#define BLACK 0
#define WHITE 1
#define TRANSPARENT 255
#define SOLID 127

#define I2CREAD 1
#define I2CWRITE 0

// font flags
#define FONT_NUMBERS        1
#define FONT_UPPERCASECHARS 2
#define FONT_LOWERCASECHARS 4
#define FONT_HASSPACE       8
#define FONT_FULL           255


class SH1106Lib : public TinyPrint
{
public:
	SH1106Lib();
	~SH1106Lib();

	void initialize();
	void sendCommand(byte c, bool useOwnTransmission = true);
	void sendData(byte data, bool useOwnTransmission = true);
	void sendData(byte *data, uint8_t count, bool useOwnTransmission = true);

	void clearDisplay(void);
	void fillRect(uint8_t left, uint8_t top, uint8_t width, uint8_t height, uint8_t color);
	void drawBitmap(uint8_t x, uint8_t y, const byte *bitmap, uint8_t w, uint8_t h, uint8_t color, uint8_t backgroundType);
	void drawPixel(uint8_t x, uint8_t y, uint8_t color);

	void setCursor(uint8_t x, uint8_t y);
	void resetCursor();
	void setFont(const unsigned char *font, uint8_t width, uint8_t height, int8_t offset = 0, uint8_t flags = FONT_FULL);
	void setTextWrap(bool enableWrap);
	void setTextColor(uint8_t color, uint8_t backgroundType);
	// Inherited via TinyPrint
	virtual byte write(uint8_t) override;
	void drawChar(uint8_t x, uint8_t y, uint8_t character, uint8_t color, uint8_t backgroundType);
private:
	void _setDisplayWritePosition(uint8_t x, uint8_t y, bool useOwnTransmission = false);
	void _beginTransmission(byte operation = I2CWRITE, bool startNewTransmission = false);
	void _endTransmission();
	/**
		Moves the display write position, and starts the ReadModifyWrite mode

		@param x The x coordinate 0..127 (this will select column)
		@param y The y coordinate 0..63 (this will select the page - will be converted to page number)
		@returns nothing
	*/
	void _startRMWMode(uint8_t x, uint8_t y);
	/**
		Sopts the ReadModifyWrite mode

		@returns nothing
	*/
	void _stopRMWMode();
	/**
		Draws a single column (8bits vertically) on the current page.
		Note: the ReadModifyWrite must be enabled for this to work

		@param data The byte that will be output to the column
		@param color The color that the 1 pixels is displayed should be black or whit - ANDs or ORs the data to the pixels on the screen
		@param backgroundType The type of the background. Valid values: SOLID, TRANSPARENT
		@param backgroundMask A byte mask to specify which bits are actually relevant in the data
		@returns nothing
	*/
	void _drawColumn(uint8_t data, uint8_t color, uint8_t backgroundType, byte backgroundMask);
	/**
		Draws a number of columns (same 8bits vertically) on the current page.
		Note: the ReadModifyWrite must be enabled for this to work

		@param x The x coordinate 0..127 (this will select column)
		@param y The y coordinate 0..63 (this will select the page - will be converted to page number)
		@param data The byte that will be output to the column
		@param count How many columns should be filled with the data
		@param color The color that the 1 pixels is displayed should be black or whit - ANDs or ORs the data to the pixels on the screen
		@returns nothing
	*/
	void _drawColumns(uint8_t x, uint8_t y, uint8_t data, uint8_t count, uint8_t color);

	// text related variables
	uint8_t _cursorX = 0;
	uint8_t _cursorY = 0;
	uint8_t _textColor = WHITE;
	uint8_t _backgroundType = TRANSPARENT;
	uint8_t _fontWidth = 5;
	uint8_t _fontHeight = 8;
	int8_t _fontOffset = 0;
	uint8_t _fontFlags = FONT_FULL;
	bool _wrap = true;
	const unsigned char* _font;

	uint8_t _pixelPosX = 0;
	uint8_t _pixelPosY = 0;

	// i2c vars
	bool _i2cTransmissionInProgress = false;
};

#endif // _SH1106LIB_H

/*
Empty:
Program LedTester size: 4�544 bytes (used 14% of a 32�256 byte maximum) (1,05 secs)
Minimum Memory Usage: 317 bytes (15% of a 2048 byte maximum)

D
Program LedTester size: 4�604 bytes (used 14% of a 32�256 byte maximum) (1,08 secs)
Minimum Memory Usage: 333 bytes (16% of a 2048 byte maximum)

Program LedTester size: 4�680 bytes (used 15% of a 32�256 byte maximum) (1,13 secs)
Minimum Memory Usage: 327 bytes (16% of a 2048 byte maximum)

D : TP
Program LedTester size: 4�746 bytes (used 15% of a 32�256 byte maximum) (1,19 secs)
Minimum Memory Usage: 327 bytes (16% of a 2048 byte maximum)

D : P
Program LedTester size: 4�668 bytes (used 14% of a 32�256 byte maximum) (1,13 secs)
Minimum Memory Usage: 3	7 bytes (16% of a 2048 byte maximum)

lib + tinyp
Program LedTester size: 4�660 bytes (used 14% of a 32�256 byte maximum) (2,04 secs)
Minimum Memory Usage: 331 bytes (16% of a 2048 byte maximum)


Program LedTester size: 5�074 bytes (used 16% of a 32�256 byte maximum) (1,10 secs)
Minimum Memory Usage: 331 bytes (16% of a 2048 byte maximum)

lib + tinyp 2018.03.25
Program LedTester size: 3�720 bytes (used 12% of a 32�256 byte maximum) (0,96 secs)
Minimum Memory Usage: 219 bytes (11% of a 2048 byte maximum)

lib nobuffer + tinyp 2018.04.15
Program LedTester size: 5�478 bytes (used 17% of a 32�256 byte maximum) (1,06 secs)
Minimum Memory Usage: 236 bytes (12% of a 2048 byte maximum)

lib nobuffer + tinyp 2018.04.15
Program LedTester size: 6�650 bytes (used 21% of a 32�256 byte maximum) (1,37 secs)
Minimum Memory Usage: 238 bytes (12% of a 2048 byte maximum)

lib nobuffer + print 2018.04.15
Program LedTester size: 6�574 bytes (used 20% of a 32�256 byte maximum) (1,30 secs)
Minimum Memory Usage: 244 bytes (12% of a 2048 byte maximum)

lib nobuffer + tinyprint + softi2cmaster 2018.05.01
Program LedTester size: 6 696 bytes (used 21% of a 32�256 byte maximum) (0,68 secs)
Minimum Memory Usage: 208 bytes (10% of a 2048 byte maximum)

lib nobuffer + tinyprint + softi2cmaster (RMW mode) 2018.08.05
Program SH1106Test size: 5 316 bytes (used 16% of a 32 256 byte maximum) (3,64 secs)
Minimum Memory Usage: 210 bytes (10% of a 2048 byte maximum)
*/
