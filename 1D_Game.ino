#include <FastLED.h>
#include <math.h>

#define NUM_LEDS   300
#define LED_TYPE   WS2811
#define BRIGHTNESS 64
#define COLOR_ORDER GRB
#define UPDATES_PER_SECOND 100

#define LED_PIN    5
#define BUTTON_1_PIN 10
#define BUTTON_2_PIN 11
#define BUTTON_3_PIN 12

CRGB LED_STRIPE[NUM_LEDS];
CRGB blankPixel = CRGB(0, 0, 0);

// ============================================

// Some header files import each other (secured by include guards)
#include "headers/helpers.h"
#include "headers/Player.h"
#include "headers/Enemy.h"
#include "headers/Bullet.h"
#include "headers/Heart.h"
using namespace P;
using namespace E;
using namespace B;
using namespace H;

//#include "headers/animations.h"

// ============================================
int buttonMoveForwards = LOW;
int buttonMoveBackwards = LOW;
int buttonShoot = LOW;

Player player = Player();

#define MAX_ENEMIES 20
Enemy enemies[MAX_ENEMIES];

#define MAX_PLAYER_BULLETS 2
#define MAX_ENEMY_BULLETS 3

#define MAX_BULLETS MAX_PLAYER_BULLETS + MAX_ENEMY_BULLETS
Bullet bullets[MAX_BULLETS];

// collectable hearts:
#define MAX_HEARTS 5
Heart hearts[MAX_HEARTS];

int level = 1;
#define levelBarStepsCount 3
CRGB levelBarColorCoding[levelBarStepsCount] = {CRGB(10, 10, 10), CRGB(8, 41, 120), CRGB(120, 71, 0)};
int levelBarUnits[levelBarStepsCount] = {1, 5, 10};
int levelBarOverPos = NUM_LEDS - 1;

// Set heart that resembles health bar
Heart healthBarHeart = Heart(0, true);
int healthBarOverPos = 0;

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
    entryAnimation(healthBarOverPos, healthBarOverPos + playerSpawnpoint, player.color, blankPixel);
    player.pos = playerSpawnpoint + healthBarOverPos;
    
    enemyCount = min(2 * level, MAX_ENEMIES);

    healthBarHeart = showHeartsCollected(healthBarHeart, player.lives - 1);
    healthBarOverPos = healthBarHeart.pos;

    // Fill arrays with empty placeholder bullet and enemy objects
    for (int i = 0; i < MAX_BULLETS; i++) bullets[i] = Bullet();
    for (int i = 0; i < MAX_ENEMIES; i++) enemies[i] = Enemy();

    // Spawn enemies
    for (int i = 0; i <= enemyCount; i++) {
        if (i < levelHasNShootingEnemies(level, enemyCount)) {
            int pos = random(NUM_LEDS / 2, levelBarOverPos - 1);
            enemies[i] = Enemy(pos, 2); // shooting
        } else {
            // random() is exclusive max
            int pos = random(playerSpawnpoint + enemySpawnDist + 1, levelBarOverPos - 1);
            enemies[i] = Enemy(pos, 1); // basic
        }
    }

    // Spawn collectable hearts
    for (int i = 0; i < levelHasNHearts(level, player.lives, MAX_HEARTS); i++) {
        if (hearts[i].pos == -1) {
            // random() is exclusive max
            int pos = random(NUM_LEDS * 0.6, levelBarOverPos - 1);
            hearts[i] = Heart(pos, false);
        }
    }
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
            loseAnimation(player.pos, 0, player.color, blankPixel);
            resetGame();
            startLevel();
        } else {
            // Respawn out of range of enemies and pos >= 0
            int respawnPoint = max(player.pos - enemySpawnDist - 1, healthBarOverPos);
            loseAnimation(player.pos, respawnPoint, player.color, blankPixel);
            player.pos = respawnPoint;
        }
    }
    // Check if won
    if (player.pos >= levelBarOverPos) {
        winAnimation(levelBarOverPos);
        level++;
        if (level >= lastLevel) {
            amazingWinAnimation();
        }
        startLevel();
    }

    // Clear stripe blank
    clearStripe();
    
    buttonMoveForwards = digitalRead(BUTTON_2_PIN);
    buttonMoveBackwards = digitalRead(BUTTON_1_PIN);
    buttonShoot = digitalRead(BUTTON_3_PIN);
    
    // Input controls ===========================
    player.velocity = 0;
    if (buttonMoveBackwards == HIGH) {
        if (player.pos - playerSpeed >= healthBarOverPos) {
            player.velocity = -playerSpeed;
        }
    } else if (buttonMoveForwards == HIGH) {
        player.velocity = playerSpeed;
    }
    player.pos += player.velocity;
    
    if (buttonShoot == LOW && isShooting == 0) {
        isShooting = 1;
        // Make new bullet
        int newBulletIndex = findFreeBulletSpace(bullets, 0, MAX_PLAYER_BULLETS);
        if (player.bulletCount < MAX_PLAYER_BULLETS && newBulletIndex != -1) {
            bullets[newBulletIndex] = player.shoot();
            player.animateColor(3); // increase for animation speed
        }
    } else if (buttonShoot == HIGH && isShooting == 1) {
        // Shoot only if button was released before
        isShooting = 0;
    }
    
    // Bullets ===========================
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].pos == -1) continue;

        // Fade out until pixel is blank, then remove
        if (bullets[i].stopped == true) {
            if (bullets[i].fadeOut() == blankPixel) {
                bullets[i] = Bullet();
            }
        } else {
            bullets[i].move();
            if (bullets[i].shotBy == 1) { // player
                // Remove enemy and bullet on collision (enemy in bullet range)
                for (int j = 0; j < enemyCount; j++) {
                    if (enemies[j].getsHit((int) bullets[i].pos, bullets[i].velocity)) {
                        enemies[j].lives--;
                        player.bulletCount--;

                        // Draw bullet only on top of killed enemy
                        if (enemies[j].lives > 0) {
                            bullets[i] = Bullet();
                        } else {
                            enemies[j] = Enemy();
                            bullets[i].stopped = true;
                        }
                        break;
                    }
                }
                // Fade out bullet if moved too far or out of strip
                if (bullets[i].movedTooFar(playerBulletRange, healthBarOverPos, levelBarOverPos)) {
                    bullets[i].stopped = true;
                    player.bulletCount--;
                }
            } else if (bullets[i].shotBy == 2) { // enemy
                if (player.getsHit((int) bullets[i].pos) 
                    || bullets[i].movedTooFar(enemyBulletRange, healthBarOverPos, levelBarOverPos)) {
                    bullets[i].stopped = true;
                    enemies[bullets[i].enemyIndex].bulletIndex = -1;
                }
                if (player.getsHit((int) bullets[i].pos)) {
                    player.lives--;
                    lostLive = true;
                }
            }
        }
        LED_STRIPE[(int) bullets[i].pos] = bullets[i].color;
    }

    // collectable Hearts ===========================
    for (int i = 0; i < MAX_HEARTS; i++) {
        if (hearts[i].justCollected) {
            if (hearts[i].fadeOut() == blankPixel) {
                hearts[i] = Heart();
            }
        } else {
            if (hearts[i].getsCollected(player.pos, playerSpeed)) {
                player.lives++;
                hearts[i].justCollected = true;
                hearts[i].collected = true;
            } else {
                hearts[i].animateColor();
                LED_STRIPE[hearts[i].pos] = hearts[i].color;
            }
        }
    } 

    // Enemies ===========================
    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i].pos == -1) continue;

        // Player got hit
        if (player.getsHit(enemies[i].pos)) {
            player.lives--;
            player.pos -= 1; // Set back 1 so enemy is visible
            lostLive = true;
        } else {
            if (enemies[i].movedTooFar(healthBarOverPos, levelBarOverPos)) {
                enemies[i] = Enemy();
                continue;
            }

            if (enemies[i].enemyType == 1) { // basic
                enemies[i].moveIrregularly(player.pos, NUM_LEDS);

            } else if (enemies[i].enemyType == 2) { // shooting
                enemies[i].moveAwayIrregularly(player.pos, levelBarOverPos, NUM_LEDS);
                // Shoot only if enemy is in front of player
                int newBulletIndex = findFreeBulletSpace(bullets, MAX_PLAYER_BULLETS, MAX_ENEMY_BULLETS);
                if (newBulletIndex != -1 && enemies[i].shouldShoot(player.pos, NUM_LEDS)) {
                    bullets[newBulletIndex] = enemies[i].shoot();
                    // For referencing between enemy and bullet
                    enemies[i].bulletIndex = newBulletIndex;
                    bullets[newBulletIndex].enemyIndex = i;
                }
            }

            // Make damaged enemy brighter and use breathing effect
            enemies[i].animateColor();
            LED_STRIPE[enemies[i].pos] = enemies[i].color;
        }
    }

    // Show how many levels are done in level bar
    levelBarOverPos = showLevelBar(NUM_LEDS - 1, level - 1);

    // Animate collected hearts
    healthBarHeart.animateColor();
    healthBarHeart = showHeartsCollected(healthBarHeart, player.lives - 1);
    healthBarOverPos = healthBarHeart.pos;

    // Set player pixel
    player.animateColor(0); // increase for animation speed
    LED_STRIPE[player.pos] = player.color;

    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
}

int findFreeBulletSpace(Bullet bullets[], int minIndex, int len) {
    for (int i = minIndex; i < minIndex + len; i++) {
        if (bullets[i].pos == -1) {
            return i;
        }
    }
    return -1;
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

int levelHasNHearts(int lvl, int heartsGotten, int maxHearts) {
    int hearts = 1;
    // Only give away hearts if space in array
    if (lvl >= 2 && lvl % 2 == 0 && maxHearts - heartsGotten >= hearts) {
        return hearts;
    }
    return 0;
}

int levelHasNShootingEnemies(int lvl, int maxEnemies) {
    int enemies = 1;
    if (lvl >= 3 && enemies < maxEnemies) {
        return enemies;
    }
    return 0;
}