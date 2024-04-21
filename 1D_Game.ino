#include <FastLED.h>
#include <math.h>

#include <Bluepad32.h>

#define LED_PIN    22
#define NUM_LEDS   300

#define UPDATES_PER_SECOND 500
CRGB blankPixel = CRGB(0, 0, 0);
CRGB LED_STRIPE[NUM_LEDS];

ControllerPtr controller;

float speed = 1;


float pos = 0;
float velocity = 0;
int bullet_pos = -1;

void onConnectedController(ControllerPtr ctl) {
    controller = ctl; 
}

void onDisconnectedController(ControllerPtr ctl) {
    controller = nullptr; 
}

void dumpGamepad(ControllerPtr ctl) {
    Serial.printf(
        "idx=%d, dpad: 0x%02x, buttons: 0x%04x, joystick L: %4d, %4d, joystick R: %4d, %4d, brake: %4d, throttle: %4d, "
        "misc: 0x%02x, l1: %d, l2: %d, r1: %d, r2: %d, thumbL: %d, thumbR: %d, topLeft: %d, topRight: %d\n",
        controller->index(),        // Controller Index
        controller->dpad(),         // D-pad
        controller->buttons(),      // bitmask of pressed buttons
        controller->axisX(),        // (-511 - 512) left X Axis
        controller->axisY(),        // (-511 - 512) left Y axis
        controller->axisRX(),       // (-511 - 512) right X axis
        controller->axisRY(),       // (-511 - 512) right Y axis
        controller->brake(),        // (0 - 1023): brake button
        controller->throttle(),     // (0 - 1023): throttle (AKA gas) button
        controller->miscButtons(),  // bitmask of pressed "misc" buttons
        controller->l1(),           
        controller->l2(),
        controller->r1(),
        controller->r2(),
        controller->thumbL(),
        controller->thumbR(),
        controller->topLeft(),
        controller->topRight()
    );
}
bool shooting = false;

void setup() {
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(115200);
    delay(1000); // power-up safety delay

    FastLED.addLeds<WS2811, LED_PIN, GRB>(LED_STRIPE, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(64);

    Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    // Setup the Bluepad32 callbacks
    BP32.setup(&onConnectedController, &onDisconnectedController);

    // "device factory reset", or similar.
    // BP32.forgetBluetoothKeys();
}

void loop() {
    // fetch all the controllers' data.
    bool dataUpdated = BP32.update();
    if (dataUpdated && controller && controller->isConnected() && controller->hasData() && controller->isGamepad()) {
        float axisMax = 512;
        float axisYLowThreshold = 50;
        float axisYHighThreshold = axisMax - 150;

        if (controller->axisY() < -axisYLowThreshold) { // Forward
            if (controller->axisY() < -axisYHighThreshold) {
                velocity = speed;

            } else {
                float axisIntensity = (abs(controller->axisY()) - axisYLowThreshold) / (axisMax - axisYLowThreshold);
                velocity = speed * axisIntensity;
            }

        } else if (controller->axisY() > axisYLowThreshold) {  // Backward
            if (controller->axisY() > axisYHighThreshold) {
                velocity = -1 * speed;

            } else {
                float axisIntensity = (abs(controller->axisY()) - axisYLowThreshold) / (axisMax - axisYLowThreshold);
                velocity = -1 * speed * axisIntensity;
            }

        } else if (controller->x() || controller->b()) { // Forward
            velocity = 1;

        } else if (controller->a()) { // Backward
            velocity = -1;

        } else {
            velocity = 0;
        }

        if (controller->l1() || controller->r1()) { // Shoot
            bullet_pos = pos;
        }

        if (controller->y()) { // Fun stuff
            static int led = 0;
            led++;
            controller->setPlayerLEDs(led & 0x0f);
        
            controller->setRumble(128, 128);
            controller->playDualRumble(0, 1250, 128, 128);
        }

        // dumpGamepad(controller);
    }
    
    pos += velocity;

    if (pos < 0) pos = 0;
    if (pos > NUM_LEDS - 1) pos = NUM_LEDS - 1;

    // Clear stripe
    for (int i = 0; i < NUM_LEDS; i++) {
        LED_STRIPE[i] = blankPixel;
    }

    if (bullet_pos >= NUM_LEDS) bullet_pos = -1;
    if (bullet_pos >= 0) {
        LED_STRIPE[bullet_pos] = CRGB(5, 250, 5);
        bullet_pos++;
    }

    LED_STRIPE[(int) pos] = CRGB(250, 5, 5);

    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
}
