#include <FastLED.h>
#include "helpers.h"

#include "Bullet.h"
#include "Player.h"
using namespace B;
using namespace P;

#ifndef ENEMY_H
#define ENEMY_H

namespace E {
    extern int enemySpeed;
    extern int enemySpawnDist;
    
    extern int enemyBasicLives;
    extern int enemyBasicMoveSlowness;

    extern int enemyShootLives;
    extern int enemyShootMoveSlowness;
    extern int enemyShootWantedDistance;

    extern int enemyAnimationStrength;
    extern int enemyAnimationLength;
    extern CRGB enemyBaseColor;
    extern CRGB enemyShootBaseColor;
}

class Enemy {
public:
    int pos;
    int enemyType; // 0 = none, 1 = normal, 2 = shooting
    int lives;
    CRGB color;
    float currPointInAnimation;
    int currAnimationColorChange;
    int bulletIndex; // can only have one bullet

    Enemy();
    Enemy(int pos, int enemyType);

    void moveIrregularly(int playerPos, int num_leds);
    void moveAwayIrregularly(int playerPos, int maxPos, int num_leds);
    bool getsHit(int bulletPos, int bulletSpeed);
    bool shouldShoot(int playerPos, int num_leds);
    Bullet shoot();
    bool movedTooFar(int minPos, int maxPos);
    void animateColor();
};

#endif