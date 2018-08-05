/***************************************************
	This is a library for the SH1106 I2C OLED display
	These displays use I2C to communicate, 2 pins are required to  interface

	Written by https://github.com/notisrac
	MIT license, all text above must be included in any redistribution
****************************************************/

#include "SH1106Lib.h"
#include <SoftI2CMaster.h>


SH1106Lib::SH1106Lib()
{
}

SH1106Lib::~SH1106Lib()
{
}

/*
	Initializes the display
*/
void SH1106Lib::initialize()
{
	if (!i2c_init()) { // Initialize everything and check for bus lockup
		//Serial.println("I2C init failed");
	}


	//TWBR = 12; // upgrade i2c speed to 400KHz!
	_beginTransmission(I2CWRITE, true);

	sendCommand(SH1106_DISPLAYOFF, false);                    // 0xAE
	sendCommand(SH1106_SETDISPLAYCLOCKDIV, false);            // 0xD5
	sendCommand(0x80, false);                                  // the suggested ratio 0x80
	sendCommand(SH1106_SETMULTIPLEX, false);                  // 0xA8
	sendCommand(0x3F, false);
	sendCommand(SH1106_SETDISPLAYOFFSET, false);              // 0xD3
	sendCommand(0x00, false);                                   // no offset

	sendCommand(SH1106_SETSTARTLINE | 0x0, false);            // line #0 0x40
	sendCommand(SH1106_CHARGEPUMP, false);                    // 0x8D
														  // SH1106_EXTERNALVCC
	sendCommand(0x10, false);
	sendCommand(SH1106_MEMORYMODE, false);                    // 0x20
	sendCommand(0x00, false);                                  // 0x0 act like ks0108
	sendCommand(SH1106_SEGREMAP | 0x1, false);
	sendCommand(SH1106_COMSCANDEC, false);
	sendCommand(SH1106_SETCOMPINS, false);                    // 0xDA
	sendCommand(0x12, false);
	sendCommand(SH1106_SETCONTRAST, false);                   // 0x81
														  // SH1106_EXTERNALVCC
	sendCommand(0x9F, false);
	sendCommand(SH1106_SETPRECHARGE, false);                  // 0xd9
														  // SH1106_EXTERNALVCC
	sendCommand(0x22, false);
	sendCommand(SH1106_SETVCOMDETECT, false);                 // 0xDB
	sendCommand(0x40, false);
	sendCommand(SH1106_DISPLAYALLON_RESUME, false);           // 0xA4
	sendCommand(SH1106_NORMALDISPLAY, false);                 // 0xA6

	sendCommand(SH1106_DISPLAYON, false);//--turn on oled panel

									 // ?????
	sendCommand(SH1106_SETCOLUMNADDRLOW | 0x0, false);  // low col = 0
	sendCommand(SH1106_SETCOLUMNADDRHIGH | 0x0, false);  // hi col = 0
	// TODO can this be moved to begin???
	sendCommand(SH1106_SETSTARTLINE | 0x0, false); // line #0

	_endTransmission();
}

/*
	Sends a single command to the display
	command: the command (byte) that needs to be sent to the device
	startTransmission: should it start a new transmission? Set this to false if you already started a transmission
*/
void SH1106Lib::sendCommand(byte command, bool useOwnTransmission = true)
{
	// I2C
	_beginTransmission(I2CWRITE, useOwnTransmission);

	i2c_write(SH1106_MESSAGETYPE_COMMAND); // This is a Command 
	i2c_write(command);

	if (useOwnTransmission)
	{
		_endTransmission();
	}
}

/*
	Sends a single byte of data to the display
	data: the data (byte) that needs to be sent to the device
*/
void SH1106Lib::sendData(byte data, bool useOwnTransmission = true)
{
	// I2C
	_beginTransmission(I2CWRITE, useOwnTransmission);

	i2c_write(SH1106_MESSAGETYPE_DATA); // This byte is DATA 
	i2c_write(data);

	if (useOwnTransmission)
	{
		_endTransmission();
	}
}

/*
	Sends a batch of data to the display
	data: the data array (bytes) that needs to be sent to the device
	count: the number of items in the array
*/
void SH1106Lib::sendData(byte * data, uint8_t count, bool useOwnTransmission = true)
{
	// TODO what is up when count > SH1106_MAXSEGMENTSPERWRITE?

	// I2C
	_beginTransmission(I2CWRITE, useOwnTransmission);

	i2c_write(SH1106_MESSAGETYPE_DATA); // This byte is DATA 
	for (uint8_t k = 0; k < count; k++) {
		i2c_write(data[k]);
	}

	if (useOwnTransmission)
	{
		_endTransmission();
	}
}

/*
	Clears the display
*/
void SH1106Lib::clearDisplay(void)
{
	uint8_t page, segments;
	byte buffer[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	_beginTransmission(I2CWRITE, true);
	// clear the buffer so we can fill the screen with zeroes
	for (page = 0; page < SH1106_NUMBEROF_PAGES; page++)
	{
		// move to the beginning of the next page
		_setDisplayWritePosition(0, page * SH1106_ROWHEIGHT);
		for (segments = 0; segments < (SH1106_NUMBEROF_SEGMENTS / SH1106_MAXSEGMENTSPERWRITE); segments++)
		{
			// no need to set the draw position, as every memory write advances the write pos to the next one
			sendData(buffer, SH1106_MAXSEGMENTSPERWRITE, false);
		}
	}
	_endTransmission();
}

unsigned char reverse(unsigned char b) {
	b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
	b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
	b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
	return b;
}

char* printBits(byte myByte) {
	static char str[9];
	byte mask = 0x80;

	for (uint8_t i = 0; i < 8; i++)
	{
		if (mask & myByte)
		{
			str[i] = '1';
		}
		else
		{
			str[i] = '0';
		}
		mask >>= 1;
	}
	str[8] = '\0';

	return str;
}

void SH1106Lib::fillRect(uint8_t left, uint8_t top, uint8_t width, uint8_t height, uint8_t color)
{
	// sanity check
	if (left > SH1106_LCDWIDTH || top > SH1106_LCDHEIGHT)
	{
		return;
	}
	if (left + width > SH1106_LCDWIDTH)
	{
		width = SH1106_LCDWIDTH - left;
	}
	if (top + height > SH1106_LCDHEIGHT)
	{
		height = SH1106_LCDHEIGHT - top - 1;
	}

	uint8_t i, j = 0;
	byte maskStart, maskEnd = B00000000;

	// calculate the start and the end masks
	maskStart = B11111111 << ((top % 8));
	maskEnd = ~(B11111111 << ((top + height) % 8) + 1);

	if (height <= 8 && (floor(top / 8) == floor((top + height) / 8)))
	{ // smaller than a page, so combine the start and the end mask
		maskStart = maskStart & maskEnd;
	}
	else
	{
		// write the end mask to the last page it touches
		_drawColumns(left, top + height, maskEnd, width, color);
	}

	// write the start mask
	_drawColumns(left, top, maskStart, width, color);

	if (height > 8)
	{ // larger than 8
		// loop through the inner parts
		for (i = top + (8 - (top % 8)); i < top + height - ((top + height) % 8); i += 8)
		{
			_drawColumns(left, i, B11111111, width, color);
		}
	}
}

/*
	Draws a bitmap from the program memory to the display
	x: the x coordinate to put the bitmap
	y: the y coordiante to put the bitmap
	bitmap: the byte array representing the image
	w: the width of the image
	h: the height of the image
	color: the color which the image should be displayed (BLACK/WHITE)
	backgroundType: SOLID or TRANSPARENT background
*/
void SH1106Lib::drawBitmap(uint8_t x, uint8_t y, const byte * bitmap, uint8_t w, uint8_t h, uint8_t color, uint8_t backgroundType = SOLID)
{
	if (w > SH1106_LCDWIDTH || h > SH1106_LCDHEIGHT)
	{ // sanity check
		return;
	}

	uint8_t i, j, n, diff, yActual;
	uint8_t actualByte = 0x00;

	//uint8_t startSize = 8 - (y % 8);
	//uint8_t endSize = (y + h) % 8;

	// height / 8, because we will plot in columns with the height of 8
	for (j = 0; j < ceil(h / 8.0); j++) {
		// calculate the start pos
		diff = (y + j * 8) % 8;
		for (n = 0; n < ((0 == diff)? 1 : 2); n++) // if it starts on the page border, then we can do it in one run
		{
			yActual = y + ((j + n) * 8);
			if (yActual > SH1106_LCDHEIGHT)
			{ // don't try to write outside the display area
				continue;
			}
			_startRMWMode(x, y + ((j + n) * 8));
			// loop through the width of the image, and plot the columns
			for (i = 0; i < w; i++) {
				if ((x + i) > SH1106_LCDWIDTH)
				{ // don't try to write outside the display area
					continue;
				}
				actualByte = pgm_read_byte(bitmap + (i + (j * w)));
				if (0 == n)
				{ // this is the below the page barrier
					actualByte = actualByte << diff;
				}
				else
				{ // this is the leftover - this only comes in play when the current part of the image crosses the page boundary
					actualByte = actualByte >> 8 - diff;
				}
				// display the column of pixels
				_drawColumn(actualByte, color, backgroundType, B11111111);
			}
			_stopRMWMode();
		}
	}
}

/*
	Draws a single pixel on th display
	x: the x coordinate to put the pixel
	y: the y coordiante to put the pixel
	color: the color of the pixel
*/
void SH1106Lib::drawPixel(uint8_t x, uint8_t y, uint8_t color)
{
	if (x < 0 || x > SH1106_LCDWIDTH
		|| y < 0 || y > SH1106_LCDHEIGHT)
	{
		return;
	}

	_drawColumns(x, y, 1 << (y & 7), 1, color);
}

/*
	Sets the text cursor to this position
	x: the x coordinate
	y: the y coordiante
*/
void SH1106Lib::setCursor(uint8_t x, uint8_t y)
{
	_cursorX = x;
	_cursorY = y;
}

/*
	Resets the text cursor to 0,0
*/
void SH1106Lib::resetCursor()
{
	_cursorX = 0;
	_cursorY = 0;
}

/*
	Sets the font to write with
	font: pinter to the array containing the font
	width: the width of the font in pixels
	height: the height of the font in pixels
	offset: signed value to offset the position the character is found in the font
*/
void SH1106Lib::setFont(const unsigned char *font, uint8_t width, uint8_t height, int8_t offset = 0, uint8_t flags = FONT_FULL)
{
	_font = font;
	_fontWidth = width;
	_fontHeight = height;
	_fontOffset = offset;
	_fontFlags = flags;
}

/*
	Sets whether the text should continue on the next row, if it has reached the end of the current one
	enableWrap: true to enable, false to disable
*/
void SH1106Lib::setTextWrap(bool enableWrap)
{
	_wrap = enableWrap;
}

/*
	Sets whether the text should continue on the next row, if it has reached the end of the current one
	color: the color of the text
	backColor: the color of the background of the text. Set it to TRANSPARENT to have a transparend background
*/
void SH1106Lib::setTextColor(uint8_t color, uint8_t backgroundType)
{
	_textColor = color;
	_backgroundType = backgroundType;
}

/*
	Displays a single character
	c: the character to display

	returns: 1
*/
byte SH1106Lib::write(uint8_t c)
{
	Serial.print("%");
	if (c == '\n') { // on a linebreak move the cursor down one line, and back to the start
		setCursor(0, _cursorY + _fontHeight);
	}
	else if (c == '\r') {
		// skip carrage return, as we already handle newline
	}
	else {
		// draw the pixels for the actual character
		drawChar(_cursorX, _cursorY, c, _textColor, _backgroundType);
		// advance the cursor
		setCursor(_cursorX + _fontWidth + 1, _cursorY);
		// if the next letter would not fit in the current row, jump to the beginnign of the next one
		if (_wrap && (_cursorX > (SH1106_LCDWIDTH - _fontWidth + 1))) {
			//Serial.print();
			setCursor(0, _cursorY + _fontHeight + 1);
		}
	}
	return 1;
}


/*
	Draws a character on the screen from the font
	x: x coordinate where the character should be displayed
	y: y coordinate where the character should be displayed
	character: The character to display
	color: the color of the character
	backgroundType: SOLID or TRANSPARENT
*/
void SH1106Lib::drawChar(uint8_t x, uint8_t y, uint8_t character, uint8_t color, uint8_t backgroundType)
{
	if ((x + _fontWidth >= SH1106_LCDWIDTH) || // Clip right
		(y + _fontHeight >= SH1106_LCDHEIGHT) || // Clip bottom
		((x + _fontWidth + 1) < 0) || // Clip left
		((y + _fontHeight) < 0))   // Clip top
		return;

	uint8_t i, j, n;
	uint8_t diff, yActual, actualByte;
	uint8_t byteHeight = ceil(_fontHeight / 8.0);

	// try to mod the character, if the font does not have the required case
	if (isLowerCase(character))
	{
		if ((_fontFlags & FONT_LOWERCASECHARS) != FONT_LOWERCASECHARS)
		{
			character = toUpperCase(character);
		}
	}
	else
	{
		if ((_fontFlags & FONT_UPPERCASECHARS) != FONT_UPPERCASECHARS)
		{
			character = toLowerCase(character);
		}
	}


	// height / 8, because we will plot in columns with the height of 8
	for (j = 0; j < byteHeight; j++) {
		// calculate the start pos
		diff = (y + j * 8) % 8;
		for (n = 0; n < ((0 == diff) ? 1 : 2); n++) // if it starts on the page border, then we can do it in one run
		{
			yActual = y + ((j + n) * 8);
			if (yActual > SH1106_LCDHEIGHT)
			{ // don't try to write outside the display area
				continue;
			}
			_startRMWMode(x, y + ((j + n) * 8));
			// loop through the width of the image, and plot the columns
			for (i = 0; i < _fontWidth; i++) {
				if ((x + i) > SH1106_LCDWIDTH)
				{ // don't try to write outside the display area
					continue;
				}
				if (i == _fontWidth || (character == ' ' && ((_fontFlags & FONT_HASSPACE) != FONT_HASSPACE ))) {
					actualByte = 0x00;
				}
				else {
					actualByte = pgm_read_byte(_font + ((character - _fontOffset) * _fontWidth + i) * byteHeight + j);
				}
				if (0 == n)
				{ // this is the below the page barrier
					actualByte = actualByte << diff;
				}
				else
				{ // this is the leftover - this only comes in play when the current part of the image crosses the page boundary
					actualByte = actualByte >> 8 - diff;
				}
				// display the column of pixels
				_drawColumn(actualByte, color, backgroundType, B11111111);
			}
			_stopRMWMode();
		}
	}

}

/*
	Draws a character on the screen from the font
	x: the x coordinate
	y: the y coordinate
	startTransmission: should it start a new transmission? Set this to false if you already started a transmission
*/
void SH1106Lib::_setDisplayWritePosition(uint8_t x, uint8_t y, bool useOwnTransmission = false)
{
	if (x == _pixelPosX && ((y >> 3) == _pixelPosY))
	{
		return;
	}

	_pixelPosX = x;
	_pixelPosY = y >>3;

	// the SH1106 display starts at x = 2! (there are two columns of off screen pixels)
	x += 2;

	if (useOwnTransmission)
	{
		_beginTransmission(I2CWRITE, true);
	}

	//set page address - there are 8 pages (B0H-B7H), each loop sets the next page - THIS IS THE Y COORD IN 8 INCREMENTS, 0->7
	sendCommand(SH1106_PAGEADDR + (y >> 3), false);
	// set lower column address  (00H - 0FH) => need the upper half only - THIS IS THE X, 0->127
	sendCommand((x & 0x0F), false);
	// set higher column address (10H - 1FH) => 0x10 | (2 >> 4) = 10
	sendCommand(SH1106_SETCOLUMNADDRHIGH + (x >> 4), false);

	if (useOwnTransmission)
	{
		_endTransmission();
	}
}

void SH1106Lib::_beginTransmission(byte operation, bool startNewTransmission = false)
{
	if (_i2cTransmissionInProgress && !startNewTransmission)
	{ // send a repeated start if there is no need to start a new one
		if (!i2c_rep_start((SH1106_I2C_ADDRESS << 1) | operation))
		{
			//Serial.println("I2C device busy");
			return;
		}
	}
	else
	{
		if (_i2cTransmissionInProgress)
		{ // stop the current transmission if there is one
			_endTransmission();
		}

		if (!i2c_start((SH1106_I2C_ADDRESS << 1) | operation)) { // start transfer
			//Serial.println("I2C device busy");
			return;
		}
	}

	_i2cTransmissionInProgress = true;
}

void SH1106Lib::_endTransmission()
{
	i2c_stop();
	_i2cTransmissionInProgress = false;
}

void SH1106Lib::_startRMWMode(uint8_t x, uint8_t y)
{
	_beginTransmission(I2CWRITE, true);

	// set the position and enable the readmodifywrite mode
	_setDisplayWritePosition(x, y, false);
	sendCommand(SH1106_READMODIFYWRITE_START, false);
	i2c_write(0xC0); // ???? 
}

void SH1106Lib::_stopRMWMode()
{
	// end the readmodifywrite mode
	sendCommand(SH1106_READMODIFYWRITE_END, false);
	_endTransmission();
}

void SH1106Lib::_drawColumn(uint8_t data, uint8_t color, uint8_t backgroundType = TRANSPARENT, byte backgroundMask = B00000000)
{
	byte b = data;
	// read the pixel data from the display
	_beginTransmission(I2CREAD, false); // restart in read mode
	i2c_read(false); // dummy read
	b = i2c_read(true);

	// handle the solid/transparent background
	if (SOLID == backgroundType)
	{
		if (BLACK == color)
		{ // solid white background
			b |= backgroundMask;
		}
		else
		{ // solid black background
			b &= ~backgroundMask;
		}
	}

	// modify the read byte with the data, based on the color
	if (WHITE == color)
	{
		b |= data;
	}
	else
	{
		b &= ~data;
	}

	// write the modified data back
	sendData(b, false);
}

void SH1106Lib::_drawColumns(uint8_t x, uint8_t y, uint8_t data, uint8_t count, uint8_t color)
{
	_startRMWMode(x, y);
	for (uint8_t i = 0; i < count; i++)
	{
		_drawColumn(data, color);
	}
	_stopRMWMode();
}
