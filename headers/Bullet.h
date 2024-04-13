#include <FastLED.h>

#ifndef BULLET_H
#define BULLET_H

extern CRGB blankPixel;

namespace B {
    extern int playerBulletSpeed;
    extern int playerBulletRange;
    extern int enemyBulletRange;
    extern float enemyBulletSpeed;
}    

class Bullet {
public:
    float pos; // always floor before rendering and checking positions
    int shotBy; // 0: none, 1: player, 2: enemy
    float velocity;
    CRGB color;
    float distanceMoved; // non-negative
    bool stopped;
    int enemyIndex;
    // Empty constructor for array struct initialisation
    Bullet();
    Bullet(float pos, int shotBy, float velocity, CRGB color);

    void move();
    bool movedTooFar(int bulletRange, int minPos, int maxPos);
    CRGB fadeOut();
};

#endif