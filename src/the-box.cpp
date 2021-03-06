#include <Automaton.h>
#include <SoftwareSerial.h>
#include "thebox_modbus.h"
#include "led_patterns.h"
#include "freeMemory.h"

Atm_led dna_led[6];
Atm_button dna_sensor[3], big_red_button;
Atm_step puzzle_controller;
Atm_timer timer;

#define DNA_GREEN(N) (N*2)
#define DNA_RED(N) (N*2+1)

SoftwareSerial SerialDebug(10, 11); // RX, TX

void attempt_upload(int idx, int v, int up) {
    if (puzzle_controller.state() != 0)
        return;

    for (int i = 0; i < 3; i++)
        if (dna_sensor[i].state() != Atm_button::PRESSED)
            dna_led[DNA_RED(i)].lead(0).blink(100, 100, 10).start();

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
    } else {
        dna_led[DNA_GREEN(idx)].off();
    }
}

void puzzle_init(int idx, int v, int up) {
    for (int i = 0; i < 3; i++) {
        dna_led[DNA_RED(i)].lead(0).off();
        dna_led[DNA_GREEN(i)].lead(0).off();

        dna_sensor[i].trigger(Atm_button::EVT_RELEASE);
    }

    gCurrentPatternNumber = NOT_READY;
    upload_progress = 1;
    timer.stop();

    modbus_set(COMPLETE, 0);
    modbus_set(INCOMPLETE_UPLOAD, 0);
    modbus_set(EMPTY_UPLOAD, 0);
}

void puzzle_all_dna_inserted(int idx, int v, int up) {
    modbus_set(COMPLETE, 0);
    modbus_set(INCOMPLETE_UPLOAD, 0);
    modbus_set(EMPTY_UPLOAD, 0);

    gCurrentPatternNumber = UPLOADING;
    dna_led[DNA_GREEN(0)].lead(200).blink(100, 1500).start();
    dna_led[DNA_GREEN(1)].lead(100).blink(100, 1500).start();
    dna_led[DNA_GREEN(2)].lead(000).blink(100, 1500).start();

    upload_progress = 1;
    timer.stop();

    timer.begin(1000) // Each step takes 1 second
            .repeat(NUM_LEDS)
            .onTimer([](int idx, int v, int up) {
                upload_progress++;
            })
            .onFinish(puzzle_controller, puzzle_controller.EVT_STEP)
            .start();
}

void puzzle_dna_uploaded(int idx, int v, int up) {
    modbus_set(COMPLETE, 1);
    gCurrentPatternNumber = UPLOAD_READY;
    upload_progress = NUM_LEDS;
    timer.stop();

    dna_led[DNA_GREEN(0)].off();
    dna_led[DNA_GREEN(1)].off();
    dna_led[DNA_GREEN(2)].off();

    dna_led[DNA_RED(0)].off();
    dna_led[DNA_RED(1)].off();
    dna_led[DNA_RED(2)].off();
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

void setup() {
    Serial.begin(115200);
    SerialDebug.begin(115200);
    SerialDebug.println(F("Setup starting..."));
    SerialDebug.println(freeMemory());

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
            .onStep(2, puzzle_dna_uploaded);

    // Start
    puzzle_controller.trigger(puzzle_controller.EVT_STEP);

    modbus_setup();

    digitalWrite(LED_BUILTIN, HIGH);
    SerialDebug.println(F("Setup complete."));
}

void loop() {
    modbus_loop();
    automaton.run();

    EVERY_N_MILLISECONDS(10) {
        gPatterns[gCurrentPatternNumber]();
        FastLED.show();

        // do some periodic updates
        gHue++; // slowly cycle the "base color" through the rainbow
    }
}

#pragma clang diagnostic pop
