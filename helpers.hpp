namespace Helpers {
    void endGame() {
        Serial.println("GAME CRASHED");
        delay(1000);
        exit(1);
    }

    CRGB addColors(CRGB color1, CRGB color2) {
        CRGB newColor;
        newColor.r = color1.r + color2.r;
        newColor.g = color1.g + color2.g;
        newColor.b = color1.b + color2.b;
        return newColor;
    }

    CRGB addValuesToColor(CRGB color, int red, int green, int blue) {
        CRGB newColor;
        newColor.r = color.r + red;
        newColor.g = color.g + green;
        newColor.b = color.b + blue;
        return newColor;
    }

    void printIntArray(int array[], int len) {
        Serial.println("");
        for (int i = 0; i < len; i++) {
            Serial.print(array[i]); Serial.print(", "); 
        }
        Serial.println("");
    }

    void printLEDs(CRGB leds[]) {
        Serial.println("");
        for (int i = 0; i < NUM_LEDS; i++) {
            Serial.print(leds[i].g); 
        }
        Serial.println("");
    }
}