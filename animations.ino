
CRGB fadeOutColor(CRGB color) {
    float fadeDecrease = 100;
    int minAvgLight = 30;

    // Functions from FastLED library
    color = color.fadeToBlackBy(fadeDecrease);
    if (color.getAverageLight() <= minAvgLight) {
        return blankPixel;
    }
    return color;
}

Heart showHeartsCollected(Heart representingHeart, int hearts) {
    representingHeart.pos = 0;
    for (int i = hearts; i > 0 ; i--) {
        LED_STRIPE[representingHeart.pos] = representingHeart.color;
        representingHeart.pos++;
    }
    return representingHeart; 
}

int showLevelBar(int levelBarPos, int levelsToShow) {
    while (levelsToShow > 0) {
        for (int i = levelBarStepsCount - 1; i >= 0 ; i--) {
            if (levelsToShow - levelBarUnits[i] >= 0) {
                LED_STRIPE[levelBarPos] = levelBarColorCoding[i];
                levelsToShow -= levelBarUnits[i];
                levelBarPos--;
            }
        }
    }
    return levelBarPos;
}

// c for Color
int cStep = 5;
int minColor = 5;
int maxColor = 255 - minColor;
CRGB animatePlayerColor(CRGB color) {
    if (color.r + cStep <= maxColor && color.g <= minColor) {
        color.r += cStep;
        color.b -= cStep;
    } else if (color.g + cStep <= maxColor && color.b <= minColor) {
        color.g += cStep;
        color.r -= cStep;
    } else if (color.b + cStep <= maxColor) {
        color.b += cStep;
        color.g -= cStep;
    } else {
        color.r -= cStep;
        color.g -= cStep;
    }
    return color;
}

// Imitate breathing effect
Enemy animateEnemyColor(Enemy enemy, CRGB baseColor, int animationStrength) {
    float animationSpeed = 0.2;
    int damagedColorChange = 12;

    enemy.currPointInAnimation += animationSpeed;
    if (enemy.currPointInAnimation >= enemyAnimationLength) {
        enemy.currPointInAnimation = -enemyAnimationLength;
    }
    
    float animationFuncVal = max(0, -1 * pow(enemy.currPointInAnimation / 5 - 1, 4) + 1);
    // -(x / 5 - 1)^4 + 1
    int livesLost = enemyLives - enemy.lives;
    int changeGreen = animationStrength * animationFuncVal;
    int changeRedBlue = livesLost * damagedColorChange;
    enemy.color = addValuesToColor(baseColor, changeRedBlue, changeGreen, changeRedBlue);
    return enemy;
}

// Imitate heartbeat effect
Heart animateHeartColor (Heart heart, int animationFunctionRange, int animationStrength) {
    float animationSpeed = 1;

    heart.currPointInAnimation += animationSpeed;
    if (heart.currPointInAnimation >= animationFunctionRange) {
        heart.currPointInAnimation = -animationFunctionRange;
    }
    
    float animationFuncVal = max(0, -1 * pow(heart.currPointInAnimation / 7.07, 4) + pow(heart.currPointInAnimation / 5, 2));
    // -(x / 7.07)^4 + (x / 5)^2
    int changeRed = animationStrength * animationFuncVal;
    heart.color = addValuesToColor(heartBaseColor, changeRed, 0, 0);
    // Serial.print("Heart Colors: "); Serial.print(animationColorChangeRed); Serial.print(" "); Serial.print(0); Serial.print(" "); printColor(heart.color); Serial.print("\n");

    return heart;
}

void clearStripe() {
    for (int i = 0; i < NUM_LEDS; i++) {
        LED_STRIPE[i] = blankPixel;
    }
}

void entryAnimation(int pos, CRGB color, CRGB blankPixel) {
    int slowness = 3;
    for (int i = 0; i <= pos; i++) {
        LED_STRIPE[i] = color;
        FastLED.show();
        FastLED.delay(1000 / (UPDATES_PER_SECOND / slowness));
    }
    for (int i = 0; i <= pos - 1; i++) {
        LED_STRIPE[i] = blankPixel;
        FastLED.show();
        FastLED.delay(1000 / (UPDATES_PER_SECOND / slowness));
    }
}

void winAnimation(int pos) {
    int repeat = 4; //times. Has to be uneven for smoothness
    int onTime = 2;
    bool toggle = 1;

    for (int i = 0; i < repeat; i++) {
        for (int j = 0; j < onTime; j++) {
            if (toggle) {
                player.color = animatePlayerColor(player.color);
                LED_STRIPE[pos] = player.color;
            } else {
                LED_STRIPE[pos] = blankPixel;
            }

            FastLED.show();
            FastLED.delay(1000 / (UPDATES_PER_SECOND / 5));
        }
        toggle = !toggle;
    }
}

void loseAnimation(int to) {
    int setbackSpeed = 3;

    while (player.pos - setbackSpeed > to) {
        player.color = animatePlayerColor(player.color);
        LED_STRIPE[player.pos] = blankPixel;
        player.pos -= setbackSpeed;
        LED_STRIPE[player.pos] = player.color;
        
        FastLED.show();
        FastLED.delay(1000 / UPDATES_PER_SECOND);
    }
}

void amazingWinAnimation() {
    // Preset pallettes (sorted by niceness): PartyColors_p, LavaColors_p, RainbowColors_p, RainbowStripeColors_p, 
        // Ugly: OceanColors_p, CloudColors_p, ForestColors_p
    // Blending can be LINEARBLEND, NOBLEND
    int startIndex = 0;
    int brightness = 255;

    while (true) {
        startIndex++;

        int colorIndex = startIndex;
        for (int i = 0; i < NUM_LEDS; i++) {
            LED_STRIPE[i] = ColorFromPalette(PartyColors_p, colorIndex, brightness, LINEARBLEND);
            colorIndex += 3;
        }
        FastLED.show();
        FastLED.delay(1000 / UPDATES_PER_SECOND);
    }
}