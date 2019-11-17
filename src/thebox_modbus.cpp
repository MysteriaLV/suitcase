#include <ESP8266.h>
#include <Modbus.h>
#include <ModbusIP_ESP8266AT.h>
#include "freeMemory.h"
#include "thebox_modbus.h"

ESP8266 wifi(Serial, 115200);

//ModbusIP object
ModbusIP mb;

void(* resetFunc) (void) = 0; //declare reset function @ address 0

// Action handler. Add all your actions mapped by action_id in rs485_node of Lua script
void process_actions() {
    if (mb.Hreg(ACTIONS) == 0)
        return;

    switch (mb.Hreg(ACTIONS)) {
        case 1 : // Put here code for Reset
            Serial.println(F("[Reset] action fired"));
            digitalWrite(LED_BUILTIN, HIGH);
            resetFunc();
            break;
        case 2 : // Put here code for Force_complete
            Serial.println("[Force_complete] action fired");
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
    Serial.println(freeMemory());

    if (!wifi.restart())
        Serial.println(F("ESP8266 restart broken!"));

    //Config Modbus IP
    mb.config(wifi, F("AliensRoom"), F("********"));

    mb.addHreg(ACTIONS, 0);
    mb.addHreg(COMPLETE, 0);
    mb.addHreg(EMPTY_UPLOAD, 0);
    mb.addHreg(INCOMPLETE_UPLOAD, 0);
}

void modbus_set(word event, word value) {
    mb.Hreg(event, value);
}

void modbus_loop() {
    mb.task();              // not implemented yet: mb.Hreg(TOTAL_ERRORS, mb.task());
    process_actions();
}
