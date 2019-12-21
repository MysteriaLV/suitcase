#include <ESP8266.h>
#include <Modbus.h>
#include <ModbusIP_ESP8266AT.h>
#include "thebox_modbus.h"

ESP8266 wifi(Serial, 115200);
#define Serial SerialDebug

//ModbusIP object
ModbusIP mb;

// Action handler. Add all your actions mapped by action_id in rs485_node of Lua script
void process_actions() {
    if (mb.Hreg(ACTIONS) == 0)
        return;

    switch (mb.Hreg(ACTIONS)) {
        case 1 : // Put here code for Reset
            Serial.println(F("[Reset] action fired"));
            digitalWrite(LED_BUILTIN, HIGH);
            puzzle_controller.trigger(Atm_step::EVT_LINEAR);
            puzzle_controller.trigger(puzzle_controller.EVT_STEP);
            break;
        case 2 : // Put here code for Force_step
            Serial.println("[Force_step] action fired");
            digitalWrite(LED_BUILTIN, LOW);
            puzzle_controller.trigger(Atm_step::EVT_STEP);
            break;
        default:
            break;
    }

    // Signal that action was processed
    mb.Hreg(ACTIONS, 0);
}

void modbus_setup() {
    if (!wifi.kick())
        Serial.println(F("ESP8266 failed! Only pins 1,2 should be on."));

    //Config Modbus IP
    mb.config(wifi, F("AliensRoom"), F("************"));

    Serial.print("IP: ");
    Serial.println(wifi.getLocalIP().c_str());

    mb.addHreg(ACTIONS, 0);
    mb.addHreg(COMPLETE, 0);
    mb.addHreg(EMPTY_UPLOAD, 0);
    mb.addHreg(INCOMPLETE_UPLOAD, 0);
}

void modbus_set(word event, word value) {
    mb.Hreg(event, value);
}

void modbus_loop() {
    mb.task(SerialDebug);              // not implemented yet: mb.Hreg(TOTAL_ERRORS, mb.task());
    process_actions();
}
