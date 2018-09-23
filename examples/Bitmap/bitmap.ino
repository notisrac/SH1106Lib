/*
 * SH1106Lib\Examples\Bitmap.ino
 * Example sketch demonstrating how use the lib to display a bitmap image
 * Image conversion was made with: File to C style array converter https://notisrac.github.io/FileToCArray/
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
#define TP_FLASHSTRINGHELPER 0
#define TP_NUMBERS 0
#define TP_FLOAT 0
#define TP_WINDOWSLINEENDS 0

#include <SH1106Lib.h>
#include "rick.h"

SH1106Lib display;
uint8_t x = 0;

void setup() {
	display.initialize();
	display.clearDisplay();
}

void loop() {
 	display.drawBitmap(x, 3, data, IMAGE_WIDTH, IMAGE_HEIGHT, WHITE, BLACK);
  x += 10;
  if(x >= (128 - IMAGE_WIDTH))
  {
    x = 0;
  }

	delay(100);
	display.clearDisplay();
}
