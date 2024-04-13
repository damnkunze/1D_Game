#include <FastLED.h>
#include "Bullet.h"
using namespace B;

#ifndef PLAYER_H
#define PLAYER_H

namespace P {
    extern int playerSpeed;
    extern int playerStartLives;
    extern int playerSpawnpoint;
    extern CRGB playerStartColor; 
}

class Player {
public:
    int pos;
    CRGB color;
    int lives;
    int velocity;
    int bulletCount;
    Player();
    
    bool getsHit(int enemyPos);
    Bullet shoot();
    CRGB animateColor(int multiplier);
};

#endif