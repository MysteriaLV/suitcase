#include <Automaton.h>
#include "led_patterns.h"
#include "thebox_modbus.h"

Atm_led dna_led[6];
Atm_button dna_sensor[3], big_red_button;
Atm_step puzzle_controller;
Atm_timer timer;

#define DNA_GREEN(N) (N*2)
#define DNA_RED(N) (N*2+1)

void attempt_upload(int idx, int v, int up) {
    if (puzzle_controller.state() != 0)
        return;

    for (int i = 0; i < 3; i++)
        if (dna_sensor[i].state() != Atm_button::PRESSED)
            dna_led[DNA_RED(i)].blink(100, 100, 10).start();

    if (dna_sensor[0].state() != Atm_button::PRESSED &&
        dna_sensor[1].state() != Atm_button::PRESSED &&
        dna_sensor[2].state() != Atm_button::PRESSED)
        modbus_set(EMPTY_UPLOAD, 1);


    if (dna_sensor[0].state() == Atm_button::PRESSED &&
        dna_sensor[1].state() == Atm_button::PRESSED &&
        dna_sensor[2].state() == Atm_button::PRESSED)
        puzzle_controller.trigger(Atm_step::EVT_STEP);
    else
        modbus_set(INCOMPLETE_UPLOAD, 1);
}


void validate_dna(int idx, int v, int up) {
    if (puzzle_controller.state() != 0)
        return;

    if (v) {
        dna_led[DNA_GREEN(idx)].on();
//        dna_led[DNA_RED(idx)].off();
    } else {
        dna_led[DNA_GREEN(idx)].off();
//        dna_led[DNA_RED(idx)].on();
    }
}

void puzzle_init(int idx, int v, int up) {
//    for (int i = 0; i < 3; i++)
//        dna_led[DNA_RED(i)].on();
}

void puzzle_all_dna_inserted(int idx, int v, int up) {
    modbus_set(COMPLETE, 1);

    gCurrentPatternNumber = UPLOADING;
    dna_led[DNA_GREEN(0)].lead(200).blink(100, 1500).start();
    dna_led[DNA_GREEN(1)].lead(100).blink(100, 1500).start();
    dna_led[DNA_GREEN(2)].lead(000).blink(100, 1500).start();

    timer.begin(1000 * 3) // Each step takes 3 seconds
            .repeat(NUM_LEDS)
            .onTimer([](int idx, int v, int up) {
                upload_progress++;
            })
            .onFinish(puzzle_controller, puzzle_controller.EVT_STEP)
            .start();
}

void puzzle_dna_uploaded(int idx, int v, int up) {
    modbus_set(COMPLETE, 2);
    gCurrentPatternNumber = UPLOAD_READY;

    for (int i = 0; i < 3; i++)
        dna_led[DNA_RED(i)].off();

    dna_led[DNA_GREEN(0)].lead(200).blink(300, 300).start();
    dna_led[DNA_GREEN(1)].lead(100).blink(300, 300).start();
    dna_led[DNA_GREEN(2)].lead(000).blink(300, 300).start();

    dna_led[DNA_RED(0)].lead(500).blink(300, 300).start();
    dna_led[DNA_RED(1)].lead(400).blink(300, 300).start();
    dna_led[DNA_RED(2)].lead(300).blink(300, 300).start();
}

void setup() {
    Serial.begin(115200);
    modbus_setup();

//    delay(1000); // 1 second delay for recovery

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

    dna_sensor[0].begin(4).onPress(validate_dna, 0).onRelease(validate_dna, 0);
    dna_sensor[1].begin(5).onPress(validate_dna, 1).onRelease(validate_dna, 1);
    dna_sensor[2].begin(6).onPress(validate_dna, 2).onRelease(validate_dna, 2);

    big_red_button.begin(3).onPress(attempt_upload);
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
    puzzle_controller.trigger(puzzle_controller.EVT_STEP);

}


void loop() {
    modbus_loop();
    automaton.run();

    EVERY_N_MILLISECONDS(10) {
        gPatterns[gCurrentPatternNumber]();
        FastLED.show();
    }

    // do some periodic updates
    EVERY_N_MILLISECONDS(10) { gHue++; } // slowly cycle the "base color" through the rainbow
}