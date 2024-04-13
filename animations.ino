// #include "headers/animations.h"
// using namespace H;

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

void clearStripe() {
    for (int i = 0; i < NUM_LEDS; i++) {
        LED_STRIPE[i] = blankPixel;
    }
}

void entryAnimation(int from, int to, CRGB color, CRGB blankPixel) {
    int slowness = 3;
    for (int i = from; i <= to; i++) {
        LED_STRIPE[i] = color;
        FastLED.show();
        FastLED.delay(1000 / (UPDATES_PER_SECOND / slowness));
    }
    for (int i = from; i <= to - 1; i++) {
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

void loseAnimation(int pos, int to, CRGB color, CRGB blankPixel) {
    int setbackSpeed = 3;

    while (pos - setbackSpeed > to) {
        LED_STRIPE[pos] = blankPixel;
        pos -= setbackSpeed;
        LED_STRIPE[pos] = color;
        
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
            colorIndex += 5;
        }
        FastLED.show();
        FastLED.delay(1000 / UPDATES_PER_SECOND);
    }
}