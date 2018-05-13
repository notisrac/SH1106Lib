#include <SH1106Lib.h>
#include "glcdfont.h"

SH1106Lib display;

void setup() {
	display.initialize();
	display.clearDisplay();
}

void loop() {
	display.setFont(font, 5, 7);
	display.setCursor(0, 0);
	display.setTextWrap(true);
	display.setTextColor(WHITE, BLACK);
	display.print(F("White man came across the sea He brought us pain and misery He killed our tribes he killed our creed He took our game for his own need We fought him hard we fought him well Out on the plains we gave him hell"));

	delay(1000);
	display.clearDisplay();
}
