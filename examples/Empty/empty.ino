/*
 * SH1106Lib\Examples\Empty.ino
 * Example sketch demonstrating the bare minimum needed to run the lib
 * 
 * 2018, noti
 */

// config for softwareI2Cmaster lib
#define SDA_PORT PORTC
#define SDA_PIN 4 // = A4
#define SCL_PORT PORTC
#define SCL_PIN 5 // = A5

#include <SH1106Lib.h>

SH1106Lib display;

void setup() {
	display.initialize();
	display.clearDisplay();
}

void loop() {
	//display.fillRect(10, 10, 80, 30, WHITE);
}

