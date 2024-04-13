#include "headers/Heart.h"

using namespace H;
namespace H {
    float heartNotColAnimStrength = 170;
    float heartNotColAnimLength = 20;
    float heartNotColAnimSpeed = 1;

    float heartColAnimStrength = 70;
    float heartColAnimLength = 30;
    float heartColAnimSpeed = 0.7;
    CRGB heartBaseColor = CRGB(255 - heartNotColAnimStrength, 0, 0);
}

// Empty placeholder
Heart::Heart() {
    this->pos = -1;
    this->color = blankPixel;
    this->currPointInAnimation = 0;
    this->currAnimationColorChange = 0;
    this->collected = false;
    this->justCollected = false;
}

Heart::Heart(int pos, bool collected) {
    this->pos = pos;
    this->collected = collected;
    this->color = heartBaseColor;
    if (this->collected) {
        this->currPointInAnimation = random(-heartColAnimLength + 1, heartColAnimLength); 
    } else {
        this->currPointInAnimation = random(-heartNotColAnimLength + 1, heartNotColAnimLength); 
    }
    this->currAnimationColorChange = 0;
    this->justCollected = false;
}

bool Heart::getsCollected(int playerPos, int playerSpeed) {
    return this->pos != -1 && this->pos <= playerPos && this->pos >= playerPos - playerSpeed;
}

// Imitate heartbeat effect
void Heart::animateColor () {
    int animationFunctionRange = heartNotColAnimLength;
    int animationStrength = heartNotColAnimStrength;
    float animationSpeed = heartNotColAnimSpeed;

    if (this->collected) {
        animationFunctionRange = heartColAnimLength;
        animationStrength = heartColAnimStrength;
        animationSpeed = heartColAnimSpeed;
    }

    this->currPointInAnimation += animationSpeed;
    if (this->currPointInAnimation >= animationFunctionRange) {
        this->currPointInAnimation = -animationFunctionRange;
    }
    
    float animationFuncVal = max(0, -1 * pow(currPointInAnimation / 7.07, 4) + pow(currPointInAnimation / 5, 2));
    // -(x / 7.07)^4 + (x / 5)^2
    int changeRed = animationStrength * animationFuncVal;
    this->color = addValuesToColor(heartBaseColor, changeRed, 0, 0);
}

CRGB Heart::fadeOut() {
    int minAvgLight = 30;
    float fadeDecrease = 40;

    // Functions from FastLED library
    this->color = this->color.fadeToBlackBy(fadeDecrease);
    if (this->color.getAverageLight() <= minAvgLight) {
        return blankPixel;
    }
    return this->color;
}