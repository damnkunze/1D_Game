#include <FastLED.h>
#include <math.h>

#define LED_PIN    5
#define NUM_LEDS   300
#define LED_TYPE   WS2811
#define BRIGHTNESS 64
#define COLOR_ORDER GRB
CRGB LED_STRIPE[NUM_LEDS];
#define UPDATES_PER_SECOND 100
CRGB blankPixel = CRGB(0, 0, 0);

#define BUTTON_1_PIN 10
#define BUTTON_2_PIN 11
#define BUTTON_3_PIN 12
int buttonMoveForwards = 0;
int buttonMoveBackwards = 0;
int buttonShoot = 0;

int playerSpeed = 1;
int playerStartLives = 1;
int playerSpawnpoint = 5;
CRGB playerStartColor = CRGB(250, 5, 5); // CRGB(random(5, 245), random(5, 245), random(5, 250));

int bulletSpeed = 1;
int bulletRange = 30;

int enemyLives = 4;
int enemySpeed = 1;
int enemyMinSpawnpoint = 70;
int enemySlowness = 200;
int enemyAnimationStrength = 100;
int enemyAnimationLength = 10;
CRGB enemyBaseColor = CRGB(0, 255 - enemyAnimationStrength, 0);

float heartAnimationStrengthCollectable = 90;
float heartAnimationLengthCollectable = 17;
float heartAnimationStrengthCollected = 30;
float heartAnimationLengthCollected = 25;
CRGB heartBaseColor = CRGB(255 - heartAnimationStrengthCollectable, 0, 0);
// ============================================

class Player {
public:
    int pos;
    CRGB color;
    int lives;
    int velocity;
    Player(int pos, CRGB color, int lives) {
        this -> pos = pos;
        this -> color = color;
        this -> lives = lives;
        this -> velocity = 0;
    }
};

class Bullet {
public:
    int pos;
    int velocity;
    CRGB color;
    int distanceMoved;
    bool stopped;
    // Empty constructor for array struct initialisation
    Bullet() {}
    Bullet(int pos, int velocity, CRGB color, bool stopped) {
        this -> pos = pos;
        this -> velocity = velocity;
        this -> color = color;
        this -> distanceMoved = 0;
        this -> stopped = stopped;
    }
};

class Enemy {
public:
    int pos;
    int lives;
    CRGB color;
    float currPointInAnimation;
    int currAnimationColorChange;
    Enemy() {}
    Enemy(int pos, int lives, CRGB color, float currPointInAnimation, int currAnimationColorChange) {
        this -> pos = pos;
        this -> lives = lives;
        this -> color = color;
        this -> currPointInAnimation = currPointInAnimation;
        this -> currAnimationColorChange = currAnimationColorChange;
    }
};

class Heart {
public:
    int pos;
    CRGB color;
    float currPointInAnimation;
    int currAnimationColorChange;
    bool justCollected;
    Heart() {}
    Heart(int pos, CRGB color, float currPointInAnimation, int currAnimationColorChange, bool justCollected) {
        this -> pos = pos;
        this -> color = color;
        this -> currPointInAnimation = currPointInAnimation;
        this -> currAnimationColorChange = currAnimationColorChange;
        this -> justCollected = justCollected;
    }
};

// ============================================

Player player = Player(playerSpawnpoint, playerStartColor, playerStartLives);
Bullet placeholderBullet = Bullet(-1, 0, blankPixel, true);
Enemy placeholderEnemy = Enemy(-1, 0, blankPixel, 0, 0);
Heart placeholderHeart = Heart(-1, blankPixel, 0, 0, false);

#define MAX_ENEMIES 20
Enemy enemies[MAX_ENEMIES];

#define MAX_BULLETS 3
Bullet bullets[MAX_BULLETS];

// collectable hearts:
#define MAX_HEARTS 5
Heart hearts[MAX_HEARTS];
int levelHasNHearts(int lvl, int lives) {
    int nhearts = 1; 
    // Only give away hearts even level and if space in array
    if (lvl % 2 == 0 && MAX_HEARTS - lives >= nhearts) {
        return nhearts;
    }
    return nhearts; //TODO return 0
}

int level = 1;
#define levelBarStepsCount 3
CRGB levelBarColorCoding[levelBarStepsCount] = {CRGB(10, 10, 10), CRGB(8, 41, 120), CRGB(120, 71, 0)};
int levelBarUnits[levelBarStepsCount] = {1, 5, 10};
int levelBarOverPos = NUM_LEDS - 1;

int healthBarOverPos = 0;
Heart healthBarHeart = placeholderHeart;

int enemyCount = 2 * level;
int lastLevel = 16;

void resetGame() {
    level = 1;
    player = Player(playerSpawnpoint, playerStartColor, playerStartLives);

    // Fill with empty placeholder hearts
    for (int i = 0; i < MAX_HEARTS; i++) {
        hearts[i] = placeholderHeart;
    }
}

void startLevel() {
    enemyCount = min(2 * level, MAX_ENEMIES);
    player.pos = playerSpawnpoint + healthBarOverPos;

    // Fill arrays with empty placeholder bullet and enemy objects
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i] = placeholderBullet;
    }
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i] = placeholderEnemy;
    }

    // Spawn enemies
    for (int i = 0; i <= enemyCount; i++) {
        // random() is exclusive max
        int pos = random(playerSpawnpoint + enemyMinSpawnpoint + 1, levelBarOverPos - 1);
        // Add random color change to base color
        float currPointInAnimation = random(-enemyAnimationLength + 1, enemyAnimationLength);
        enemies[i] = Enemy(pos, enemyLives, enemyBaseColor, currPointInAnimation, 0);
    }

    // Spawn collectable hearts
    int nhearts = levelHasNHearts(level, player.lives);
    for (int i = 0; i < nhearts; i++) {
        if (hearts[i].pos == -1) {
            // random() is exclusive max
            int pos = random(NUM_LEDS * 0.6, levelBarOverPos - 1);
            int currPointInAnimation = random(-heartAnimationLengthCollectable + 1, heartAnimationLengthCollectable); 
            hearts[i] = Heart(pos, heartBaseColor, currPointInAnimation, 0, false);
        }
    }
    // Set health bar heart
    int currPointInAnimation = random(-heartAnimationLengthCollected + 1, heartAnimationLengthCollected); 
    healthBarHeart = Heart(0, heartBaseColor, currPointInAnimation, 0, false);

    entryAnimation(playerSpawnpoint, player.color, blankPixel);
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
            loseAnimation(0);
            resetGame();
            startLevel();
        } else {
            int firstEnemyPos = getFirstEnemyPos();
            // Respawn out of range of enemies and pos >= 0
            int respawnPoint = max(firstEnemyPos - enemyMinSpawnpoint - 1, 0);
            loseAnimation(respawnPoint);
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
    
    buttonMoveForwards = digitalRead(BUTTON_2_PIN);
    buttonMoveBackwards = digitalRead(BUTTON_1_PIN);
    buttonShoot = digitalRead(BUTTON_3_PIN);
    
    // Input controls
    player.velocity = 0;
    if (buttonMoveBackwards == HIGH) {
        if (player.pos - playerSpeed >= healthBarOverPos) player.velocity = -playerSpeed;

    } else if (buttonMoveForwards == HIGH) {
        player.velocity = playerSpeed;
    }
    player.pos += player.velocity;
    
    if (buttonShoot == LOW && isShooting == 0) {
        isShooting = 1;

        // Make new bullet
        for (int i = 0; i < MAX_BULLETS; ++i) {
            if (bullets[i].pos == -1) {
                // First bullet animation spot is on player and >= 0:
                int pos = max(player.pos - bulletSpeed, 0);
                // Velocity is different when player is moving and > 0
                int velocity = max(player.velocity, 0) + bulletSpeed;
                bullets[i] = Bullet(pos, velocity, player.color, false);
                break;
            }
     }

    } else if (buttonShoot == HIGH && isShooting == 1) {
        // Shoot only if button was released before
        isShooting = 0;
    }

    // Clear stripe blank
    clearStripe();
    
    // Set player pixel
    player.color = animatePlayerColor(player.color);
    LED_STRIPE[player.pos] = player.color;
    // printColor(player.color);

    // Set enemy pixels and move them, check if lost
    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i].pos != -1) {
            // Player got hit
            if (enemies[i].pos <= player.pos && enemies[i].pos >= player.pos - playerSpeed) {
                player.lives--;
                player.pos -= 1; // Set back 1 so enemy is visible
                lostLive = true;
            } else {
                // Move enemy irregularly 
                float distanceToPlayer = enemies[i].pos - player.pos;
                float enemySlownessBias = 0.02;
                float chanceToMove = pow(distanceToPlayer / NUM_LEDS, 2) + enemySlownessBias; // (x / 300)^2 + 0.02
                // 1: dont move, 0: move
                float shouldMove = random(0, 1 + enemySlowness * chanceToMove);
                // Serial.print("Enemy Move "); Serial.print(chanceToMove); Serial.print(" "); Serial.print(shouldMove); Serial.print("\n");
                
                if (shouldMove <= 1) {
                    enemies[i].pos -= enemySpeed;
                }
            }

            // Make damaged enemy brighter and use breathing effect
            enemies[i] = animateEnemyColor(enemies[i], enemyBaseColor, enemyAnimationStrength);
            LED_STRIPE[enemies[i].pos] = enemies[i].color;
        }
    }

    // Set bullet pixels and move them
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].pos != -1) {
            if (bullets[i].stopped == true) {
                bullets[i].color = fadeOutColor(bullets[i].color);
                if (bullets[i].color == blankPixel) bullets[i] = placeholderBullet;

            } else {
                bullets[i].pos += bullets[i].velocity;
                bullets[i].distanceMoved += bullets[i].velocity;

                // Remove enemy and bullet on collision (enemy in bullet range)
                for (int j = 0; j < enemyCount; j++) {
                    if (enemies[j].pos != -1 && enemies[j].pos <= bullets[i].pos && enemies[j].pos >= bullets[i].pos - bulletRange) {
                        enemies[j].lives--;

                        // Draw bullet on top of killed enemy
                        if (enemies[j].lives > 0) {
                            bullets[i] = placeholderBullet;
                        } else {
                            enemies[j] = placeholderEnemy;
                            bullets[i].stopped = true;
                        }
                        break;
                    }
                }

                // Fade out bullet if moved too far or out of strip
                if (bullets[i].pos != -1 && (bullets[i].distanceMoved > bulletRange || bullets[i].pos >= levelBarOverPos)) {
                    if (bullets[i].pos > levelBarOverPos) bullets[i].pos = levelBarOverPos;
                    bullets[i].stopped = true;
                }
            }

            LED_STRIPE[bullets[i].pos] = bullets[i].color;
        }
    }

    // Animate collectable hearts
    for (int i = 0; i < MAX_HEARTS; i++) {
        if (hearts[i].pos != -1) {
            if (hearts[i].pos <= player.pos && hearts[i].pos >= player.pos - playerSpeed) {
                player.lives++;
                hearts[i] = placeholderHeart;
            } else {
                hearts[i] = animateHeartColor(hearts[i], heartAnimationLengthCollectable, heartAnimationStrengthCollectable);
                LED_STRIPE[hearts[i].pos] = hearts[i].color;
            }
        }
    }

    // Show how many levels are done in level bar
    levelBarOverPos = showLevelBar(NUM_LEDS - 1, level - 1);

    // Animate collected hearts
    healthBarHeart = showHeartsCollected(healthBarHeart, player.lives - 1);
    healthBarHeart = animateHeartColor(healthBarHeart, heartAnimationLengthCollected, heartAnimationStrengthCollected);
    healthBarOverPos = healthBarHeart.pos;

    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
}
