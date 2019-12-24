#include <BasicLinearAlgebra.h> //dependency of Geometry.h
#include <Geometry.h> //https://github.com/tomstewart89/Geometry -> used to rotate the LEDs around the cube.
#include "Adafruit_NeoPixel.h" //https://github.com/adafruit/Adafruit_NeoPixel used for helping us set LED color values.
#define NUM_LEDS 33 //if you've been given a mirror box by John, don't change this value.  If you're experimenting on your own time, set this to the number of LEDs in your array.
#define PIN 4 //if you've been given a mirror box by John, the data pin is 4.

//if you wish to change the max values possible of each color channel, change this to a number between 0 and 255;
#define R_CLAMP 255
#define G_CLAMP 255
#define B_CLAMP 255

//increase or decrease this dependent on loop speed
#define GLOBAL_WAIT 100

//if you want to change the Y-value the LEDs start on, change this value.  You'll get more saturated colors the lower or higher the value is.
#define STARTINGY 50

//if you want to change the rate of rotation around the centerpoint, change these values
#define ROTX 0.07
#define ROTY 0.07
#define ROTZ 0.07

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

Point points[NUM_LEDS];
void setup() {
  // Serial.begin(115200); //uncomment this if you are going to monitor outputs.
  strip.begin();
  strip.show();

  //initialize all of our points before any update logic starts
  initPoints();
}

//walks the points around 3d space in a cube-like shape and initializes their Y-value;
void initPoints() {
  int splitCount = NUM_LEDS / 4;
  int remainder = NUM_LEDS - (splitCount * 4);
  Serial << splitCount << '\n' << remainder << '\n';

  //set up all points at their initial values in 3d space, we'll have four splits, the last gets the remainder
  float len = 255;
  
  //forward
  float frontPadding = ((float)255 / (float)8 / (float)2);
  uint16_t count = 0;
  for(uint16_t i = 0; i < splitCount; i++) {
    Point *point = &points[i];
    point->X() = frontPadding + (len / splitCount) * count++;
    point->Y() = STARTINGY;
    point->Z() = 0;
  }

  //left
  count = 0;
  for(uint16_t i = splitCount; i < splitCount * 2; i++) {
    Point *point = &points[i];
    point->X() = 0;
    point->Y() = STARTINGY;
    point->Z() = frontPadding +(len / splitCount) * count++;
  }

  //back
  count = 0;
  for(uint16_t i = splitCount * 2; i < splitCount * 3; i++) {
    Point *point = &points[i];
    point->X() = frontPadding + (len / splitCount) * count++;
    point->Y() = STARTINGY;
    point->Z() = len;
  }

  //right
  count = 0;
  frontPadding = ((float)255 / (float)9 / (float)2);
  for(uint16_t i = splitCount * 3; i < (splitCount * 4) + 1; i++) {
    Point *point = &points[i];
    point->X() = len;
    point->Y() = STARTINGY;
    point->Z() = frontPadding + (len / (splitCount + 1)) * count++;
  }
}

void loop() {
  //rainbowCycle(33);
  rotatePoints(ROTX, ROTY, ROTZ);
  delay(GLOBAL_WAIT);
}

void rotatePoints(float rX, float rY, float rZ) {
  Rotation r;
  r.RotateY(rY);
  r.RotateX(rX);
  r.RotateZ(rZ);

  Point ptZero = points[0];
  for(uint16_t i=0; i < NUM_LEDS; i++) {
    Point *pt = &points[i];
    Point newPt = r * *pt;
    pt->X() = newPt.X();
    pt->Z() = newPt.Z();
    pt->Y() = newPt.Y();
    
    strip.setPixelColor(i, getPointColor(pt));
  }
  strip.show();
}

uint32_t getPointColor(Point *point) {
  float x = min(R_CLAMP, max(0, point->X()));
  float y = min(G_CLAMP, max(0, point->Y()));
  float z = min(B_CLAMP, max(0, point->Z()));
  
  return strip.Color(round(x), round(y), round(z));
}
