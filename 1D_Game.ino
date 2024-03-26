#include <FastLED.h>
#include <math.h>

#define LED_PIN    5
#define NUM_LEDS   300
#define LED_TYPE   WS2811
#define BRIGHTNESS 64
#define COLOR_ORDER GRB
#define UPDATES_PER_SECOND 100
CRGB blankPixel = CRGB(0, 0, 0);
CRGB LED_STRIPE[NUM_LEDS];

#define BUTTON_1_PIN 10
#define BUTTON_2_PIN 11
#define BUTTON_3_PIN 12

#define MAX_PLAYER_BULLETS 3
#define MAX_ENEMY_BULLETS 10
#define MAX_ENEMIES 20
#define MAX_HEARTS 2 // collectable hearts

#include "helpers.hpp"
using namespace Helpers;
#include "classes/Bullet.hpp"
using namespace B;
#include "classes/Player.hpp"
using namespace P;
#include "classes/Enemy.hpp"
using namespace E;
#include "classes/Heart.hpp"
using namespace H;
#include "animations.hpp"
using namespace A;
Animation anim = Animation();

int buttonMoveForwards = LOW;
int buttonMoveBackwards = LOW;
int buttonShoot = LOW;

// ============================================

Player player = Player();
Enemy enemies[MAX_ENEMIES];
#define MAX_BULLETS MAX_PLAYER_BULLETS + MAX_ENEMY_BULLETS
Bullet bullets[MAX_BULLETS];
// First 3 bullets are reserved for player, rest is for enemies

Heart hearts[MAX_HEARTS];

int levelBarOverPos = NUM_LEDS - 1;
int healthBarOverPos = 0;
Heart healthBarHeart = Heart();

int level = 1;
int enemyCount = 2 * level;
int lastLevel = 16;

void resetGame() {
    level = 1;
    player = Player();

    // Fill with empty placeholder hearts
    for (int i = 0; i < MAX_HEARTS; i++) {
        hearts[i] = Heart();
    }
}

void startLevel() {
    enemyCount = min(2 * level, MAX_ENEMIES);
    player.pos = playerSpawnpoint + healthBarOverPos;

    // Fill arrays with empty placeholder bullet and enemy objects
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i] = Bullet();
    }
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i] = Enemy();
    }

    // Spawn enemies
    for (int i = 0; i <= enemyCount; i++) {
        // random() is exclusive max
        int pos = random(playerSpawnpoint + enemySpawnDist + 1, levelBarOverPos - 1);
        enemies[i] = Enemy(pos, "basic");
    }

    /* for (int i = 0; i <= levelHasNShootingEnemies(level); i++) {
        int pos = random(NUM_LEDS / 2, levelBarOverPos - 1);
        enemies[i] = Enemy(pos, "shooting");
    } */

    // Spawn collectable hearts
    int nhearts = levelHasNHearts(level, player.lives);
    for (int i = 0; i < nhearts; i++) {
        if (hearts[i].pos == -1) {
            int pos = random(NUM_LEDS * 0.6, levelBarOverPos - 1);
            hearts[i] = Heart(pos, false);
        }
    }
    // Set health bar heart
    healthBarHeart = Heart(0, true);
    anim.entryAnimation(healthBarOverPos, playerSpawnpoint, player.color);
}

void setup() {
    pinMode(BUTTON_1_PIN, INPUT);
    pinMode(BUTTON_2_PIN, INPUT);
    pinMode(BUTTON_3_PIN, INPUT);
    Serial.begin(9600);

    delay(1000); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(LED_STRIPE, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);

    resetGame();
    startLevel();
}

bool isShooting = 0;
bool lostLive = true;

void loop() {
    if (lostLive == true) {
        lostLive = false;

        if (player.lives <= 0) {
            anim.loseAnimation(player, 0);
            resetGame();
            startLevel();
        } else {
            int firstEnemyPos = getFirstEnemyPos(enemies, enemyCount);
            // Respawn out of range of enemies and pos >= 0
            int respawnPoint = max(firstEnemyPos - enemySpawnDist - 1, 0);
            anim.loseAnimation(player, respawnPoint);
            player.pos = respawnPoint;
        }
    }
    // Check if won
    if (player.pos >= levelBarOverPos) {
        anim.winAnimation(player, levelBarOverPos);
        level++;
        if (level >= lastLevel) {
            anim.amazingWinAnimation();
        }
        startLevel();
    }
    
    buttonMoveForwards = digitalRead(BUTTON_2_PIN);
    buttonMoveBackwards = digitalRead(BUTTON_1_PIN);
    buttonShoot = digitalRead(BUTTON_3_PIN);
    
    // Input controls
    if (buttonMoveBackwards == HIGH) {
        if (player.pos - playerSpeed >= healthBarOverPos) player.velocity = -playerSpeed;

    } else if (buttonMoveForwards == HIGH) {
        player.velocity = playerSpeed;
    }
    player.pos += player.velocity;
    player.velocity = 0;
    
    if (buttonShoot == LOW && isShooting == 0) {
        isShooting = 1;
        // Make new bullet
        if (player.bulletCount < MAX_PLAYER_BULLETS) {
            int newBulletIndex = findFreeBulletSpace(bullets, 0, MAX_PLAYER_BULLETS);
            if (newBulletIndex != -1) {
                bullets[newBulletIndex] = player.shoot();
            }
        }
    } else if (buttonShoot == HIGH && isShooting == 1) {
        // Shoot only if button was released before
        isShooting = 0;
    }

    // Clear stripe blank
    anim.clearStripe();

    // Show how many levels are done in level bar
    levelBarOverPos = anim.showLevelBar(NUM_LEDS - 1, level - 1);

    // Animate collected hearts
    healthBarHeart = anim.showHeartsCollected(healthBarHeart, player.lives - 1);
    healthBarHeart.animateColor();
    healthBarOverPos = healthBarHeart.pos;

    // Animate collectable hearts
    for (int i = 0; i < MAX_HEARTS; i++) {
        if (hearts[i].pos != -1) {
            if (hearts[i].justCollected) {
                hearts[i].color = anim.fadeOutColor(hearts[i].color, blankPixel);
                if (hearts[i].color == blankPixel) hearts[i] = Heart();
            } else {
                if (hearts[i].getsCollected(player.pos)) {
                    player.lives++;
                    hearts[i].justCollected = true;
                    hearts[i].collected = true;
                } else {
                    hearts[i].animateColor();
                    LED_STRIPE[hearts[i].pos] = hearts[i].color;
                }
            }
        }
    }

    // Set enemy pixels and move them, check if lost
    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i].pos != -1) {
            // Player got hit
            if (player.getsHit(enemies[i].pos)) {
                player.lives--;
                player.pos -= 1; // Set back 1 so enemy is visible
                lostLive = true;
            } else {
                enemies[i].moveIrregularly(player.pos);
                // Remove if moved behind health bar
                if (enemies[i].pos < healthBarOverPos) {
                    enemies[i] = Enemy();
                }

                if (enemies[i].enemyType == 'shooting') {
                    // Shoot only if enemy is in front of player
                    if (enemies[i].pos >= player.pos) {
                        int newBulletIndex = findFreeBulletSpace(bullets, MAX_PLAYER_BULLETS, MAX_ENEMY_BULLETS);
                        if (newBulletIndex != -1) {
                            bullets[newBulletIndex] = enemies[i].shoot();
                        }
                    }
                }
            }

            // Make damaged enemy brighter and use breathing effect
            enemies[i].animateColor();
            LED_STRIPE[enemies[i].pos] = enemies[i].color;
        }
    }

    // Set bullet pixels and move them
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].pos != -1) {
            if (bullets[i].stopped == true) {
                bullets[i].color = anim.fadeOutColor(bullets[i].color, blankPixel);
                if (bullets[i].color == blankPixel) bullets[i] = Bullet();

            } else {
                bullets[i].move();

                if (bullets[i].shotBy == 'player'){
                    // Remove enemy and bullet on collision (enemy in bullet range)
                    for (int j = 0; j < enemyCount; j++) {
                        if (enemies[j].pos != -1 && enemies[j].getsHit(bullets[i].pos, bulletSpeed)) {
                            enemies[j].lives--;
                            player.bulletCount--;

                            if (enemies[j].lives > 0) {
                                bullets[i] = Bullet();
                            } else {
                                // Draw bullet on top of killed enemy
                                enemies[j] = Enemy();
                                bullets[i].stopped = true;
                            }
                            break;
                        }
                    }
                } else if (bullets[i].shotBy == 'enemy') {
                    if (player.getsHit(bullets[i].pos)) {
                        player.lives--;
                        lostLive = true;
                        bullets[i].stopped = true;
                    }
                }
                // Fade out bullet if moved too far or out of strip
                
                if (bullets[i].movedTooFar(healthBarOverPos, levelBarOverPos)) {
                    bullets[i].stopped = true;
                    if (bullets[i].shotBy == 'player') {
                        player.bulletCount--;
                    }
                }
            }
            LED_STRIPE[bullets[i].pos] = bullets[i].color;
        }
    }

    // Set player pixel
    player.animateColor();
    LED_STRIPE[player.pos] = player.color;

    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
}

int findFreeBulletSpace(Bullet bullets[], int minIndex, int len) {
    for (int i = minIndex; i < minIndex + len; i++) {
        if (bullets[i].pos = -1) {
            return i;
        }
    }
    return -1;
}

int levelHasNShootingEnemies(int lvl) {
    if (lvl > 0 && lvl % 3 == 0) {
        return 1;
    }
    return 1; //TODO return 0
}

int levelHasNHearts(int lvl, int lives) {
    int nhearts = 1; 
    // Only give away hearts even level and if space in array
    if (lvl % 2 == 0 && MAX_HEARTS - lives >= nhearts) {
        return nhearts;
    }
    return nhearts; //TODO return 0
}

int getFirstEnemyPos(Enemy enemies[], int enemyCount) {
    int firstEnemyPos = NUM_LEDS - 1;
    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i].pos != -1 && enemies[i].pos < firstEnemyPos) {
            firstEnemyPos = enemies[i].pos;
        }
    }
    return firstEnemyPos;
}

// Serial.print("Enemy Move "); Serial.print(chanceToMove); Serial.print(" "); Serial.print(shouldMove); Serial.print("\n");
