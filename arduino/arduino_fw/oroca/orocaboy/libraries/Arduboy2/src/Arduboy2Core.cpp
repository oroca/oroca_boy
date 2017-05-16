/**
 * @file Arduboy2Core.cpp
 * \brief
 * The Arduboy2Core class for Arduboy hardware initilization and control.
 */

#include "Arduboy2Core.h"

#include "Adafruit_GFX.h"
#include <Adafruit_ST7735.h>



#define _BLACK   ST7735_BLACK
#define _BLUE    ST7735_BLUE
#define _RED     ST7735_RED
#define _GREEN   ST7735_GREEN
#define _CYAN    ST7735_CYAN
#define _MAGENTA ST7735_MAGENTA
#define _YELLOW  ST7735_YELLOW
#define _WHITE   ST7735_WHITE


extern Adafruit_ST7735 tft;




// need to redeclare these here since we declare them static in .h
volatile uint8_t *Arduboy2Core::csport, *Arduboy2Core::dcport;
uint8_t Arduboy2Core::cspinmask, Arduboy2Core::dcpinmask;

const uint8_t PROGMEM pinBootProgram[] = {
  // buttons
  PIN_LEFT_BUTTON, INPUT_PULLUP,
  PIN_RIGHT_BUTTON, INPUT_PULLUP,
  PIN_UP_BUTTON, INPUT_PULLUP,
  PIN_DOWN_BUTTON, INPUT_PULLUP,
  PIN_A_BUTTON, INPUT_PULLUP,
  PIN_B_BUTTON, INPUT_PULLUP,

  // audio is specifically not included here as those pins are handled
  // separately by `audio.begin()`, `audio.on()` and `audio.off()` in order
  // to respect the EEPROM audio settings

  0
};

const uint8_t PROGMEM lcdBootProgram[] = {
  // boot defaults are commented out but left here in case they
  // might prove useful for reference
  //
  // Further reading: https://www.adafruit.com/datasheets/SSD1306.pdf
  //
  // Display Off
  // 0xAE,

  // Set Display Clock Divisor v = 0xF0
  // default is 0x80
  0xD5, 0xF0,

  // Set Multiplex Ratio v = 0x3F
  // 0xA8, 0x3F,

  // Set Display Offset v = 0
  // 0xD3, 0x00,

  // Set Start Line (0)
  // 0x40,

  // Charge Pump Setting v = enable (0x14)
  // default is disabled
  0x8D, 0x14,

  // Set Segment Re-map (A0) | (b0001)
  // default is (b0000)
  0xA1,

  // Set COM Output Scan Direction
  0xC8,

  // Set COM Pins v
  // 0xDA, 0x12,

  // Set Contrast v = 0xCF
  0x81, 0xCF,

  // Set Precharge = 0xF1
  0xD9, 0xF1,

  // Set VCom Detect
  // 0xDB, 0x40,

  // Entire Display ON
  // 0xA4,

  // Set normal/inverse display
  // 0xA6,

  // Display On
  0xAF,

  // set display mode = horizontal addressing mode (0x00)
  0x20, 0x00,

  // set col address range
  // 0x21, 0x00, COLUMN_ADDRESS_END,

  // set page address range
  // 0x22, 0x00, PAGE_ADDRESS_END
};


Arduboy2Core::Arduboy2Core() {}

void Arduboy2Core::boot()
{

  bootPins();
  bootOLED();


  bootPowerSaving();
}

#ifdef ARDUBOY_SET_CPU_8MHZ
// If we're compiling for 8MHz we need to slow the CPU down because the
// hardware clock on the Arduboy is 16MHz.
// We also need to readjust the PLL prescaler because the Arduino USB code
// likely will have incorrectly set it for an 8MHz hardware clock.
void Arduboy2Core::setCPUSpeed8MHz()
{
  uint8_t oldSREG = SREG;
  cli();                // suspend interrupts
  PLLCSR = _BV(PINDIV); // dissable the PLL and set prescale for 16MHz)
  CLKPR = _BV(CLKPCE);  // allow reprogramming clock
  CLKPR = 1;            // set clock divisor to 2 (0b0001)
  PLLCSR = _BV(PLLE) | _BV(PINDIV); // enable the PLL (with 16MHz prescale)
  SREG = oldSREG;       // restore interrupts
}
#endif

void Arduboy2Core::bootPins()
{
  uint8_t pin, mode;
  const uint8_t *i = pinBootProgram;

  while(true) {
    pin = pgm_read_byte(i++);
    mode = pgm_read_byte(i++);
    if (pin==0) break;
    pinMode(pin, mode);
  }
}

void Arduboy2Core::bootOLED()
{
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);
  tft.drawFrame();
}

void Arduboy2Core::LCDDataMode()
{
}

void Arduboy2Core::LCDCommandMode()
{
}



void Arduboy2Core::safeMode()
{
  blank(); // too avoid random gibberish
}


/* Power Management */

void Arduboy2Core::idle()
{
}

void Arduboy2Core::bootPowerSaving()
{
}

uint8_t Arduboy2Core::width() { return WIDTH; }

uint8_t Arduboy2Core::height() { return HEIGHT; }


/* Drawing */

void Arduboy2Core::paint8Pixels(uint8_t pixels)
{
  //SPI.transfer(pixels);
}

void Arduboy2Core::paintScreen(const uint8_t *image)
{
  /*
  for (int i = 0; i < (HEIGHT*WIDTH)/8; i++)
  {
    SPI.transfer(pgm_read_byte(image + i));
  }
  */
}

// paint from a memory buffer, this should be FAST as it's likely what
// will be used by any buffer based subclass
void Arduboy2Core::paintScreen(uint8_t image[], bool clear)
{
  uint8_t c;
  int i = 0;



#if 1
  uint16_t x;
  uint16_t y;
  uint8_t pixel;
  int index = 0;


  for(x=0; x<WIDTH; x++)
  {
    for(y=0; y<HEIGHT; y++)
    {
      int yOffset = abs(y) % 8;
      int sRow = y / 8;

      if(yOffset == 0)
      {
        pixel = image[sRow*WIDTH + x];
      }

      if(pixel & (1<<yOffset))
      {
        tft.drawPixel(x , y, _WHITE);
      }
      else
      {
        tft.drawPixel(x , y, _BLACK);
      }
    }
  }
  tft.drawFrame();
#elif 0
  uint16_t x;
  uint16_t y;
  uint8_t pixel;
  int index = 0;
  static uint8_t tog = 0;

  for(x=0; x<WIDTH; x++)
  {
    for(y=0; y<HEIGHT; y++)
    {
      int yOffset = abs(y) % 8;
      int sRow = y / 8;

      if(yOffset == 0)
      {
        pixel = image[sRow*WIDTH + x];
      }

      if(pixel & (1<<yOffset))
      {
        tft.drawPixel(x , y*2 + 0, _WHITE);
        tft.drawPixel(x , y*2 + 1, _WHITE);
      }
      else
      {
        tft.drawPixel(x , y*2 + 0, _BLACK);
        tft.drawPixel(x , y*2 + 1, _BLACK);
      }
    }
  }
  if (tog == 0)
    tft.drawFrame();

  tog ^= 1;

#else
  if (clear)
  {
    SPDR = image[i]; // set the first SPI data byte to get things started
    image[i++] = 0;  // clear the first image byte
  }
  else
    SPDR = image[i++];

  // the code to iterate the loop and get the next byte from the buffer is
  // executed while the previous byte is being sent out by the SPI controller
  while (i < (HEIGHT * WIDTH) / 8)
  {
    // get the next byte. It's put in a local variable so it can be sent as
    // as soon as possible after the sending of the previous byte has completed
    if (clear)
    {
      c = image[i];
      // clear the byte in the image buffer
      image[i++] = 0;
    }
    else
      c = image[i++];

    while (!(SPSR & _BV(SPIF))) { } // wait for the previous byte to be sent

    // put the next byte in the SPI data register. The SPI controller will
    // clock it out while the loop continues and gets the next byte ready
    SPDR = c;
  }
  while (!(SPSR & _BV(SPIF))) { } // wait for the last byte to be sent
#endif
}

void Arduboy2Core::blank()
{
  //for (int i = 0; i < (HEIGHT*WIDTH)/8; i++)
  //  SPI.transfer(0x00);
  tft.fillScreen(_BLACK);
}

void Arduboy2Core::sendLCDCommand(uint8_t command)
{
}

// invert the display or set to normal
// when inverted, a pixel set to 0 will be on
void Arduboy2Core::invert(bool inverse)
{
  //sendLCDCommand(inverse ? OLED_PIXELS_INVERTED : OLED_PIXELS_NORMAL);
}

// turn all display pixels on, ignoring buffer contents
// or set to normal buffer display
void Arduboy2Core::allPixelsOn(bool on)
{
  //sendLCDCommand(on ? OLED_ALL_PIXELS_ON : OLED_PIXELS_FROM_RAM);
}

// flip the display vertically or set to normal
void Arduboy2Core::flipVertical(bool flipped)
{
  //sendLCDCommand(flipped ? OLED_VERTICAL_FLIPPED : OLED_VERTICAL_NORMAL);
}

// flip the display horizontally or set to normal
void Arduboy2Core::flipHorizontal(bool flipped)
{
  //sendLCDCommand(flipped ? OLED_HORIZ_FLIPPED : OLED_HORIZ_NORMAL);
}

/* RGB LED */

void Arduboy2Core::setRGBled(uint8_t red, uint8_t green, uint8_t blue)
{
#ifdef ARDUBOY_10 // RGB, all the pretty colors

#elif defined(AB_DEVKIT)
  // only blue on devkit
  digitalWrite(BLUE_LED, ~blue);
#endif
}

void Arduboy2Core::digitalWriteRGB(uint8_t red, uint8_t green, uint8_t blue)
{
#ifdef ARDUBOY_10
#elif defined(AB_DEVKIT)
  digitalWrite(BLUE_LED, blue);
#endif
}

/* Buttons */

uint8_t Arduboy2Core::buttonsState()
{
  uint8_t buttons;
#if 1
  buttons = 0;
  if(!digitalRead(PIN_UP_BUTTON)) buttons = UP_BUTTON;
  if(!digitalRead(PIN_DOWN_BUTTON)) buttons |= DOWN_BUTTON;
  if(!digitalRead(PIN_LEFT_BUTTON)) buttons |= LEFT_BUTTON;
  if(!digitalRead(PIN_RIGHT_BUTTON)) buttons |= RIGHT_BUTTON;
  if(!digitalRead(PIN_A_BUTTON)) buttons |= A_BUTTON;
  if(!digitalRead(PIN_B_BUTTON)) buttons |= B_BUTTON;
#else
  // up, down, left, right
  buttons = ((~(GPIOA->IDR)) & (uint32_t)(UP_BUTTON|DOWN_BUTTON|LEFT_BUTTON|RIGHT_BUTTON));
  // A(left), B(right)
  buttons |= ((~(GPIOB->IDR)) & (uint32_t)(A_BUTTON|B_BUTTON));
#endif
  return buttons;
}
