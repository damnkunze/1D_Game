#include "headers/Enemy.h"
using namespace B;
using namespace P;

using namespace E;
namespace E {
    int enemySpeed = 1;
    int enemySpawnDist = 70;

    int enemyBasicLives = 4;
    int enemyBasicMoveSlowness = 200;

    int enemyShootLives = 2;
    int enemyShootWantedDistance = enemyBulletRange * 2;
    int enemyShootMoveSlowness = 600;
    int enemyShootShootingSlowness = 1000;

    int enemyAnimationStrength = 100;
    int enemyAnimationLength = 10;
    CRGB enemyBasicBaseColor = CRGB(0, 255 - enemyAnimationStrength, 0);
    CRGB enemyShootBaseColor = CRGB(0, 0, 255 - enemyAnimationStrength);
}

// Empty placeholder
Enemy::Enemy() {
    this->pos = -1;
    this->enemyType = 0; // none
    this->lives = 0;
    this->color = blankPixel;
    this->currPointInAnimation = 0;
    this->currAnimationColorChange = 0;
    this->bulletIndex = -1;
}

Enemy::Enemy(int pos, int enemyType) {
    this->pos = pos;
    this->enemyType = enemyType;
    this->lives = enemyBasicLives;
    this->color = enemyBasicBaseColor;

    if (enemyType == 2) { // shooting
        this->lives = enemyShootLives;
        this->color = enemyShootBaseColor;
    }
    // Add random color change to base color
    this->currPointInAnimation = random(-enemyAnimationLength + 1, enemyAnimationLength);
    this->currAnimationColorChange = 0;
    this->bulletIndex = -1;
}

void Enemy::moveIrregularly(int playerPos, int num_leds) {
    float distanceToPlayer = this->pos - playerPos;
    float chanceToMove = pow(distanceToPlayer / num_leds, 2) + 0.02; // (x / 300)^2 + 0.02
    // 1: dont move, 0: move
    float shouldMove = random(0, 1 + enemyBasicMoveSlowness * chanceToMove);
    
    if (shouldMove <= 1) {
        this->pos -= enemySpeed;
    }
}

void Enemy::moveAwayIrregularly(int playerPos, int maxPos, int num_leds) {
    float distanceToPlayer = this->pos - playerPos;
    if (distanceToPlayer > enemyShootWantedDistance || this->pos + enemySpeed >= maxPos) {
        return;
    }
    float chanceToMove = pow(distanceToPlayer / num_leds, 2) + 0.02; // (x / 300)^2 + 0.02
    // 1: dont move, 0: move
    float shouldMove = random(0, 1 + enemyShootMoveSlowness * chanceToMove);
    
    if (shouldMove <= 1) {
        this->pos += enemySpeed;
    }
}

bool Enemy::getsHit(int bulletPos, int bulletVelocity) {
    return (this->pos != -1 && this->pos <= bulletPos && this->pos >= bulletPos - bulletVelocity);
}

bool Enemy::shouldShoot(int playerPos, int num_leds) {
    if (this->bulletIndex != -1) {
        return false;
    }
    float distanceToPlayer = this->pos - playerPos;

    float chanceToShoot = pow(distanceToPlayer / num_leds, 2) + 0.02; // (x / 300)^2 + 0.02
    // 1: dont move, 0: move
    float shouldShoot = random(0, 1 + enemyShootShootingSlowness * chanceToShoot);
    return shouldShoot <= 1;
}

Bullet Enemy::shoot() {
    int bulletColorChange = 80;

    int bulletPos = max(this->pos - enemyBulletSpeed, 0);
    CRGB bulletColor = addValuesToColor(this->color, -bulletColorChange, -bulletColorChange, -bulletColorChange);
    return Bullet(bulletPos, 2, enemyBulletSpeed, bulletColor); // 2: enemy
}

bool Enemy::movedTooFar(int minPos, int maxPos) {
    if (this->enemyType == 2) maxPos -= 1;

    if (this->pos > maxPos || this->pos < minPos) {
        if (this->pos < minPos) this->pos = minPos - 1;
        if (this->pos > maxPos) this->pos = maxPos + 1;
        return true;
    }
    return false;
}

// Imitate breathing effect
void Enemy::animateColor() {
    float animationSpeed = 0.2;
    int damagedColorChangeBasic = 12;
    int damagedColorChangeShoot = 18;

    this->currPointInAnimation += animationSpeed;
    if (this->currPointInAnimation >= enemyAnimationLength) {
        this->currPointInAnimation = -enemyAnimationLength;
    }
    
    float animationFuncVal = max(0, -1 * pow(this->currPointInAnimation / 5 - 1, 4) + 1);
    // -(x / 5 - 1)^4 + 1

    if (this->enemyType == 1) {
        int livesLost = enemyBasicLives - this->lives;
        int changeGreen = enemyAnimationStrength * animationFuncVal - (livesLost * damagedColorChangeBasic);
        int changeRedBlue = livesLost * damagedColorChangeBasic;
        this->color = addValuesToColor(enemyBasicBaseColor, changeRedBlue, changeGreen, changeRedBlue);
    } else if (this->enemyType == 1) {
        int livesLost = enemyShootLives - this->lives;
        int changeBlue = enemyAnimationStrength * animationFuncVal;
        int changeRedGreen = livesLost * damagedColorChangeShoot;
        this->color = addValuesToColor(enemyShootBaseColor, changeRedGreen, changeRedGreen, changeBlue);
    }
}