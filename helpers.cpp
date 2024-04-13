#include "headers/helpers.h"

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

int constrainInt(int value, int min, int max) {
    if (value < min) {
        return min;
    } else if (value > max) {
        return max;
    }
    return value;
}

CRGB addValuesToColor(CRGB color, int red, int green, int blue) {
    CRGB newColor;
    newColor.r = constrainInt(color.r + red, 0, 255);
    newColor.g = constrainInt(color.g + green, 0, 255);
    newColor.b = constrainInt(color.b + blue, 0, 255);
    return newColor;
}

void printIntArray(int array[], int len) {
    Serial.println("");
    for (int i = 0; i < len; i++) {
        Serial.print(array[i]); Serial.print(", "); 
    }
    Serial.println("");
}

void printLEDs(CRGB leds[], int length) {
    Serial.println("");
    for (int i = 0; i < length; i++) {
        Serial.print(leds[i].g); 
    }
    Serial.println("");
}