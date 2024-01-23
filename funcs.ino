void endGame() {
  Serial.println("GAME CRASHED");
  exit(1);
}

void setupLevel() {
  // Clear arrays
  for (int i = 0; i <= MAX_BULLETS; i++) {
    free(bullets[i]);
    bullets[i] = NULL;
  }
  for (int i = 0; i <= MAX_ENEMY_COUNT; i++) {
    free(enemies[i]);
    enemies[i] = NULL;
  }

  enemyCount = 2 * level;
  playerPos = spawnpoint;

  // Spawn enemies
  for (int i = 0; i <= enemyCount; i++) {
    enemy* new_enemy = calloc(1, sizeof(enemy));
    if (new_enemy == NULL) endGame();
    new_enemy -> pos = random(spawnpoint + enemyRange, NUM_LEDS);
    new_enemy -> lives = enemyLives;
    new_enemy -> color = enemyBaseColor;

    enemies[i] = new_enemy;
  }
  
  // Entry animation
  for (int i = 0; i <= spawnpoint; i++) {
    if (i > 0) leds[i - 1] = blankPixel;
    leds[i] = playerColor;
    FastLED.show();
    FastLED.delay(1000 / (UPDATES_PER_SECOND / 3));
  }
}

CRGB getPlayerColor(CRGB color) {
  if (color.r + cStep <= cNTreshold && color.g <= cThreshold) {
    color.r += cStep;
    color.b -= cStep;
  } else if (color.g + cStep <= cNTreshold && color.b <= cThreshold) {
    color.g += cStep;
    color.r -= cStep;
  } else if (color.b + cStep <= cNTreshold) {
    color.b += cStep;
    color.g -= cStep;
  }

  // printColor(color);
  return color;
}

void clearStripe() {
  for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = blankPixel;
  }
}

void winAnimation(int pos) {
  int repeat = 8; //times //has to be uneven for smoothness
  int onTime = 4;
  bool toggle = 1;

  for (int i = 0; i < repeat; i++) {
    for (int j = 0; j < onTime; j++) {
      if (toggle) {
        playerColor = getPlayerColor(playerColor);
        leds[pos] = playerColor;
      } else {
        leds[pos] = blankPixel;

      }

      FastLED.show();
      FastLED.delay(1000 / (UPDATES_PER_SECOND / 5));
    }
    toggle = !toggle;
  }
}

void loseAnimation(int pos) {
  int setbackSpeed = 3;

  while (playerPos > spawnpoint) {
    leds[playerPos] = blankPixel;
    playerPos -= setbackSpeed;
    leds[playerPos] = playerColor;
    
    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
  }
}

void printColor(CRGB color) {
  Serial.print("("); Serial.print(color.r); Serial.print(" "); Serial.print(color.g); Serial.print(" "); Serial.print(color.b); Serial.println(")");
}

void printLEDs(CRGB leds[]) {
  Serial.println("");
  for (int i = 0; i < NUM_LEDS; i++) {
    Serial.print(leds[i].g); 
  }
  Serial.println("");
}