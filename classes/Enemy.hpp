#include <FastLED.h>

namespace E {
    int enemyLives = 4;
    int enemySpeed = 1;
    int enemySpawnDist = 70;
    int enemySlowness = 200;

    int enemyMaxBullets = 1;
    int enemyBulletReach = 20;
    int enemyBulletSpeed = -1;

    int enemyAnimationStrength = 100;
    int enemyAnimationLength = 10;
    CRGB enemyBaseColor = CRGB(0, 255 - enemyAnimationStrength, 0);

    class Enemy {
    public:
        int pos;
        String enemyType;
        int lives;
        CRGB color;
        float currPointInAnimation;
        int currAnimationColorChange;

        // Empty placeholder
        Enemy() {
            pos = -1;
            enemyType = "none";
            lives = 0;
            color = blankPixel;
            currPointInAnimation = 0;
            currAnimationColorChange = 0;
        }
        Enemy(int pos, String enemyType) {
            pos = pos;
            enemyType = enemyType;
            lives = enemyLives;
            color = enemyBaseColor;
            // Add random color change to base color
            currPointInAnimation = random(-enemyAnimationLength + 1, enemyAnimationLength);;
            currAnimationColorChange = 0;
        }

        void moveIrregularly(int playerPos) {
            float distanceToPlayer = pos - playerPos;
            float enemySlownessBias = 0.02;
            float chanceToMove = pow(distanceToPlayer / NUM_LEDS, 2) + enemySlownessBias; // (x / 300)^2 + 0.02
            // 1: dont move, 0: move
            float shouldMove = random(0, 1 + enemySlowness * chanceToMove);
            
            if (shouldMove <= 1) {
                pos -= enemySpeed;
            }
        }

        bool getsHit(int bulletPos, int bulletSpeed) {
            return (pos <= bulletPos && pos >= bulletPos - bulletSpeed);
        }
  
        Bullet shoot() {
            int bulletPos = max(pos - bulletSpeed, 0);
            CRGB bulletColor = addValuesToColor(color, 40, -40, 0);
            return Bullet(bulletPos, "enemy", enemyBulletSpeed, bulletColor);
        }

        // Imitate breathing effect
        void animateColor() {
            float animationSpeed = 0.2;
            int damagedColorChange = 12;

            currPointInAnimation += animationSpeed;
            if (currPointInAnimation >= enemyAnimationLength) {
                currPointInAnimation = -enemyAnimationLength;
            }
            
            float animationFuncVal = max(0, -1 * pow(currPointInAnimation / 5 - 1, 4) + 1);
            // -(x / 5 - 1)^4 + 1
            int livesLost = enemyLives - lives;
            int changeGreen = enemyAnimationStrength * animationFuncVal;
            int changeRedBlue = livesLost * damagedColorChange;
            color = addValuesToColor(enemyBaseColor, changeRedBlue, changeGreen, changeRedBlue);
        }
    };
}