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

/*
	Draws a bitmap from the program memory to the display
	x: the x coordinate to put the bitmap
	y: the y coordiante to put the bitmap
	bitmap: the byte array representing the image
	w: the width of the image
	h: the height of the image
	color: the color which the image should be displayed
*/
void SH1106Lib::drawBitmap(uint8_t x, uint8_t y, const byte * bitmap, uint8_t w, uint8_t h, uint8_t color, uint8_t bgcolor)
{
	if (w > SH1106_LCDWIDTH || h > SH1106_LCDHEIGHT)
	{ // sanity check
		return;
	}

	uint8_t i, j;
	uint8_t bitMask = 128;
	uint8_t actualByte = 0x00;
	bool bit = false;

	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) {
			actualByte = pgm_read_byte(bitmap + (((j * w) + i) / 8));
			bit = actualByte & bitMask;

			if (bit)
			{
				drawPixel(x + i, y + j, color);
			}
			else if (bgcolor != TRANSPARENT)
			{
				drawPixel(x + i, y + j, bgcolor);
			}

			bitMask >>= 1;
			if (!bitMask)
			{
				bitMask = 128;
			}

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
	_beginTransmission(I2CWRITE, true);

	// set the position and enable the readmodifywrite mode
	_setDisplayWritePosition(x, y, false);
	sendCommand(SH1106_READMODIFYWRITE_START, false);
	i2c_write(0xC0); // ???? 

	// read the pixel data from the display
	_beginTransmission(I2CREAD, false); // restart in read mode
	i2c_read(false); // dummy read
	byte b = i2c_read(true);
	//Serial.println(b, BIN);
	if (WHITE == color)
	{
		b |= (1 << (y & 7));
	}
	else
	{
		b &= ~(1 << (y & 7));
	}
	// write the modified data back
	sendData(b, false);
	// end the readmodifywrite mode
	sendCommand(SH1106_READMODIFYWRITE_END, false);

	_endTransmission();
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
void SH1106Lib::setFont(const unsigned char *font, uint8_t width, uint8_t height, int8_t offset = 0, bool useBlankAsSpace = false)
{
	_font = font;
	_fontWidth = width;
	_fontHeight = height;
	_fontOffset = offset;
	_fontUseBlankAsSpace = useBlankAsSpace;
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
void SH1106Lib::setTextColor(uint8_t color, uint8_t backColor)
{
	_textColor = color;
	_backgroundColor = backColor;
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
		drawChar(_cursorX, _cursorY, c, _textColor, _backgroundColor);
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


/*
	Draws a character on the screen from the font
	x: x coordinate where the character should be displayed
	y: y coordinate where the character should be displayed
	character: The character to display
	color: the color of the character
*/
void SH1106Lib::drawChar(uint8_t x, uint8_t y, uint8_t character, uint8_t color, uint8_t backgroundColor)
{
	if ((x + _fontWidth >= SH1106_LCDWIDTH) || // Clip right
		(y + _fontHeight >= SH1106_LCDHEIGHT) || // Clip bottom
		((x + _fontWidth + 1) < 0) || // Clip left
		((y + _fontHeight) < 0))   // Clip top
		return;

	uint8_t line, i, j, k;
	// how many bytes does one column of the font takes up
	uint8_t byteWidth = ceil((float)_fontHeight / 8);

	for (i = 0; i < (_fontWidth + 1); i++) {
		for (k = 0; k < byteWidth; k++)
		{ // if the fon is taller than 8
			if (i == _fontWidth || (character == ' ' && _fontUseBlankAsSpace)) {
				line = 0x00;
			}
			else {
				// <the locaton of the font in memory> + 
				// (<the character code> - <the offset (if the fontset is not complete)>) * <width of the font>
				// 
				line = pgm_read_byte(_font + (((character - _fontOffset) * _fontWidth) + i)*byteWidth + k);
			}

			for (j = k * 8; j < min(k * 8 + 8, _fontHeight + 1); j++) { // draw all the bits from the current line of the char
				if (line & 0x1) { // if the current bit(pixel) is a 1, then plot a color pixel
					drawPixel(x + i, y + j, color);
				}
				else { // otherwise plot a black pixel
					if (TRANSPARENT != backgroundColor)
					{
						drawPixel(x + i, y + j, backgroundColor);
					}
				}
				// shift right to get the next bit(pixel)
				line >>= 1;
			}
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
