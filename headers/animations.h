#include <FastLED.h>
#include "Heart.h"
#include "helpers.h"
using namespace H;

#ifndef ANIMATIONS_H
#define ANIMATIONS_H

extern CRGB blankPixel;
extern int UPDATES_PER_SECOND;
extern int NUM_LEDS;
extern CRGB LED_STRIPE[NUM_LEDS];

Heart showHeartsCollected(Heart representingHeart, int hearts);
int showLevelBar(int levelBarPos, int levelsToShow);
void clearStripe();
void entryAnimation(int from, int to, CRGB color, CRGB blankPixel);
void winAnimation(int pos);
void loseAnimation(int to);
void amazingWinAnimation();

#endif