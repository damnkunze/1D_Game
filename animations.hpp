namespace A {
    #define levelBarStepsCount 3
    CRGB levelBarColorCoding[levelBarStepsCount] = {CRGB(10, 10, 10), CRGB(8, 41, 120), CRGB(120, 71, 0)};
    int levelBarUnits[levelBarStepsCount] = {1, 5, 10};

    class Animation {
        public:

        Animation() {};
        
        CRGB fadeOutColor(CRGB color, CRGB blankPixel) {
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

        void clearStripe() {
            for (int i = 0; i < NUM_LEDS; i++) {
                LED_STRIPE[i] = blankPixel;
            }
        }

        void entryAnimation(int startPos, int endPos, CRGB color) {
            int slowness = 3;
            for (int i = startPos; i <= endPos; i++) {
                LED_STRIPE[i] = color;
                FastLED.show();
                FastLED.delay(1000 / (UPDATES_PER_SECOND / slowness));
            }
            for (int i = startPos; i <= endPos - 1; i++) {
                LED_STRIPE[i] = blankPixel;
                FastLED.show();
                FastLED.delay(1000 / (UPDATES_PER_SECOND / slowness));
            }
        }

        void winAnimation(Player player, int pos) {
            int repeat = 4; //times. Has to be uneven for smoothness
            int onTime = 2;
            bool toggle = 1;

            for (int i = 0; i < repeat; i++) {
                for (int j = 0; j < onTime; j++) {
                    if (toggle) {
                        player.animateColor();
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

        void loseAnimation(Player player, int to) {
            int setbackSpeed = 3;

            while (player.pos - setbackSpeed > to) {
                player.animateColor();
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
    };
}