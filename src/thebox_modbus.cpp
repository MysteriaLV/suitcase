#include <ESP8266.h>
#include <Modbus.h>
#include <ModbusIP_ESP8266AT.h>
#include "freeMemory.h"

ESP8266 wifi(Serial, 115200);

//ModbusIP object
ModbusIP mb;

//////////////// registers of THE-BOX ///////////////////
enum {
    // The first register starts at address 0
    ACTIONS,      // Always present, used for incoming actions

    // Any registered events, denoted by 'triggered_by_register' in rs485_node of Lua script, 1 and up
    MINOR_FAILURE,
    MAJOR_FAILURE,
    COMPLETE,


    TOTAL_ERRORS     // leave this one, error counter
};


// Action handler. Add all your actions mapped by action_id in rs485_node of Lua script
void process_actions() {
    if (mb.Hreg(ACTIONS) == 0)
        return;

    switch (mb.Hreg(ACTIONS)) {
        case 1 : // Put here code for Reset
            Serial.println(F("[Reset] action fired"));
            digitalWrite(LED_BUILTIN, HIGH);
            break;
        case 2 : // Put here code for Complete
            Serial.println(F("[Complete] action fired"));
            digitalWrite(LED_BUILTIN, LOW);
            break;
        default:
            break;
    }

    // Signal that action was processed
    mb.Hreg(ACTIONS, 0);
}

void modbus_setup() {
    Serial.println(freeMemory());

    //Config Modbus IP
    mb.config(wifi, F("AliensRoom"), F("********"));
    Serial.print(F("FW Version:"));
    Serial.println(wifi.getVersion().c_str());
    Serial.print(F("IP: "));
    Serial.println(wifi.getLocalIP().c_str());


    mb.addHreg(ACTIONS, 0);
    mb.addHreg(MINOR_FAILURE, 0);
    mb.addHreg(MAJOR_FAILURE, 0);
    mb.addHreg(COMPLETE, 0);
}


void modbus_loop() {
    mb.task();              // not implemented yet: mb.Hreg(TOTAL_ERRORS, mb.task());
    process_actions();
}
