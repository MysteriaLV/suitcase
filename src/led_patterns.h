//
// Created by PVeretennikovs on 16-Mar-19.
//

#ifndef THE_BOX_LED_PATTERNS_H
#define THE_BOX_LED_PATTERNS_H

#include <FastLED.h>

FASTLED_USING_NAMESPACE

#define DATA_PIN    7
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define BRIGHTNESS          96
#define NUM_LEDS    24
CRGB leds[NUM_LEDS];


// List of patterns to cycle through.  Each is defined as a separate function below.
enum {NOT_READY, UPLOADING, UPLOAD_READY};
typedef void (*PatternList[])();

uint8_t gCurrentPatternNumber = NOT_READY; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

uint8_t upload_progress = 1;


void rainbow() {
    // FastLED's built-in rainbow generator
    fill_rainbow(leds, NUM_LEDS, gHue, 7);
}


void addGlitter(fract8 chanceOfGlitter) {
    if (random8() < chanceOfGlitter) {
        leds[random16(NUM_LEDS)] += CRGB::White;
    }
}

void rainbowWithGlitter() {
    // built-in FastLED rainbow, plus some random sparkly glitter
    rainbow();
    addGlitter(80);
}

void confetti() {
    // random colored speckles that blink in and fade smoothly
    fadeToBlackBy(leds, NUM_LEDS, 10);
    int pos = random16(NUM_LEDS);
    leds[pos] += CHSV(gHue + random8(64), 200, 255);
}

void sinelon() {
    // a colored dot sweeping back and forth, with fading trails
    fadeToBlackBy(leds, NUM_LEDS, 10);
    int pos = beatsin16(60, 0, upload_progress - 1);
    leds[pos] += CHSV(gHue, 255, 192);
}

void not_ready() {
    EVERY_N_MILLISECONDS(10) {
        fadeToBlackBy(leds, NUM_LEDS, 1);
    }
    EVERY_N_SECONDS(10) {
        fill_solid(leds, NUM_LEDS, CRGB::DarkRed);
    }
}

void bpm() {
    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
    uint8_t BeatsPerMinute = 62;
    CRGBPalette16 palette = PartyColors_p;
    uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
    for (int i = 0; i < NUM_LEDS; i++) { //9948
        leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
    }
}

void circle() {
    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
    CRGBPalette16 palette = LavaColors_p;
    uint8_t beat = (uint8_t ) (GET_MILLIS()*1.5) % 255;
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette(palette, (gHue % 100) + (i * 2), beat - gHue + (i * 10));
    }
}

void juggle() {
    // eight colored dots, weaving in and out of sync with each other
    fadeToBlackBy(leds, NUM_LEDS, 20);
    byte dothue = 0;
    for (int i = 0; i < 8; i++) {
        leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
        dothue += 32;
    }
}

PatternList gPatterns = {not_ready, sinelon, circle};


#endif //THE_BOX_LED_PATTERNS_H
