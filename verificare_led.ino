#include <FastLED.h>

#define LED_PIN     6
#define NUM_LEDS    12

CRGB leds[NUM_LEDS];

void setup() {

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  
}

void loop() {
  
  leds[0] = CRGB(255, 0, 0);
  FastLED.show();
  delay(500);  
  leds[3] = CRGB(0, 255, 0);
  FastLED.show();
  delay(500);
  leds[7] = CRGB(0, 0, 255);
  FastLED.show();
  delay(500);
  leds[11] = CRGB(0, 255, 0);
  FastLED.show();
  delay(500);
 
}