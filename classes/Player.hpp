#include <FastLED.h>

namespace P {
    const uint8_t playerSpeed = 1;
    const uint8_t playerStartLives = 2;
    const uint16_t playerSpawnpoint = 5;
    CRGB playerStartColor = CRGB(250, 5, 5); // CRGB(random(5, 245), random(5, 245), random(5, 250));

    class Player {
    public:
        uint16_t pos;
        CRGB color;
        uint8_t lives;
        uint8_t velocity;
        uint8_t bulletCount;
        
        Player() {
            pos = playerSpawnpoint;
            color = playerStartColor;
            lives = playerStartLives;
            velocity = 0;
            bulletCount = 0;
        }

        bool getsHit(int enemyPos) {
            return (enemyPos <= pos && enemyPos >= pos - velocity);
        }

        Bullet shoot() {
            bulletCount++;
            // First bullet animation spot is on player and >= 0:
            int pos = max(pos - bulletSpeed, 0);
            // Velocity is different when player is moving and > 0
            int velocity = max(velocity, 0) + bulletSpeed;
            return Bullet(pos, "player", velocity, color);
        }

        CRGB animateColor() {
            int cStep = 5;
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
    };
}