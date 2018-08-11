#include <SH1106Lib.h>

SH1106Lib display;

void setup() {
	display.initialize();
	display.clearDisplay();
}

void loop() {
	display.fillRect(10, 10, 80, 30, WHITE);

	delay(1000);
	display.clearDisplay();
}

/*
Sketch uses 3570 bytes (11%) of program storage space. Maximum is 32256 bytes.
Global variables use 33 bytes (1%) of dynamic memory, leaving 2015 bytes for local variables. Maximum is 2048 bytes.
*/
