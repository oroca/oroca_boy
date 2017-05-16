/*
 * blink_led.cpp
 *
 *  Created on: 2017. 3. 11.
 *      Author: Baram
 */

#include "gfx.h"
#include "Arduino.h"
#include <SPI.h>
#include "Adafruit_GFX.h"
#include <Adafruit_ST7735.h>
#include <math.h>



//#define TEST1
#define TEST2
//#define TEST3
//#define TEST4
//#define TEST5




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


extern "C"
{
void drawFrame(void)
{
  tft.drawFrame();
}
}


void test(void);


void setup(void)
{
  coord_t   width, height;
  coord_t   i, j;

  Serial.begin(115200);

  pinMode(13, OUTPUT);



  tft.begin();
  tft.setRotation(1);

  test();
}


void loop(void)
{
  static uint32_t t_time[8];


  if (millis()-t_time[0] >= 1000)
  {
    Serial.println("Test");
    digitalWrite(13, !digitalRead(13));
    t_time[0] = millis();
  }

  if (Serial.available())
  {
    Serial.print("Received : 0x");
    Serial.println(Serial.read(), HEX);
  }
}



#ifdef TEST1

void test(void)
{
  coord_t   width, height;
  coord_t   i, j;


  gfxInit();

  // Get the screen size
  width = gdispGetWidth();
  height = gdispGetHeight();

  // Code Here
  gdispDrawBox(10, 10, width / 2, height / 2, Yellow);

  gdispFillArea(width / 2, height / 2, width / 2 - 10, height / 2 - 10, Blue);
  gdispDrawLine(5, 30, width - 50, height - 40, Red);

  for (i = 5, j = 0; i < width && j < height; i += 7, j += i / 20)
    gdispDrawPixel(i, j, White);

  gdispFlush();
  delay(1000);

  coord_t   r1, r2, cx, cy;
  uint8_t   sectors;


  // Get the screen size
  width = gdispGetWidth();
  height = gdispGetHeight();

  // Initialize some variables
  r1 = width > height ? height/3 : width/3;
  r2 = r1*3/4;
  cx = width/2;
  cy = height/2;
  sectors = 1;

  while(1)
  {
    // Draw the arc sectors
    gdispClear(White);
    gdispDrawArcSectors(cx, cy, r1, sectors, Blue);
    gdispFillArcSectors(cx, cy, r2, sectors, Red);

    // Increase the sectors counter
    sectors++;

    //gdispFlush();
    // Waste some time
    gfxSleepMilliseconds(250);
  }

}
#endif



#ifdef TEST2
#define BALLCOLOR1    Red
#define BALLCOLOR2    Yellow
#define WALLCOLOR   HTML2COLOR(0x303030)
#define BACKCOLOR   HTML2COLOR(0xC0C0C0)
#define FLOORCOLOR    HTML2COLOR(0x606060)
#define SHADOWALPHA   (255-255*0.2)

double invsqrt(const double x)
{
    return 1.0 / sqrt(x);
}

void test(void) {
  coord_t   width, height, x, y, radius, ballx, bally, dx, floor;
  coord_t   minx, miny, maxx, maxy;
  coord_t   ballcx, ballcy;
  color_t   colour;
  float   ii, spin, dy, spinspeed, h, f, g;

  gfxInit();

  width = gdispGetWidth();
  height = gdispGetHeight();

  radius=height/5+height%2+1; // The ball radius
  ii = 1.0/radius;      // radius as easy math
  floor=height/5-1;     // floor position
  spin=0.0;         // current spin angle on the ball
  spinspeed=0.1;        // current spin speed of the ball
  ballx=width/2;        // ball x position (relative to the ball center)
  bally=height/4;       // ball y position (relative to the ball center)
  dx=.01*width;       // motion in the x axis
  dy=0.0;           // motion in the y axis
  ballcx = 12*radius/5;   // ball x diameter including the shadow
  ballcy = 21*radius/10;    // ball y diameter including the shadow


  minx = miny = 0; maxx = width; maxy = height;   // The clipping window for this frame.

  while(1) {
    // Draw one frame
    gdispStreamStart(minx, miny, maxx-minx, maxy-miny);
    for (y=miny; h = (bally-y)*ii, y<maxy; y++) {
      for (x=minx; x < maxx; x++) {
        g=(ballx-x)*ii;
        f=-.3*g+.954*h;
        if (g*g < 1-h*h) {
          /* The inside of the ball */
          if ((((int)(9-spin+(.954*g+.3*h)*invsqrt(1-f*f))+(int)(2+f*2))&1))
            colour = BALLCOLOR1;
          else
            colour = BALLCOLOR2;
        } else {
          // The background (walls and floor)
          if (y > height-floor) {
            if (x < height-y || height-y > width-x)
              colour = WALLCOLOR;
            else
              colour = FLOORCOLOR;
          } else if (x<floor || x>width-floor)
            colour = WALLCOLOR;
          else
            colour = BACKCOLOR;

          // The ball shadow is darker
          if (g*(g+.4)+h*(h+.1) < 1)
            colour = gdispBlendColor(colour, Black, SHADOWALPHA);
        }
        gdispStreamColor(colour); /* pixel to the LCD */
      }
    }
    gdispStreamStop();

    // Force a display update if the controller supports it
    gdispFlush();

    // Calculate the new frame size (note this is a drawing optimisation only)
    minx = ballx - radius; miny = bally - radius;
    maxx = minx + ballcx; maxy = miny + ballcy;
    if (dx > 0) maxx += dx; else minx += dx;
    if (dy > 0) maxy += dy; else miny += dy;
    if (minx < 0) minx = 0;
    if (maxx > width) maxx = width;
    if (miny < 0) miny = 0;
    if (maxy > height) maxy = height;

    // Motion
    spin += spinspeed;
    ballx += dx; bally += dy;
    dx = ballx < radius || ballx > width-radius ? spinspeed=-spinspeed,-dx : dx;
    dy = bally > height-1.75*floor ? -.04*height : dy+.002*height;
  }
}
#endif


#ifdef TEST3
void test(void) {
  coord_t   width;
  font_t    font1, font2;
  coord_t   fheight1, fheight2;
  const char  *line1, *line2;
  char    buf[8];

    // Initialize and clear the display
    gfxInit();

    // Get the screen size
    width = gdispGetWidth();

    // Get the fonts we want to use
  font1 = gdispOpenFont("DejaVu*");
  font2 = gdispOpenFont("UI2*");
  //font2 = gdispOpenFont("Geosans*");
  //font2 = gdispOpenFont("Free*");
  //font2 = gdispOpenFont("Hellovetica*");
  //font2 = gdispOpenFont("babyblue*");
  //font2 = gdispOpenFont("PF Ronda*");
  //font2 = gdispOpenFont("Apple*");

  // Font 1
  line1 = "a b c d e f h i j k l m n o p q r s t u v w x y z A B C D E F G H I J K L M N O P Q R S T U V W X Y Z";
  line2 = "0 1 2 3 4 5 6 7 8 9 ~ ! @ # $ % ^ & * _ - + = ( ) { } [ ] < > | / \\ : ; , . ? ' \" `";
  fheight1 = gdispGetFontMetric(font1, fontHeight)+2;
  gdispFillStringBox(0, 0, width,  fheight1, line1, font1, Black, White, justifyCenter);
  gdispFillStringBox(0, fheight1+1, width,  fheight1, line2, font1, Black, White, justifyCenter);

  // Font 2
  fheight2 = gdispGetFontMetric(font2, fontHeight)+2;
  gdispFillStringBox(0, 2*fheight1+2, width,  fheight2, line1, font2, Black, White, justifyCenter);
  gdispFillStringBox(0, 2*fheight1+fheight2+3, width,  fheight2, line2, font2, Black, White, justifyCenter);

  // Show Sizes
  buf[0] = (fheight1-2)/10 + '0';
  buf[1] = (fheight1-2)%10 + '0';
  buf[2] = ',';
  buf[3] = ' ';
  buf[4] = (fheight2-2)/10 + '0';
  buf[5] = (fheight2-2)%10 + '0';
  buf[6] = 0;
  gdispFillStringBox(0, 2*fheight1+2*fheight2+4, width,  fheight1, buf, font1, Red, White, justifyCenter);

  // Wait forever
    while(TRUE) {
      gfxSleepMilliseconds(500);
    }
}
#endif


#ifdef TEST4
/* ---------------------------------------------------------------------- */

#define N         512      /* Number of dots */
#define SCALE     1024
#define INCREMENT 512   /* INCREMENT = SCALE / sqrt(N) * 2 */
#define PI2 6.283185307179586476925286766559

#define background RGB2COLOR(0,0,0)

uint16_t width, height;
int16_t sine[SCALE+(SCALE/4)];
int16_t *cosi = &sine[SCALE/4]; /* cos(x) = sin(x+90d)... */


void initialize (void)
{
  uint16_t i;

  /* if you change the SCALE*1.25 back to SCALE, the program will
   * occassionally overrun the cosi array -- however this actually
   * produces some interesting effects as the BUBBLES LOSE CONTROL!!!!
   */
  for (i = 0; i < SCALE+(SCALE/4); i++)
    //sine[i] = (-SCALE/2) + (int)(sinf(PI2 * i / SCALE) * sinf(PI2 * i / SCALE) * SCALE);
    sine[i] = (int)(sinf(PI2 * i / SCALE) * SCALE);
}


void matrix (int16_t xyz[3][N], color_t col[N])
{
  static uint32_t t = 0;
  int16_t x = -SCALE, y = -SCALE;
  uint16_t i, s, d;
  uint8_t red,grn,blu;

#define RED_COLORS (32)
#define GREEN_COLORS (64)
#define BLUE_COLORS (32)

  for (i = 0; i < N; i++)
    {
      xyz[0][i] = x;
      xyz[1][i] = y;

      d = sqrt(x * x + y * y);  /* originally a fastsqrt() call */
      s = sine[(t * 30) % SCALE] + SCALE;

      xyz[2][i] = sine[(d + s) % SCALE] * sine[(t * 10) % SCALE] / SCALE / 2;

      red = (cosi[xyz[2][i] + SCALE / 2] + SCALE) *
  (RED_COLORS - 1) / SCALE / 2;
      grn = (cosi[(xyz[2][i] + SCALE / 2 + 2 * SCALE / 3) % SCALE] + SCALE) *
  (GREEN_COLORS - 1) / SCALE / 2;
      blu = (cosi[(xyz[2][i] + SCALE / 2 + SCALE / 3) % SCALE] + SCALE) *
  (BLUE_COLORS - 1) / SCALE / 2;
      col[i] = ((red << 11) + (grn << 5) + blu);

      x += INCREMENT;

      if (x >= SCALE) x = -SCALE, y += INCREMENT;
    }
  t++;
}


void rotate (int16_t xyz[3][N], uint16_t angleX, uint16_t angleY, uint16_t angleZ)
{
  uint16_t i;
  int16_t tmpX, tmpY;
  int16_t sinx = sine[angleX], cosx = cosi[angleX];
  int16_t siny = sine[angleY], cosy = cosi[angleY];
  int16_t sinz = sine[angleZ], cosz = cosi[angleZ];

  for (i = 0; i < N; i++)
    {
      tmpX      = (xyz[0][i] * cosx - xyz[2][i] * sinx) / SCALE;
      xyz[2][i] = (xyz[0][i] * sinx + xyz[2][i] * cosx) / SCALE;
      xyz[0][i] = tmpX;

      tmpY      = (xyz[1][i] * cosy - xyz[2][i] * siny) / SCALE;
      xyz[2][i] = (xyz[1][i] * siny + xyz[2][i] * cosy) / SCALE;
      xyz[1][i] = tmpY;

      tmpX      = (xyz[0][i] * cosz - xyz[1][i] * sinz) / SCALE;
      xyz[1][i] = (xyz[0][i] * sinz + xyz[1][i] * cosz) / SCALE;
      xyz[0][i] = tmpX;
    }
}


void draw(int16_t xyz[3][N], color_t col[N])
{
  static uint16_t oldProjX[N] = {0};
  static uint16_t oldProjY[N] = {0};
  static uint8_t oldDotSize[N] = {0};
  uint16_t i, projX, projY, projZ, dotSize;

  for (i = 0; i < N; i++)
    {
      projZ = SCALE - (xyz[2][i] + SCALE) / 4;
      projX = width / 2 + (xyz[0][i] * projZ / SCALE) / 25;
      projY = height / 2 + (xyz[1][i] * projZ / SCALE) / 25;
      dotSize = 3 - (xyz[2][i] + SCALE) * 2 / SCALE;

      gdispDrawCircle (oldProjX[i], oldProjY[i], oldDotSize[i], background);
      if (projX > dotSize &&
    projY > dotSize &&
    projX < width - dotSize &&
    projY < height - dotSize)
  {
    gdispDrawCircle (projX, projY, dotSize, col[i]);
    oldProjX[i] = projX;
    oldProjY[i] = projY;
    oldDotSize[i] = dotSize;
  }
    }

  gdispFlush();
}


/* ---------------------------------------------------------------------- */

int16_t angleX = 0, angleY = 0, angleZ = 0;
int16_t speedX = 0, speedY = 0, speedZ = 0;

int16_t xyz[3][N];
color_t col[N];


void test (void)
{
  int pass = 0;

  gfxInit();

  gfxSleepMilliseconds (10);
  gdispClear (background); /* glitches.. */
  gfxSleepMilliseconds (10);
  gdispClear (background);  /* glitches.. */
  gfxSleepMilliseconds (10);
  gdispClear (background);  /* glitches.. */

  width = (uint16_t)gdispGetWidth();
  height = (uint16_t)gdispGetHeight();

  gdispFlush();

  initialize();

  for (;;)
    {
      matrix(xyz, col);
      rotate(xyz, angleX, angleY, angleZ);
      draw(xyz, col);

      angleX += speedX;
      angleY += speedY;
      angleZ += speedZ;

      if (pass > 400) speedY = 1;
      if (pass > 800) speedX = 1;
      if (pass > 1200) speedZ = 1;
      pass++;

      if (angleX >= SCALE) angleX -= SCALE;
      else if (angleX < 0) angleX += SCALE;

      if (angleY >= SCALE) angleY -= SCALE;
      else if (angleY < 0) angleY += SCALE;

      if (angleZ >= SCALE) angleZ -= SCALE;
      else if (angleZ < 0) angleZ += SCALE;
    }
}

/* ---------------------------------------------------------------------- */
#endif



#ifdef TEST5

/* The handles for our two Windows */
GHandle GW1, GW2;

void test(void) {
    coord_t   i, j;

    /* Initialize and clear the display */
    gfxInit();
    gdispClear(White);

    /* Create two windows */
    {
      GWindowInit wi;

    gwinClearInit(&wi);
      wi.show = TRUE; wi.x = 20; wi.y = 10; wi.width = 200; wi.height = 150;
        GW1 = gwinWindowCreate(0, &wi);
      wi.show = TRUE; wi.x = 50; wi.y = 190; wi.width = 150; wi.height = 100;
        GW2 = gwinWindowCreate(0, &wi);
    }

    /* Set fore- and background colors for both windows */
    gwinSetColor(GW1, Black);
    gwinSetBgColor(GW1, White);
    gwinSetColor(GW2, White);
    gwinSetBgColor(GW2, Blue);

    /* Clear both windows - to set background color */
    gwinClear(GW1);
    gwinClear(GW2);

    gwinDrawLine(GW1, 5, 30, 150, 110);
    for(i = 5, j = 0; i < 200 && j < 150; i += 3, j += i/20)
          gwinDrawPixel(GW1, i, j);

    /*
     * Draw two filled circles at the same coordinate
     * of each window to demonstrate the relative coordinates
     * of windows
     */
    gwinFillCircle(GW1, 20, 20, 15);
    gwinFillCircle(GW2, 20, 20, 15);

    i = 0;
    while(TRUE) {
      gwinSetColor(GW1, White);
      gwinFillCircle(GW1, 20+i, 20, 15);
      gwinSetColor(GW1, Black);
      i++;
      gwinFillCircle(GW1, 20+i, 20, 15);

      gfxSleepMilliseconds(10);
    }
}
#endif
