/*
 * SH1106Lib\Examples\Empty.ino
 * Example sketch demonstrating the bare minimum needed to run the lib
 * 
 * 2018, noti
 */

// config for softwareI2Cmaster lib
#define SDA_PORT PORTA
#define SDA_PIN 6 // = A4
#define SCL_PORT PORTA
#define SCL_PIN 4 // = A5

#include <SH1106Lib.h>

SH1106Lib display;

void setup() {
	display.initialize();
	display.clearDisplay();
}

void loop() {
	// display.fillRect(10, 10, 80, 30, WHITE);
	// for (uint8_t i = 0; i < 100; i++)
	// {
	// 	display.fillRect(random(0, 80), random(0, 40), 40, 20, i % 2);
	// }
	// delay(1000);
	// display.clearDisplay();
}

