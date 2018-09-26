/*
 * SH1106Lib\Examples\Text.ino
 * Example sketch demonstrating how use the lib to display text
 * glcdfont.h is part of the Adafruit GFX Library:
 * https://github.com/adafruit/Adafruit-GFX-Library/blob/master/glcdfont.c
 * 
 * 2018, noti
 */

// config for softwareI2Cmaster lib
#define I2C_HARDWARE 1
#define I2C_TIMEOUT 10
#define I2C_MAXWAIT 10
#define I2C_PULLUP 1
#define I2C_FASTMODE 1
#define SDA_PORT PORTC
#define SDA_PIN 4 // = A4
#define SCL_PORT PORTC
#define SCL_PIN 5 // = A5

// config for tinyprint lib
#define TP_PRINTLINES 0
#define TP_FLASHSTRINGHELPER 1
#define TP_NUMBERS 0
#define TP_FLOAT 0
#define TP_WINDOWSLINEENDS 0

#include <SH1106Lib.h>
#include "glcdfont.h"

SH1106Lib display;

void setup() {
	display.initialize();
	display.clearDisplay();

	display.setFont(font, 5, 7);
	display.setTextWrap(true);
	display.setTextColor(WHITE, BLACK);
}

void loop() {
	display.setCursor(0, 0);
	display.print(F("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip..."));

	delay(1000);
	display.clearDisplay();
}
