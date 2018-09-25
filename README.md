# No buffer SH1106 OLED display driver
An Arduino library for the SH1106 I2C OLED display, for the lower end of the Atmel AVR MCU range that has limited resources.
The size is achieved by using libraries that are modular and are size conscious themselves, restricting the featuer list, and eliminating the buffer alltogether.


## FEATURES
 * Absolutely **no buffer**
 * Minimal RAM (33 bytes) and flash (~2300 bytes) requirement
 * Designed for 8-bit AVR devices with limited resources ([see Supported Devices](#supported-devices))
 * Supports hardware and software I2C modes
 * Modular design: size will vary based on the features you enable


## GETTING STARTED
You must install the following libraries first:
 * https://playground.arduino.cc/Main/SoftwareI2CLibrary
 * https://github.com/notisrac/TinyPrint
 * This library :)

_Follow this guide to install them: https://www.arduino.cc/en/Guide/Libraries_

This is the minimal code required to make the library work:
```c++
// first, we need to define the I2C pins for the softwareI2C lib
#define SDA_PORT PORTC
#define SDA_PIN 4 // = A4 on Arduino UNO
#define SCL_PORT PORTC
#define SCL_PIN 5 // = A5 on Arduino UNO

#include "SH1106Lib.h"
SH1106Lib display;

void setup() {
    display.initialize();
    display.clearDisplay();
}

void loop() {
    // use display.
}
```

## CONFIG
The bare minimum configuration you have to do is telling the SoftI2CMaster what pins to use:
```c++
#define SDA_PORT PORTC
#define SDA_PIN 4
#define SCL_PORT PORTC
#define SCL_PIN 5
```

To confugre the underlying two other libraries, check their readmes:
 - [SoftI2CMaster Configuration](https://github.com/felias-fogg/SoftI2CMaster#configuration)
 - [TinyPrint Configuration](https://github.com/notisrac/TinyPrint#configuration)


## FUNCTIONS
### Draw a pixel
```c++
// displays a white pixel at 10, 10
display.drawPixel(10, 10, WHITE);
```

### Draw a filled rectangle
```c++
// draws a white filled rectangle at 5, 5, with a width of 10 and height of 3
display.fillRect(5, 5, 10, 3, WHITE); 
```

### Print text
```c++
// need to include the font itself
#include "glcdfont.h"

...

// first we tell the lib the font and its properties, that we want to use
display.setFont(font, 5, 7);
// OR
// display.setFont(font_3x5, FONT_3x5_WIDTH, FONT_3x5_HEIGHT, 45, FONT_NUMBERS | FONT_UPPERCASECHARS);
// set whether long text should wrap to the next line upon reaching the end of the display
display.setTextWrap(true);
// set the text front and background color
display.setTextColor(WHITE, TRANSPARENT);
// set where we want to print the text to
display.setCursor(0, 0);
// display the text, using the standard Arduino print methods
display.print(F("Hello world!"));
```

### Display a bitmap
```c++
// need to include the image itself
// created with: https://notisrac.github.io/FileToCArray/
#define BATTERY_HEIGHT 5
#define BATTERY_WIDTH 11

// array size is 11
static const byte battery[] PROGMEM  = {
  B00011111, B00010001, 
  B00010001, B00010001, 
  B00010001, B00010001, 
  B00010001, B00010001, 
  B00011111, B00001010, 
  B00001110
};

...

// display the bitmap at 5, 3, in white, with transparent background
display.drawBitmap(5, 3, battery, BATTERY_WIDTH, BATTERY_HEIGHT, WHITE, TRANSPARENT);
```


## Converting fonts and bitmaps
To be able to use your fonts/bitmaps with this library, you need to convert it to a specified format.
For this you can use the [File to C style array converter](https://notisrac.github.io/FileToCArray/) like this:
 1. Load the image into the converter ("Browse...", select image, "Open")
 1. Set the code format to your liking
 1. Set the palette mod to 1bit line art
 1. Tick the Optimize for column read checkbox
 1. Click "Convert"
 1. Click "Save as file" OR Click "Copy to clipboard"


## CONSIDERATIONS
 * Much slower than a bufferd lib like the [U8g2](https://github.com/olikraus/u8g2)
 * Printing text that don't span the page boundary (aka y % 8 == 0) lines are about 50% faster
 * Displaying an image that don't span the page boundary (aka y % 8 == 0) lines are about 50% faster
 * Plotting individual pixels in large numbers is slow
 * If constanty updating a text/image on screen, if the design allows it, set the background to SOLID. This is much faster, than clearing the area with fillRect
 * Keep in mind, that enabling functions, and adding your own code can increase the size of the binaries pretty fast. So if you can, choose a uc with at least 8k of program space, just to be on the safe side.

_Note: all measurements are made on an Arduino UNO with I2C_FASTMODE on, I2C_HARDWARE, I2C_TIMEOUT = 10 and I2C_MAXWAIT = 10_

## Comparison
| Library                              | Prog storage (bytes) | Memory (bytes) | Device support               | Comment |
|:-------------------------------------|:--------------------:|:--------------:|------------------------------|---------|
| U8g2 Page buffer mode (Picture Loop) | 9156                 | 581            | Almost all of them           | Empty example sketch, with lib and init |
| Adafruit_SH1106                      | 8726                 | 1299           | SH1106 OLED display only | Empty example sketch, init code. This is the Adafruit SSD1306 lib converted to SH1106 |
| SH1106LIB (this lib)                 | 2312                 | 33             | SH1106 OLED display only | Empty example sketch, init code, HW I2C support off, tinyprint off |


## SUPPORTED DEVICES
Below is a list or AVRs that meet the criteria of running this library - based on their flash and the sram sizes! (4 < flash <= 32 or 64 < sram <= 1024)
_Please see the "Tested" column, whether a uc has been tested with the lib or not!_
| Product     | Program Mem (KB) | SRAM (Bytes) | HW I2C | Tested | Comment |
|:------------|-----------------:|-------------:|:------:|:------:|---------|
| [ATmega328](https://www.microchip.com/wwwproducts/en/ATmega328) | 32 | 2048 | 1 | YES | Arduino UNO, I2C_HARDWARE on/off, I2C_FASTMODE on/off | 
| [ATmega16](https://www.microchip.com/wwwproducts/en/ATmega16) | 16 | 1024 | 1 | no |  |
| [ATmega164](https://www.microchip.com/wwwproducts/en/ATmega164A) | 16 | 1024 | 1 | no |  | 
| [ATmega168](https://www.microchip.com/wwwproducts/en/ATmega168) | 16 | 1024 | 1 | no |  |
| [ATmega169](https://www.microchip.com/wwwproducts/en/ATmega169A) | 16 | 1024 | 1 | no |  |
| [ATmega16A](https://www.microchip.com/wwwproducts/en/ATmega16A) | 16 | 1024 | 1 | no |  |
| [ATmega16M1](https://www.microchip.com/wwwproducts/en/ATmega16M1) | 16 | 1024 | 0 | no |  |
| [ATmega16U2](https://www.microchip.com/wwwproducts/en/ATmega16U2) | 16 | 512 | 0 | no |  |
| [ATmega16U4](https://www.microchip.com/wwwproducts/en/ATmega16U4) | 16 | 1256 | 1 | no |  |
| [ATmega32U2](https://www.microchip.com/wwwproducts/en/ATmega32U2) | 32 | 1024 | 0 | no |  |
| [ATmega48](https://www.microchip.com/wwwproducts/en/ATmega48) | 4 | 512 | 1 | no |  |
| [ATmega48](https://www.microchip.com/wwwproducts/en/ATmega48A) | 4 | 512 | 1 | no |  |
| [ATmega8](https://www.microchip.com/wwwproducts/en/ATmega8) | 8 | 1024 | 1 | no |  |
| [ATmega88](https://www.microchip.com/wwwproducts/en/ATmega88) | 8 | 1024 | 1 | no |  |
| [ATmega8A](https://www.microchip.com/wwwproducts/en/ATmega8A) | 8 | 1024 | 1 | no |  |
| [ATmega8U2](https://www.microchip.com/wwwproducts/en/ATmega8U2) | 8 | 512 | 0 | no |  |
| [ATTINY1607](https://www.microchip.com/wwwproducts/en/ATTINY1607) | 16 | 1024 | 1 | no |  |
| [ATtiny1634](https://www.microchip.com/wwwproducts/en/ATtiny1634) | 16 | 1024 | 1 | no |  |
| [ATtiny167](https://www.microchip.com/wwwproducts/en/ATtiny167) | 16 | 512 | 1 | no |  |
| [ATtiny40](https://www.microchip.com/wwwproducts/en/ATtiny40) | 4 | 256 | 1 | no |  |
| [ATTINY402](https://www.microchip.com/wwwproducts/en/ATTINY402) | 4 | 256 | 1 | no |  |
| [ATTINY404](https://www.microchip.com/wwwproducts/en/ATTINY404) | 4 | 256 | 1 | no |  |
| [ATTINY406](https://www.microchip.com/wwwproducts/en/ATTINY406) | 4 | 256 | 1 | no |  |
| [ATTINY412](https://www.microchip.com/wwwproducts/en/ATTINY412) | 4 | 256 | 1 | no |  |
| [ATTINY414](https://www.microchip.com/wwwproducts/en/ATTINY414) | 4 | 256 | 1 | no |  |
| [ATTINY416](https://www.microchip.com/wwwproducts/en/ATTINY416) | 4 | 256 | 1 | no |  |
| [ATTINY417](https://www.microchip.com/wwwproducts/en/ATTINY417) | 4 | 256 | 1 | no |  |
| [ATtiny4313](https://www.microchip.com/wwwproducts/en/ATtiny4313) | 4 | 256 | 1 | no |  |
| [ATtiny43U](https://www.microchip.com/wwwproducts/en/ATtiny43U) | 4 | 256 | 1 | no |  |
| [ATtiny44](https://www.microchip.com/wwwproducts/en/ATtiny44) | 4 | 256 | 1 | no |  |
| [ATtiny441](https://www.microchip.com/wwwproducts/en/ATtiny441) | 4 | 256 | 1 | no |  |
| [ATtiny44A](https://www.microchip.com/wwwproducts/en/ATtiny44A) | 4 | 256 | 1 | no |  |
| [ATtiny45](https://www.microchip.com/wwwproducts/en/ATtiny45) | 4 | 256 | 1??? | YES | I2C_HARDWARE off, works with the 1MHz internal clock too |
| [ATtiny461](https://www.microchip.com/wwwproducts/en/ATtiny461) | 4 | 256 | 1 | no |  |
| [ATtiny48](https://www.microchip.com/wwwproducts/en/ATtiny48) | 4 | 256 | 1 | no |  |
| [ATTINY807](https://www.microchip.com/wwwproducts/en/ATTINY807) | 8 | 512 | 1 | no |  |
| [ATTINY814](https://www.microchip.com/wwwproducts/en/ATTINY814) | 8 | 256 | 1 | no |  |
| [ATTINY816](https://www.microchip.com/wwwproducts/en/ATTINY816) | 8 | 512 | 1 | no |  |
| [ATTINY817](https://www.microchip.com/wwwproducts/en/ATTINY817) | 8 | 512 | 1 | no |  |
| [ATtiny828](https://www.microchip.com/wwwproducts/en/ATtiny828) | 8 | 512 | 1 | no |  |
| [ATtiny84](https://www.microchip.com/wwwproducts/en/ATtiny84) | 8 | 512 | 1 | no |  |
| [ATtiny841](https://www.microchip.com/wwwproducts/en/ATtiny841) | 8 | 512 | 1 | no |  |
| [ATtiny84A](https://www.microchip.com/wwwproducts/en/ATtiny84A) | 8 | 512 | 1??? | YES | I2C_HARDWARE off, works with the 1MHz internal clock too |
| [ATtiny85](https://www.microchip.com/wwwproducts/en/ATtiny85) | 8 | 512 | 1 | no |  |
| [ATtiny861](https://www.microchip.com/wwwproducts/en/ATtiny861) | 8 | 512 | 1 | no |  |
| [ATtiny87](https://www.microchip.com/wwwproducts/en/ATtiny87) | 8 | 512 | 1 | no |  |
| [ATtiny88](https://www.microchip.com/wwwproducts/en/ATtiny88) | 8 | 512 | 1 | no |  |
| [ATxmega8E5](https://www.microchip.com/wwwproducts/en/ATxmega8E5) | 8 | 1024 | 1 | no |  |
 _Last updated: 2018.09.23_


## API

### `SH1106Lib()`
The constructor

### `void initialize()`
Initializes the display

### `void sendCommand(byte c)`
Sends a single command to the display
 - command: the command (byte) that needs to be sent to the device

### `void sendData(byte data)`
Sends a single byte of data to the display
 - data: the data (byte) that needs to be sent to the device

### `void clearDisplay(void)`
Clears the display

### `fillRect(uint8_t left, uint8_t top, uint8_t width, uint8_t height, uint8_t color)`
Displays a filled rectangle
 - left: x coordinate of the top left corner of the rectangle
 - top: y coordinate of the top left corner of the rectangle
 - width: width of the rectangle
 - height: height of the rectangle
 - color: color of the rectangle

### `void drawBitmap(uint8_t x, uint8_t y, const byte *bitmap, uint8_t w, uint8_t h, uint8_t color, uint8_t backgroundType)`
Draws a bitmap from the program memory to the display
 - x: the x coordinate to put the bitmap
 - y: the y coordiante to put the bitmap
 - bitmap: the byte array representing the image
 - w: the width of the image
 - h: the height of the image
 - color: the color which the image should be displayed (BLACK/WHITE)
 - backgroundType: SOLID or TRANSPARENT background

### `void drawPixel(uint8_t x, uint8_t y, uint8_t color)`
Draws a single pixel on th display
 - x: the x coordinate to put the pixel
 - y: the y coordiante to put the pixel
 - color: the color of the pixel (WHITE or BLACK)

### `void setCursor(uint8_t x, uint8_t y)`
Sets the text cursor to this position
 - x: the x coordinate
 - y: the y coordiante

### `void setFont(const unsigned char *font, uint8_t width, uint8_t height, int8_t offset = 0, uint8_t flags = FONT_FULL)`
Sets the font to write with
 - font: pinter to the array containing the font
 - width: the width of the font in pixels
 - height: the height of the font in pixels
 - offset: signed value to offset the position the character is found in the font
 - flags: set of flags describing the properties of the font

```c++
// Examples:
// this is an 5x7 full font 
display.setFont(font, 5, 7);
// this is a 10x11 font, that only contains numbers
display.setFont(font_10x11, 10, 11, 48, FONT_NUMBERS);
// this is a 3x5 font, that has numbers and upper case characters, but no lowercase or space character
display.setFont(font_3x5, 3, 5, 45, FONT_NUMBERS | FONT_UPPERCASECHARS);
```

### `void setTextWrap(bool enableWrap)`
Sets whether the text should continue on the next row, if it has reached the end of the current one
 - enableWrap: true to enable, false to disable

### `void setTextColor(uint8_t color, uint8_t backgroundType)`
Sets whether the text should continue on the next row, if it has reached the end of the current one
 - color: the color of the text
 - backColor: the color of the background of the text. Set it to TRANSPARENT to have a transparend background

### `virtual byte write(uint8_t) override`
_Inherited via TinyPrint_
Displays a single character
 - c: the character to display
 - returns: 1

### `void drawChar(uint8_t x, uint8_t y, uint8_t character, uint8_t color, uint8_t backgroundType)`
Draws a character on the screen from the font
 - x: x coordinate where the character should be displayed
 - y: y coordinate where the character should be displayed
 - character: The character to display
 - color: the color of the character
 - backgroundType: SOLID or TRANSPARENT


### Color types
 - BLACK
 - WHITE

### Background types
 - SOLID
 - TRANSPARENT

### Font flags
 - FONT_NUMBERS: the font has numbers
 - FONT_UPPERCASECHARS: the font has upper case characters
 - FONT_LOWERCASECHARS: the font has lower case characters
 - FONT_HASSPACE: the font has a separate character for the space (' ')
 - FONT_FULL: a full font (this has all of the above)

```c++
// Examples:
// this is a font, that only contains numbers
FONT_NUMBERS
// this is a font, that has numbers and upper case characters, but no lowercase or space character
FONT_NUMBERS | FONT_UPPERCASECHARS
```


## LINKS
 * https://playground.arduino.cc/Main/SoftwareI2CLibrary
 * https://github.com/notisrac/TinyPrint


## TASKS
 - [x] Write display initialization code
 - [x] Implement single pixel drawing
 - [x] Implement clearDisplay
 - [x] Implement text printing functionality
 - [x] Use [SoftwareI2CLibrary](https://playground.arduino.cc/Main/SoftwareI2CLibrary)
 - [x] Display bitmaps
 - [ ] Implement drawLine
 - [ ] Implement drawCircle
 - [ ] Implement drawRect
 - [x] Put the whole thing into a library format
 - [x] Write example codes for lib
 - [x] Figure out how the Read Modify Write (RMW) mode works
 - [x] Implement fillRect with RMW mode
 - [x] Change drawChar to use RMW mode
 - [x] Change drawBitmap to use RMW mode
 - [ ] Do measurements on different devices


[![Analytics](https://ga-beacon.appspot.com/UA-122950438-1/SH1106Lib)](https://github.com/igrigorik/ga-beacon)
