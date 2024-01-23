#include <FastLED.h>

#define LED_PIN     5
#define NUM_LEDS    300
#define LED_TYPE    WS2811
#define BRIGHTNESS  64
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
#define UPDATES_PER_SECOND 100
CRGB blankPixel = CRGB(0, 0, 0);

#define BUTTON_1_PIN 10
#define BUTTON_2_PIN 11
#define BUTTON_3_PIN 12

int buttonState1 = 0;
int buttonState2 = 0;
int buttonState3 = 0;

int playerPos = 0;
int cStep = 5;
int cThreshold = 5;
int cNTreshold = 255 - cThreshold;
CRGB playerColor = CRGB(cThreshold, cThreshold, cNTreshold);

typedef struct bullet_ {
  int pos;
  int distanceMoved;
  CRGB color;
} bullet;

#define MAX_BULLETS 5
bullet* bullets[MAX_BULLETS];
CRGB bulletColor = CRGB(208,70,208);
int bulletSpeed = 1;
int bulletRange = 30;

typedef struct enemy_ {
  int pos;
  int lives;
  CRGB color;
} enemy;

#define MAX_ENEMY_COUNT 40
// int enemies[MAX_ENEMY_COUNT];
CRGB enemyBaseColor = CRGB(0, 255, 0);
enemy* enemies[MAX_ENEMY_COUNT];
int enemySpeed = 1;
int enemySlowness = 7;
int enemyRange = 70;
int enemyLives = 4;

int level = 1;
int spawnpoint = 4;
int enemyCount = 2 * level;
// ============================================

void setup() {
  pinMode(BUTTON_1_PIN, INPUT);
  pinMode(BUTTON_2_PIN, INPUT);
  pinMode(BUTTON_3_PIN, INPUT);

  Serial.begin(9600);

  delay(3000); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  setupLevel();
}

bool isShooting = 0;
bool lost = 0;

void loop() {
  if (lost == 1) {
    lost = 0;
    loseAnimation(playerPos);
    level = 1;
    setupLevel();
  }
  
  buttonState1 = digitalRead(BUTTON_1_PIN);
  buttonState2 = digitalRead(BUTTON_2_PIN);
  buttonState3 = digitalRead(BUTTON_3_PIN);
  
  // Input controls
  if (buttonState1 == HIGH) {
    playerPos += 1;
    if (playerPos >= 300) {
      winAnimation(299);
      level++;
      setupLevel();
    }

  } else if (buttonState2 == HIGH) {
    if (playerPos - 1 >= 0) playerPos -= 1;

  } else if (buttonState3 == LOW && isShooting == 0) {
    isShooting = 1;

    // Make new bullet
    for (int i = 0; i < MAX_BULLETS; ++i) {
      if (bullets[i] == NULL) {
        bullet* new_bullet = calloc (1, sizeof(bullet));
        if (new_bullet == NULL) endGame();
        new_bullet -> pos = playerPos;
        new_bullet -> distanceMoved = 0;
        new_bullet -> color = playerColor;

        bullets[i] = new_bullet;
        break;
      }
   }

  } else if (buttonState3 == HIGH && isShooting == 1) {
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
    if (bullets[i] != NULL) {

      // Remove enemy and bullet on collision
      for (int j = 0; j < enemyCount; j++) {
          if (enemies[j] != NULL && enemies[j] -> pos == bullets[i] -> pos) {
            if (enemies[j] -> lives > 1) {
              enemies[j] -> lives -= 1;
              int brightnessReduction = 20;
              (enemies[j] -> color).r += (enemyLives - enemies[j] -> lives) * brightnessReduction;
              (enemies[j] -> color).b += (enemyLives - enemies[j] -> lives) * brightnessReduction;
              // Serial.println((enemies[i] -> color).g);
            } else {
              free(enemies[j]);
              enemies[i] = NULL;
            }

            free(bullets[i]);
            bullets[i] = NULL;
            break;
          }
      }

      bullets[i] -> pos += bulletSpeed;
      bullets[i] -> distanceMoved += bulletSpeed;

      // Remove bullet if moved too far or out of strip
      if (bullets[i] -> distanceMoved > bulletRange || bullets[i] -> pos >= 300) {
        free(bullets[i]);
        bullets[i] = NULL;
        continue;
      }

      leds[bullets[i] -> pos] = bullets[i] -> color;
    }
  }

  // Set enemy pixels and move them, check if lost
  for (int i = 0; i < enemyCount; i++) {
    if (enemies[i] != NULL) {
      // Game over
      if (enemies[i] -> pos == playerPos) {
        lost = 1;
      } else if (enemies[i] -> pos - playerPos <= enemyRange) {
        // Move enemy irregularly
        if (random(0, enemySlowness) <= 1) enemies[i] -> pos -= enemySpeed;
      }
      
      leds[enemies[i] -> pos] = enemies[i] -> color;
    }
  }

  // printLEDs(leds);
  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);

}
