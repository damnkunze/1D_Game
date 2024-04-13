#include <FastLED.h>
#include <Arduino.h>

#ifndef HELPERS_H
#define HELPERS_H

extern CRGB blankPixel;

void endGame();
CRGB addColors(CRGB color1, CRGB color2);
int constrainInt(int value, int min, int max);
CRGB addValuesToColor(CRGB color, int red, int green, int blue);
void printIntArray(int array[], int len);
void printLEDs(CRGB leds[]);
#endif
