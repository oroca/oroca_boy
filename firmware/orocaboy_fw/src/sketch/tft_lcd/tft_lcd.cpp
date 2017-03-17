#include "Arduino.h"


#if 0
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Waveshare_HX8347D.h"




Waveshare_HX8347D tft = Waveshare_HX8347D();




void setup() {
  uint32_t t_time;

  Serial.begin(115200);

  tft.begin();




  tft.setRotation(1);

  tft.fillScreen(HX8347D_BLACK);
}


void loop(void) {
  static int x[8], y[8];
  static int x_dir[8] = {0,};
  static int x_width[8];
  static int x_step[8];
  static int x_color[8];

  uint32_t t_time;
  int i;


  x_width[0] = 30;  y[0] =       0;  x_step[0] = 1; x_color[0] = HX8347D_RED;
  x_width[1] = 30;  y[1] =      32;  x_step[1] = 2; x_color[1] = HX8347D_BLUE;
  x_width[2] = 30;  y[2] = y[1]+32;  x_step[2] = 3; x_color[2] = HX8347D_GREEN;
  x_width[3] = 30;  y[3] = y[2]+32;  x_step[3] = 4; x_color[3] = HX8347D_CYAN;
  x_width[4] = 30;  y[4] = y[3]+32;  x_step[4] = 5; x_color[4] = HX8347D_MAGENTA;
  x_width[5] = 30;  y[5] = y[4]+32;  x_step[5] = 6; x_color[5] = HX8347D_YELLOW;
  x_width[6] = 30;  y[6] = y[5]+32;  x_step[6] = 7; x_color[6] = HX8347D_WHITE;


  t_time = millis();


  for(i=0; i<7; i++)
  {
    tft.fillRect(x[i], y[i], x_width[i], x_width[i], x_color[i]);
  }
  tft.drawFrame();

  for(i=0; i<7; i++)
  {
    tft.fillRect(x[i], y[i], x_width[i], x_width[i], HX8347D_BLACK);

    if( x_dir[i] == 0 )
      x[i] += x_step[i];
    else
      x[i] -= x_step[i];

    if(x[i]>240-x_width[i])
    {
      x_dir[i] ^= 1;
      x[i] = 240-x_width[i];
    }

    if(x[i]<0)
    {
      x_dir[i] ^= 1;
      x[i] = 0;
    }
  }

  uint32_t process_time;

  process_time = millis() - t_time;


  tft.setCursor(0, 260);
  tft.setTextColor(HX8347D_GREEN, HX8347D_BLACK);
  tft.setTextSize(3);
  tft.println((process_time) + String(" ms "));

  tft.setCursor(0, 300);
  tft.println(1000/(process_time) + String(" FPS  "));
}


#elif 0
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Waveshare_HX8347D.h"




Waveshare_HX8347D tft = Waveshare_HX8347D();


unsigned long testText();
unsigned long testTriangles();
unsigned long testLines(uint16_t color);
unsigned long testRects(uint16_t color);
unsigned long testCircles(uint8_t radius, uint16_t color);
unsigned long testFilledTriangles();
unsigned long testRoundRects();
unsigned long testFilledRoundRects();



void setup() {
  uint32_t t_time;

  Serial.begin(115200);
  while(!Serial);
  Serial.println("HX8347D Test!");

  tft.begin();



  Serial.println(F("Benchmark                Time (microseconds)"));

  tft.setRotation(1);

  t_time = micros();
  tft.fillScreen(HX8347D_RED);
  tft.drawFrame();
  Serial.println(micros()-t_time);


  delay(500);
  tft.fillScreen(HX8347D_GREEN);
  tft.drawFrame();
  delay(500);
  tft.fillScreen(HX8347D_BLUE);
  tft.drawFrame();
  delay(500);
  tft.fillScreen(HX8347D_BLACK);
  tft.drawFrame();
  delay(500);


  Serial.print(F("Text                     "));
  Serial.println(testText());
  delay(500);

  Serial.print(F("Lines                    "));
  Serial.println(testLines(HX8347D_CYAN));
  delay(500);

  Serial.print(F("Rectangles (outline)     "));
  Serial.println(testRects(HX8347D_GREEN));
  delay(500);

  tft.fillScreen(HX8347D_BLACK);
  Serial.print(F("Circles (outline)        "));
  Serial.println(testCircles(10, HX8347D_RED));
  delay(500);


  Serial.print(F("Triangles (outline)      "));
  Serial.println(testTriangles());
  delay(500);

  Serial.print(F("Triangles (filled)       "));
  Serial.println(testFilledTriangles());
  delay(500);


  Serial.print(F("Rounded rects (outline)  "));
  Serial.println(testRoundRects());
  delay(500);

  Serial.print(F("Rounded rects (filled)   "));
  Serial.println(testFilledRoundRects());
  delay(500);

  Serial.println(F("Done!"));
}


void loop(void) {
  for(uint8_t rotation=0; rotation<4; rotation++) {
    tft.setRotation(rotation);
    testText();
    delay(1000);
  }
}

unsigned long testFillScreen() {
  unsigned long start = micros();
  tft.fillScreen(HX8347D_RED);
  tft.drawFrame();
  tft.fillScreen(HX8347D_GREEN);
  tft.drawFrame();
  tft.fillScreen(HX8347D_BLUE);
  tft.drawFrame();
  tft.fillScreen(HX8347D_WHITE);
  tft.drawFrame();
  return micros() - start;
}


unsigned long testText() {
  tft.fillScreen(HX8347D_BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(HX8347D_WHITE);  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(HX8347D_YELLOW); tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(HX8347D_RED);    tft.setTextSize(3);
  tft.println(0xDEADBEEF, HEX);
  tft.println();
  tft.setTextColor(HX8347D_GREEN);
  tft.setTextSize(5);
  tft.println("Groop");
  tft.setTextSize(2);
  tft.println("I implore thee,");
  tft.setTextSize(1);
  tft.println("my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles,");
  tft.println("Or I will rend thee");
  tft.println("in the gobberwarts");
  tft.println("with my blurglecruncheon,");
  tft.println("see if I don't!");

  tft.setTextColor(HX8347D_WHITE);
  tft.println(F("Alice was beginning to get very tired of sitting by her sister on the bank, and of having nothing to do: once or twice she had peeped into the book her sister was reading, but it had no pictures or conversations in it, 'and what is the use of a book,' thought Alice 'without pictures or conversations?'"));

tft.println(F("So she was considering in her own mind (as well as she could, for the hot day made her feel very sleepy and stupid), whether the pleasure of making a daisy-chain would be worth the trouble of getting up and picking the daisies, when suddenly a White Rabbit with pink eyes ran close by her."));

tft.println(F("There was nothing so very remarkable in that; nor did Alice think it so very much out of the way to hear the Rabbit say to itself, 'Oh dear! Oh dear! I shall be late!' (when she thought it over afterwards, it occurred to her that she ought to have wondered at this, but at the time it all seemed quite natural); but when the Rabbit actually took a watch out of its waistcoat-pocket, and looked at it, and then hurried on, Alice started to her feet, for it flashed across her mind that she had never before seen a rabbit with either a waistcoat-pocket, or a watch to take out of it, and burning with curiosity, she ran across the field after it, and fortunately was just in time to see it pop down a large rabbit-hole under the hedge."));

tft.println(F("In another moment down went Alice after it, never once considering how in the world she was to get out again."));

tft.println(F("The rabbit-hole went straight on like a tunnel for some way, and then dipped suddenly down, so suddenly that Alice had not a moment to think about stopping herself before she found herself falling down a very deep well."));

tft.println(F("Either the well was very deep, or she fell very slowly, for she had plenty of time as she went down to look about her and to wonder what was going to happen next. First, she tried to look down and make out what she was coming to, but it was too dark to see anything; then she looked at the sides of the well, and noticed that they were filled with cupboards and book-shelves; here and there she saw maps and pictures hung upon pegs. She took down a jar from one of the shelves as she passed; it was labelled 'ORANGE MARMALADE', but to her great disappointment it was empty: she did not like to drop the jar for fear of killing somebody, so managed to put it into one of the cupboards as she fell past it."));

  tft.drawFrame();
  return micros() - start;
}

unsigned long testLines(uint16_t color) {
  unsigned long start, t;
  int           x1, y1, x2, y2,
                w = tft.width(),
                h = tft.height();

  tft.fillScreen(HX8347D_BLACK);

  x1 = y1 = 0;
  y2    = h - 1;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t     = micros() - start; // fillScreen doesn't count against timing


  tft.drawFrame();
  return micros() - start;
}

unsigned long testFastLines(uint16_t color1, uint16_t color2) {
  unsigned long start;
  int           x, y, w = tft.width(), h = tft.height();

  tft.fillScreen(HX8347D_BLACK);
  start = micros();
  for(y=0; y<h; y+=5) tft.drawFastHLine(0, y, w, color1);
  for(x=0; x<w; x+=5) tft.drawFastVLine(x, 0, h, color2);

  tft.drawFrame();
  return micros() - start;
}

unsigned long testRects(uint16_t color) {
  unsigned long start;
  int           n, i, i2,
                cx = tft.width()  / 2,
                cy = tft.height() / 2;

  tft.fillScreen(HX8347D_BLACK);
  n     = min(tft.width(), tft.height());
  start = micros();
  for(i=2; i<n; i+=6) {
    i2 = i / 2;
    tft.drawRect(cx-i2, cy-i2, i, i, color);
  }

  tft.drawFrame();
  return micros() - start;
}

unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
  unsigned long start, t = 0;
  int           n, i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(HX8347D_BLACK);
  n = min(tft.width(), tft.height());
  for(i=n; i>0; i-=6) {
    i2    = i / 2;
    start = micros();
    tft.fillRect(cx-i2, cy-i2, i, i, color1);
    t    += micros() - start;
    // Outlines are not included in timing results
    tft.drawRect(cx-i2, cy-i2, i, i, color2);
  }

  tft.drawFrame();
  return t;
}

unsigned long testFilledCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;

  tft.fillScreen(HX8347D_BLACK);
  start = micros();
  for(x=radius; x<w; x+=r2) {
    for(y=radius; y<h; y+=r2) {
      tft.fillCircle(x, y, radius, color);
    }
  }

  tft.drawFrame();
  return micros() - start;
}

unsigned long testCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int           x, y, r2 = radius * 2,
                w = tft.width()  + radius,
                h = tft.height() + radius;

  // Screen is not cleared for this one -- this is
  // intentional and does not affect the reported time.
  start = micros();
  for(x=0; x<w; x+=r2) {
    for(y=0; y<h; y+=r2) {
      tft.drawCircle(x, y, radius, color);
    }
  }

  tft.drawFrame();
  return micros() - start;
}

unsigned long testTriangles() {
  unsigned long start;
  int           n, i, cx = tft.width()  / 2 - 1,
                      cy = tft.height() / 2 - 1;

  tft.fillScreen(HX8347D_BLACK);
  n     = min(cx, cy);
  start = micros();
  for(i=0; i<n; i+=5) {
    tft.drawTriangle(
      cx    , cy - i, // peak
      cx - i, cy + i, // bottom left
      cx + i, cy + i, // bottom right
      tft.color565(200, 20, i));
  }

  tft.drawFrame();
  return micros() - start;
}

unsigned long testFilledTriangles() {
  unsigned long start, t = 0;
  int           i, cx = tft.width()  / 2 - 1,
                   cy = tft.height() / 2 - 1;

  tft.fillScreen(HX8347D_BLACK);
  start = micros();
  for(i=min(cx,cy); i>10; i-=5) {
    start = micros();
    tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      tft.color565(0, i, i));
    t += micros() - start;
    tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      tft.color565(i, i, 0));
  }

  tft.drawFrame();
  return t;
}

unsigned long testRoundRects() {
  unsigned long start;
  int           w, i, i2,
                cx = tft.width()  / 2 ,
                cy = tft.height() / 2 ;

  tft.fillScreen(HX8347D_BLACK);
  w     = min(tft.width(), tft.height());
  start = micros();
  for(i=0; i<w; i+=8) {
    i2 = i / 2 - 2;
    tft.drawRoundRect(cx-i2, cy-i2, i, i, i/8, tft.color565(i, 100, 100));
  }

  tft.drawFrame();
  return micros() - start;
}

unsigned long testFilledRoundRects() {
  unsigned long start;
  int           i, i2,
                cx = tft.width()  / 2 + 10,
                cy = tft.height() / 2 + 10;

  tft.fillScreen(HX8347D_BLACK);
  start = micros();
  for(i=min(tft.width(), tft.height()) - 20; i>20; i-=6) {
    i2 = i / 2;
    tft.fillRoundRect(cx-i2, cy-i2, i-20, i-20, i/8, tft.color565(100, i/2, 100));
  }

  tft.drawFrame();
  return micros() - start;
}
#elif 0
#include "Arduino.h"


#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>


// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     9
#define TFT_RST    6   // you can also connect this to the Arduino reset
                       // in which case, set this #define pin to 0!
#define TFT_DC     10

// Option 1 (recommended): must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

// Option 2: use any pins but a little slower!
#define TFT_SCLK 13   // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);


void testdrawtext(char *text, uint16_t color);
void tftPrintTest();
void testlines(uint16_t color);
void testfastlines(uint16_t color1, uint16_t color2);
void testdrawrects(uint16_t color);
void testfillrects(uint16_t color1, uint16_t color2);
void testfillcircles(uint8_t radius, uint16_t color);
void testdrawcircles(uint8_t radius, uint16_t color);
void testroundrects();
void testtriangles();
void mediabuttons();

float p = 3.1415926;



void frameISR(void)
{
  tft.drawFrame(false);
}





void setup(void) {
  Serial.begin(115200);
  Serial.print("Hello! ST7735 TFT Test");

  // Use this initializer if you're using a 1.8" TFT
  //tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab

  // Use this initializer (uncomment) if you're using a 1.44" TFT
  tft.begin();   // initialize a ST7735S chip, black tab

  Serial.println("Initialized");

  tft.startFrame(frameISR);

  uint32_t t_time = micros();
  uint32_t time = millis();
  tft.fillScreen(ST7735_BLACK);
  //tft.drawFrame();
  tft.fillScreen(ST7735_BLUE);
  //tft.drawFrame();
  time = millis() - time;
  t_time = micros() - t_time;
  Serial.println(time, DEC);
  Serial.println(t_time, DEC);

  delay(500);

  // large block of text
  tft.fillScreen(ST7735_BLACK);
  testdrawtext((char *)"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", ST7735_WHITE);
  //tft.drawFrame();
  delay(1000);

  // tft print function!
  tftPrintTest();
  delay(4000);

  // a single pixel
  tft.drawPixel(tft.width()/2, tft.height()/2, ST7735_GREEN);
  delay(500);

  // line draw test
  testlines(ST7735_YELLOW);
  delay(500);

  // optimized lines
  testfastlines(ST7735_RED, ST7735_BLUE);
  delay(500);

  testdrawrects(ST7735_GREEN);
  delay(500);

  testfillrects(ST7735_YELLOW, ST7735_MAGENTA);
  delay(500);

  tft.fillScreen(ST7735_BLACK);
  testfillcircles(10, ST7735_BLUE);
  testdrawcircles(10, ST7735_WHITE);
  delay(500);

  testroundrects();
  delay(500);

  testtriangles();
  delay(500);

  mediabuttons();
  delay(500);

  Serial.println("done");
  delay(1000);
}

void loop() {
  tft.invertDisplay(true);
  delay(500);
  tft.invertDisplay(false);
  delay(500);
}

void testlines(uint16_t color) {
  tft.fillScreen(ST7735_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, 0, x, tft.height()-1, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, 0, tft.width()-1, y, color);
  }

  tft.fillScreen(ST7735_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, 0, x, tft.height()-1, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, 0, 0, y, color);
  }

  tft.fillScreen(ST7735_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(0, tft.height()-1, x, 0, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(0, tft.height()-1, tft.width()-1, y, color);
  }

  tft.fillScreen(ST7735_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, x, 0, color);
  }
  for (int16_t y=0; y < tft.height(); y+=6) {
    tft.drawLine(tft.width()-1, tft.height()-1, 0, y, color);
  }
}

void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void testfastlines(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST7735_BLACK);
  for (int16_t y=0; y < tft.height(); y+=5) {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (int16_t x=0; x < tft.width(); x+=5) {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}

void testdrawrects(uint16_t color) {
  tft.fillScreen(ST7735_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color);
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST7735_BLACK);
  for (int16_t x=tft.width()-1; x > 6; x-=6) {
    tft.fillRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color1);
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color2);
  }
}

void testfillcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=radius; x < tft.width(); x+=radius*2) {
    for (int16_t y=radius; y < tft.height(); y+=radius*2) {
      tft.fillCircle(x, y, radius, color);
    }
  }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  for (int16_t x=0; x < tft.width()+radius; x+=radius*2) {
    for (int16_t y=0; y < tft.height()+radius; y+=radius*2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
}

void testtriangles() {
  tft.fillScreen(ST7735_BLACK);
  int color = 0xF800;
  int t;
  int w = tft.width()/2;
  int x = tft.height()-1;
  int y = 0;
  int z = tft.width();
  for(t = 0 ; t <= 15; t+=1) {
    tft.drawTriangle(w, y, y, x, z, x, color);
    x-=4;
    y+=4;
    z-=4;
    color+=100;
  }
}

void testroundrects() {
  tft.fillScreen(ST7735_BLACK);
  int color = 100;
  int i;
  int t;
  for(t = 0 ; t <= 4; t+=1) {
    int x = 0;
    int y = 0;
    int w = tft.width()-2;
    int h = tft.height()-2;
    for(i = 0 ; i <= 16; i+=1) {
      tft.drawRoundRect(x, y, w, h, 5, color);
      x+=2;
      y+=3;
      w-=4;
      h-=6;
      color+=1100;
    }
    color+=100;
  }
}

void tftPrintTest() {
  tft.setTextWrap(false);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(ST7735_RED);
  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ST7735_YELLOW);
  tft.setTextSize(2);
  tft.println("Hello World!");
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(3);
  tft.println("Hello World!");
  tft.setTextColor(ST7735_BLUE);
  tft.setTextSize(4);
  tft.print(1234.567);
  delay(1500);
  tft.setCursor(0, 0);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(0);
  tft.println("Hello World!");
  tft.setTextSize(1);
  tft.setTextColor(ST7735_GREEN);
  tft.print(p, 6);
  tft.println(" Want pi?");
  tft.println(" ");
  tft.print(8675309, HEX); // print 8,675,309 out in HEX!
  tft.println(" Print HEX!");
  tft.println(" ");
  tft.setTextColor(ST7735_WHITE);
  tft.println("Sketch has been");
  tft.println("running for: ");
  tft.setTextColor(ST7735_MAGENTA);
  tft.print(millis() / 1000);
  tft.setTextColor(ST7735_WHITE);
  tft.print(" seconds.");
}

void mediabuttons() {
  // play
  tft.fillScreen(ST7735_BLACK);
  tft.fillRoundRect(25, 10, 78, 60, 8, ST7735_WHITE);
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST7735_RED);
  delay(500);
  // pause
  tft.fillRoundRect(25, 90, 78, 60, 8, ST7735_WHITE);
  tft.fillRoundRect(39, 98, 20, 45, 5, ST7735_GREEN);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST7735_GREEN);
  delay(500);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST7735_BLUE);
  delay(50);
  // pause color
  tft.fillRoundRect(39, 98, 20, 45, 5, ST7735_RED);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST7735_RED);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST7735_GREEN);
}
#else
#include "Arduino.h"
#include <EEPROM.h>
#include <SPI.h>
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




#define TFT_CS     9
#define TFT_RST    6
#define TFT_DC     10

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);



void setup() {
  uint32_t t_time;

  Serial.begin(115200);

  tft.begin();




  tft.setRotation(1);

  tft.fillScreen(_BLACK);
  tft.drawFrame();

  //EEPROM.write(0, 199);
}


void loop(void) {
  static int x[8], y[8];
  static int x_dir[8] = {0,};
  static int x_width[8];
  static int x_step[8];
  static int x_color[8];
  static int cnt = 0;
  static int radius = 0;
  static int radius_dir = 0;

  int block_size = 10;
  int block_gap  = 12;
  uint32_t t_time;
  int i;


  x_width[0] = block_size;  y[0] =              0;  x_step[0] = 1; x_color[0] = _RED;
  x_width[1] = block_size;  y[1] =      block_gap;  x_step[1] = 2; x_color[1] = _BLUE;
  x_width[2] = block_size;  y[2] = y[1]+block_gap;  x_step[2] = 3; x_color[2] = _GREEN;
  x_width[3] = block_size;  y[3] = y[2]+block_gap;  x_step[3] = 4; x_color[3] = _CYAN;
  x_width[4] = block_size;  y[4] = y[3]+block_gap;  x_step[4] = 5; x_color[4] = _MAGENTA;
  x_width[5] = block_size;  y[5] = y[4]+block_gap;  x_step[5] = 6; x_color[5] = _YELLOW;
  x_width[6] = block_size;  y[6] = y[5]+block_gap;  x_step[6] = 7; x_color[6] = _WHITE;


  t_time = micros();


  for(i=0; i<7; i++)
  {
    tft.fillRect(x[i], y[i], x_width[i], x_width[i], x_color[i]);
  }


  tft.fillCircle(60, 110, radius, _GREEN);
  tft.fillCircle(100, 110, 20-radius, _BLUE);
  tft.drawFrame();

  tft.fillCircle(60, 110, radius, _BLACK);
  tft.fillCircle(100, 110, 20-radius, _BLACK);

  if (radius_dir)
  {
    radius--;
    if (radius == 0) radius_dir ^= 1;
  }
  else
  {
    radius++;
    if (radius >= 20) radius_dir ^= 1;
  }

  for(i=0; i<7; i++)
  {
    tft.fillRect(x[i], y[i], x_width[i], x_width[i], _BLACK);

    if( x_dir[i] == 0 )
      x[i] += x_step[i];
    else
      x[i] -= x_step[i];

    if(x[i]>(tft.width())-x_width[i])
    {
      x_dir[i] ^= 1;
      x[i] = (tft.width())-x_width[i];
    }

    if(x[i]<0)
    {
      x_dir[i] ^= 1;
      x[i] = 0;
    }
  }

  uint32_t process_time;

  delay(20);

  process_time = micros() - t_time;




  tft.setCursor(0, 100);
  tft.setTextColor(_GREEN, _BLACK);
  tft.setTextSize(1);
  tft.println((process_time/1000) + String(" ms "));

  tft.setCursor(0, 110);
  tft.println(1000/(process_time/1000) + String(" FPS  "));

  tft.println(cnt++);

  Serial.println(EEPROM.read(0));
}

#endif
