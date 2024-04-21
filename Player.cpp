#include "headers/Player.h"
using namespace B;

// constants have to be pre-defined in header
using namespace P;
namespace P {
    int playerStartLives = 2;
    CRGB playerStartColor = CRGB(250, 5, 5); 
    int playerSpeed = 1;
    int playerSpawnpoint = 5;
    // CRGB(random(5, 245), random(5, 245), random(5, 250));
}

Player::Player() {
    pos = playerSpawnpoint;
    color = playerStartColor;
    lives = playerStartLives;
    velocity = 0;
    bulletCount = 0;
}

bool Player::getsHit(int enemyPos) {
    return (enemyPos <= (int) pos && enemyPos >= (int) pos - (int) velocity);
}
        
Bullet Player::shoot() {
    bulletCount++;
    // First bullet animation spot is on player and >= 0:
    int pos = max((int) this->pos - playerBulletSpeed, 0);
    // Velocity is different when player is moving and > 0
    int velocity = max((int) this->velocity, 0) + playerBulletSpeed;
    return Bullet(pos, 1, velocity, color);
}       

CRGB Player::animateColor(int increase) {
    int cStep = 5 + increase;
    int minColor = 5;
    int maxColor = 255 - minColor;

    CRGB newColor = color;
    if (color.r + cStep <= maxColor && color.g <= minColor) {
        newColor.r += cStep;
        newColor.b -= cStep;
    } else if (color.g + cStep <= maxColor && color.b <= minColor) {
        newColor.g += cStep;
        newColor.r -= cStep;
    } else if (color.b + cStep <= maxColor) {
        newColor.b += cStep;
        newColor.g -= cStep;
    } else {
        newColor.r -= cStep;
        newColor.g -= cStep;
    }
    color = newColor;
}
