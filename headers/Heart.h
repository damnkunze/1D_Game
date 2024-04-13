#include <FastLED.h>
#include <math.h>
#include "helpers.h"
#include "Player.h"
using namespace P;

#ifndef HEART_H
#define HEART_H

extern CRGB blankPixel;

namespace H {
    extern float heartNotColAnimStrength;
    extern float heartNotColAnimLength;
    extern float heartNotColAnimSpeed;
    extern float heartColAnimStrength;
    extern float heartColAnimLength;
    extern float heartColAnimSpeed;
    extern CRGB heartBaseColor;
}

class Heart {
public:
    int pos;
    CRGB color;
    float currPointInAnimation;
    int currAnimationColorChange;
    bool collected;
    bool justCollected;

    Heart();
    Heart(int pos, bool collected);

    bool getsCollected(int playerPos, int playerSpeed);
    void animateColor();
    CRGB fadeOut();
};

#endif
