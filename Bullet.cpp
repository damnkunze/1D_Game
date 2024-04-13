#include "headers/Bullet.h"

using namespace B;
namespace B {
    int playerBulletSpeed = 1;
    int playerBulletRange = 30;

    int enemyBulletRange = 20;
    float enemyBulletSpeed = 0.5 * -1; // inverted
}

// Empty placeholder
Bullet::Bullet() {
    this->pos = -1;
    this->shotBy = 0; // 0: none, 1: player, 2: enemy
    this->velocity = 0;
    this->color = blankPixel;
    this->distanceMoved = 0;
    this->stopped = true;
    this->enemyIndex = -1;
}

Bullet::Bullet(float pos, int shotBy, float velocity, CRGB color) {
    // 'this->' required cuz variable name conflict
    this->pos = pos; // always floor before rendering and checking positions
    this->shotBy = shotBy;
    this->velocity = velocity;
    this->color = color;
    this->distanceMoved = 0;
    this->stopped = false;
    this->enemyIndex = enemyIndex;
}

void Bullet::move() {
    this->pos += velocity;
    this->distanceMoved += max(velocity, -velocity); // abs
}

bool Bullet::movedTooFar(int bulletRange, int minPos, int maxPos) {
    if (this->distanceMoved > bulletRange || this->pos > maxPos || this->pos < minPos) {
        if (this->pos < minPos) this->pos = minPos - 1;
        if (this->pos > maxPos) this->pos = maxPos + 1;
        return true;
    }
    return false;
}

/* // Blinking effect
void Bullet::animateEnemyBulletColor () {
    int lowTime = heartNotColAnimLength;
    int highTime = heartNotColAnimLength;
    int highStrength = heartNotColAnimLength;
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
} */

CRGB Bullet::fadeOut() {
    int minAvgLight = 30;
    float fadeDecrease = 40;
    // fade out depending on bullet velocity
    if (this->velocity = playerBulletSpeed) fadeDecrease = 100;
    if (this->velocity > playerBulletSpeed) fadeDecrease = 170;

    // Functions from FastLED library
    this->color = this->color.fadeToBlackBy(fadeDecrease);
    if (this->color.getAverageLight() <= minAvgLight) {
        return blankPixel;
    }
    return this->color;
}