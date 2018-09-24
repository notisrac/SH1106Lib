/*
 * SH1106Lib\Examples\Benchmark.ino
 * A small "benchmark" sketch for testing speeds of different microcontrollers
 * Image conversion was made with: File to C style array converter https://notisrac.github.io/FileToCArray/
 * 
 * Warning: it will compile to ~5500 bytes, so it won't fit onto devices with 4k or less storage!
 * 
 * 2018, noti
 */

// softwareI2C lib
#define I2C_HARDWARE 1
#define I2C_TIMEOUT 10
#define I2C_MAXWAIT 10
#define I2C_PULLUP 1
#define I2C_FASTMODE 1
#define SDA_PORT PORTC
#define SDA_PIN 4 // = A4
#define SCL_PORT PORTC
#define SCL_PIN 5 // = A5

// tinyprint lib
#define TP_PRINTLINES 0
#define TP_FLASHSTRINGHELPER 1
#define TP_NUMBERS 1
#define TP_FLOAT 0
#define TP_WINDOWSLINEENDS 0

#define IMAGE_HEIGHT 5
#define IMAGE_WIDTH 11

// array size is 11
static const byte data[] PROGMEM = {
	B00011111, B00010001,
	B00010001, B00010001,
	B00010001, B00010001,
	B00010001, B00010001,
	B00011111, B00001010,
	B00001110
};

#include "SH1106Lib.h"
#include "font_3x5.h"

SH1106Lib display;

void setup() {
	display.initialize();
	display.clearDisplay();

	display.setFont(font_3x5, FONT_3x5_WIDTH, FONT_3x5_HEIGHT, 45, FONT_NUMBERS | FONT_UPPERCASECHARS);
	display.setTextWrap(true);
	display.setTextColor(WHITE, TRANSPARENT);
}

void loop() {
	// "start" the timer
	unsigned long start = micros();

	// bitmap
	for (uint8_t i = 0; i < 64; i++)
	{
		display.drawBitmap(random(0, 110), random(0, 60), data, IMAGE_WIDTH, IMAGE_HEIGHT, WHITE, BLACK);
	}

	// text
	display.setCursor(0, 0);
	for (uint8_t i = 0; i < 65; i++)
	{
		display.print(F("ASD "));
	}

	// rectangles
	for (uint8_t i = 0; i < 48; i++)
	{
		display.fillRect(random(0, 80), random(0, 40), 40, 20, i % 2);
	}

	// print the result
	display.fillRect(18, 13, 84, 24, WHITE);
	display.fillRect(20, 15, 80, 20, BLACK);
	display.setCursor(35, 24);
	long diff = micros() - start;
	display.print(diff / 1000, DEC);
	display.print(F(" MS"));


	delay(2000);
	display.clearDisplay();
}
