#include <Automaton.h>
#include "led_patterns.h"

Atm_led dna_led[6];
Atm_button dna_sensor[3];
Atm_step puzzle_controller;

#define DNA_GREEN(N) (N*2)
#define DNA_RED(N) (N*2+1)

void validate_dna(int idx, int v, int up) {
    if (puzzle_controller.state() != 0) {
        Serial.println(puzzle_controller.state());
        return;
    }
    if (v) {
        dna_led[DNA_GREEN(idx)].on();
        dna_led[DNA_RED(idx)].off();
    } else {
        dna_led[DNA_GREEN(idx)].off();
        dna_led[DNA_RED(idx)].on();
    }

    if (dna_sensor[0].state() == Atm_button::PRESSED &&
        dna_sensor[1].state() == Atm_button::PRESSED &&
        dna_sensor[2].state() == Atm_button::PRESSED)
        puzzle_controller.trigger(Atm_step::EVT_STEP);
}

void puzzle_init(int idx, int v, int up) {
    dna_led[DNA_RED(0)].on();
    dna_led[DNA_RED(1)].on();
    dna_led[DNA_RED(2)].on();
}

void puzzle_all_dna_inserted(int idx, int v, int up) {
    gCurrentPatternNumber = UPLOADING;
    dna_led[DNA_GREEN(0)].lead(200).blink(100, 1500).start();
    dna_led[DNA_GREEN(1)].lead(100).blink(100, 1500).start();
    dna_led[DNA_GREEN(2)].lead(000).blink(100, 1500).start();
}

void puzzle_dna_uploaded(int idx, int v, int up) {

}

void setup() {
    Serial.begin(115200);

    delay(1000); // 1 second delay for recovery

    // tell FastLED about the LED strip configuration
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

    // set master brightness control
    FastLED.setBrightness(BRIGHTNESS);

#ifndef MY_TEST_MODE
    dna_led[0].begin(PIN_A0);
    dna_led[1].begin(PIN_A1);

    dna_led[2].begin(PIN_A2);
    dna_led[3].begin(PIN_A3);

    dna_led[4].begin(PIN_A4);
    dna_led[5].begin(PIN_A5);

    dna_sensor[0].begin(4).onPress(validate_dna, 0).onRelease(validate_dna, 0).trace(Serial);
    dna_sensor[1].begin(5).onPress(validate_dna, 1).onRelease(validate_dna, 1);
    dna_sensor[2].begin(6).onPress(validate_dna, 2).onRelease(validate_dna, 2);
#else
    dna_led[0].begin(PIN_A0).blink(1000, 300).start();
    dna_led[1].begin(PIN_A1).blink(1000, 500).start();

    dna_led[2].begin(PIN_A2).blink(1000, 300).start();
    dna_led[3].begin(PIN_A3).blink(1000, 500).start();

    dna_led[4].begin(PIN_A4).blink(1000, 300).start();
    dna_led[5].begin(PIN_A5).blink(1000, 500).start();

    int btn_pin = 4;
    int btn_num = 0;
    int led_num = 0;
    dna_sensor[btn_num++].begin(btn_pin++)
            .onPress(dna_led[led_num++], Atm_led::EVT_TOGGLE_BLINK)
            .onRelease(dna_led[led_num++], Atm_led::EVT_TOGGLE_BLINK);
    dna_sensor[btn_num++].begin(btn_pin++)
            .onPress(dna_led[led_num++], Atm_led::EVT_TOGGLE_BLINK)
            .onRelease(dna_led[led_num++], Atm_led::EVT_TOGGLE_BLINK);
    dna_sensor[btn_num].begin(btn_pin)
            .onPress(dna_led[led_num++], Atm_led::EVT_TOGGLE_BLINK)
            .onRelease(dna_led[led_num], Atm_led::EVT_TOGGLE_BLINK);
#endif

    puzzle_controller.begin()
            .onStep(0, puzzle_init)
            .onStep(1, puzzle_all_dna_inserted)
            .onStep(2, puzzle_dna_uploaded).trace(Serial);

    // Start
    puzzle_controller.trigger( puzzle_controller.EVT_STEP );

}


void loop() {
    automaton.run();

    EVERY_N_MILLISECONDS(10) {
        gPatterns[gCurrentPatternNumber]();
        FastLED.show();
    }

    // do some periodic updates
    EVERY_N_MILLISECONDS(20) { gHue++; } // slowly cycle the "base color" through the rainbow
}