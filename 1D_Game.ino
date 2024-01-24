#include <FastLED.h>

#define LED_PIN         5
#define NUM_LEDS        300
#define LED_TYPE        WS2811
#define BRIGHTNESS    64
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
#define UPDATES_PER_SECOND 100
CRGB blankPixel = CRGB(0, 0, 0);

#define BUTTON_1_PIN 10
#define BUTTON_2_PIN 11
#define BUTTON_3_PIN 12

int buttonMoveForwards = 0;
int buttonMoveBackwards = 0;
int buttonShoot = 0;

int playerPos = 0;
int cStep = 5;
int cThreshold = 5;
int cNTreshold = 255 - cThreshold;
CRGB playerColor = CRGB(cThreshold, cThreshold, cNTreshold);

#define MAX_BULLETS 3
int bullets[MAX_BULLETS];
CRGB bulletColor = CRGB(208,70,208);
int bulletSpeed = 1;
int bulletRange = 30;

#define MAX_ENEMY_COUNT 40
int enemies[MAX_ENEMY_COUNT];
CRGB enemyBaseColor = CRGB(0, 255, 0);
int enemySpeed = 1;
int enemySlowness = 7;
int enemyRange = 70;

int level = 1;
int spawnpoint = 5;
int enemyCount = 2 * level;
// ============================================

void startLevel() {
  // Clear arrays
  for (int i = 0; i <= MAX_BULLETS; i++) {
    bullets[i] = -1;
  }
  for (int i = 0; i <= MAX_ENEMY_COUNT; i++) {
    enemies[i] = -1;
  }

  enemyCount = 2 * level;
  playerPos = spawnpoint;

  // Spawn enemies
  for (int i = 0; i <= enemyCount; i++) {
    enemies[i] = random(spawnpoint + enemyRange, NUM_LEDS);
  }
}

void setup() {
    pinMode(BUTTON_1_PIN, INPUT);
    pinMode(BUTTON_2_PIN, INPUT);
    pinMode(BUTTON_3_PIN, INPUT);
    Serial.begin(9600);

    delay(3000); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);

    startLevel();
    entryAnimation(spawnpoint, playerColor, blankPixel);
}

bool isShooting = 0;
bool lost = 0;

void loop() {
    if (lost == 1) {
        lost = 0;
        loseAnimation(playerPos);
        level = 1;
        startLevel();
        // entryAnimation(spawnpoint, playerColor, blankPixel);
    }
    
    buttonMoveForwards = digitalRead(BUTTON_1_PIN);
    buttonMoveBackwards = digitalRead(BUTTON_2_PIN);
    buttonShoot = digitalRead(BUTTON_3_PIN);
    
    // Input controls
    if (buttonMoveForwards == HIGH) {
        playerPos += 1;
        if (playerPos >= 300) {
            winAnimation(299);
            level++;
            startLevel();
        }

    } else if (buttonMoveBackwards == HIGH) {
        if (playerPos - 1 >= 0) playerPos -= 1;

    } else if (buttonShoot == LOW && isShooting == 0) {
        isShooting = 1;

        // Make new bullet
        for (int i = 0; i < MAX_BULLETS; ++i) {
            if (bullets[i] == -1) {
                bullets[i] = playerPos;
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
    playerColor = getPlayerColor(playerColor);
    leds[playerPos] = playerColor;

    // Set bullet pixels and move them
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i] != -1) {
            bullets[i] += bulletSpeed;

            // Remove enemy and bullet on collision (collision might be 1 pixel off)
            for (int j = 0; j < enemyCount; j++) {
                    if (enemies[j] != -1 && (enemies[j] == bullets[i] || enemies[j] == bullets[i] + 1)) {
                        enemies[j] = -1;
                        bullets[i] = -1;
                        break;
                    }
            }

            // Remove bullet if moved too far or out of strip
            if (bullets[i] == -1 || bullets[i] - playerPos > bulletRange || bullets[i] >= 300) {
                bullets[i] = -1;
            } else {
                leds[bullets[i]] = bulletColor;
            }
        }
    }

    // Set enemy pixels and move them, check if lost
    for (int i = 0; i < enemyCount; i++) {
        if (enemies[i] != -1) {
            // Game over
            if (enemies[i] == playerPos) {
                lost = 1;
            } else if (enemies[i] - playerPos <= enemyRange) {
                // Move enemy irregularly
                if (random(0, enemySlowness) <= 1) enemies[i] -= enemySpeed;
            }
            
            leds[enemies[i]] = enemyBaseColor;
        }
    }
    // printIntArray(bullets, MAX_BULLETS);
    // printLEDs(leds);
    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);

}
