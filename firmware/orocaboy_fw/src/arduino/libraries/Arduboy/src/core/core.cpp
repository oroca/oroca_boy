

#include "core.h"

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
volatile uint8_t *ArduboyCore::mosiport, 
  *ArduboyCore::csport, *ArduboyCore::dcport;
uint8_t ArduboyCore::mosipinmask, 
  ArduboyCore::cspinmask, ArduboyCore::dcpinmask;

const uint8_t PROGMEM pinBootProgram[] = {
  // buttons
  PIN_LEFT_BUTTON, INPUT_PULLUP,
  PIN_RIGHT_BUTTON, INPUT_PULLUP,
  PIN_UP_BUTTON, INPUT_PULLUP,
  PIN_DOWN_BUTTON, INPUT_PULLUP,
  PIN_A_BUTTON, INPUT_PULLUP,
  PIN_B_BUTTON, INPUT_PULLUP,
  0
};

const uint8_t PROGMEM lcdBootProgram[] = {
  // boot defaults are commented out but left here incase they
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


ArduboyCore::ArduboyCore() {}

void ArduboyCore::boot()
{
  bootPins();
  bootLCD();
}

void ArduboyCore::bootPins()
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

void ArduboyCore::bootLCD()
{
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);
  tft.drawFrame();
}

void ArduboyCore::LCDDataMode()
{
}

void ArduboyCore::LCDCommandMode()
{
}



void ArduboyCore::safeMode()
{
}


/* Power Management */

void ArduboyCore::idle()
{
}

void ArduboyCore::saveMuchPower()
{
}

uint8_t ArduboyCore::width() { return WIDTH; }

uint8_t ArduboyCore::height() { return HEIGHT; }


/* Drawing */

void ArduboyCore::paint8Pixels(uint8_t pixels)
{
  //SPI.transfer(pixels);
}

void ArduboyCore::paintScreen(const unsigned char *image)
{

}

// paint from a memory buffer, this should be FAST as it's likely what
// will be used by any buffer based subclass
void ArduboyCore::paintScreen(unsigned char image[])
{
  int x;
  int y;
  int j;
  uint8_t pixel;
  int index = 0;

#if 1
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
#else
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
#endif
  tft.drawFrame();
}

void ArduboyCore::blank()
{
  tft.fillScreen(_BLACK);
}

void ArduboyCore::sendLCDCommand(uint8_t command)
{
}

// invert the display or set to normal
// when inverted, a pixel set to 0 will be on
void ArduboyCore::invert(boolean inverse)
{
}

// turn all display pixels on, ignoring buffer contents
// or set to normal buffer display
void ArduboyCore::allPixelsOn(boolean on)
{
}

// flip the display vertically or set to normal
void ArduboyCore::flipVertical(boolean flipped)
{
}

#define OLED_HORIZ_FLIPPED 0xA0 // reversed segment re-map
#define OLED_HORIZ_NORMAL 0xA1 // normal segment re-map

// flip the display horizontally or set to normal
void ArduboyCore::flipHorizontal(boolean flipped)
{
}

/* RGB LED */

void ArduboyCore::setRGBled(uint8_t red, uint8_t green, uint8_t blue)
{
#if 0
#ifdef ARDUBOY_10 // RGB, all the pretty colors
  // inversion is necessary because these are common annode LEDs
  analogWrite(RED_LED, 255 - red);
  analogWrite(GREEN_LED, 255 - green);
  analogWrite(BLUE_LED, 255 - blue);
#elif defined(AB_DEVKIT)
  // only blue on devkit
  digitalWrite(BLUE_LED, ~blue);
#endif
#endif
}

/* Buttons */

uint8_t ArduboyCore::getInput()
{
  return buttonsState();
}


uint8_t ArduboyCore::buttonsState()
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
