#include <Arduino.h>

#define PIN_RED 13
#define PIN_GREEN 15
#define PIN_BLUE 3

void setLedColorHSV(int h, double s, double v);

void setup() {
  
}
void loop() {
  setLedColorHSV((int)(millis() / 10 % 360), 1, 1);
}

// Convert a given HSV (Hue Saturation Value) to RGB(Red Green Blue) and set the led to the color
//  h is hue value, integer between 0 and 360
//  s is saturation value, double between 0 and 1
//  v is value, double between 0 and 1
void setLedColorHSV(int h, double s, double v) {
  double r = 0;
  double g = 0;
  double b = 0;
 
  double hf = h / 60.0;
 
  int i = (int)floor(h / 60.0);
  double f = h / 60.0 - i;
  double pv = v * (1 - s);
  double qv = v * (1 - s * f);
  double tv = v * (1 - s * (1 - f));
 
  switch (i)
  {
    case 0:
      r = v;
      g = tv;
      b = pv;
      break;
    case 1:
      r = qv;
      g = v;
      b = pv;
      break;
    case 2:
      r = pv;
      g = v;
      b = tv;
      break;
    case 3:
      r = pv;
      g = qv;
      b = v;
      break;
    case 4:
      r = tv;
      g = pv;
      b = v;
      break;
    case 5:
      r = v;
      g = pv;
      b = qv;
      break;
  }
 
  //set each component to a integer value between 0 and 255
  int red = constrain((int)255 * r, 0, 255);
  int green = constrain((int)255 * g, 0, 255);
  int blue = constrain((int)255 * b, 0, 255);
  analogWrite(PIN_RED, red);
  analogWrite(PIN_GREEN, green);
  analogWrite(PIN_BLUE, blue);
}