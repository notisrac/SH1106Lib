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
