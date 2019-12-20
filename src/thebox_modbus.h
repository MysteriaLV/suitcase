//
// Created by PVeretennikovs on 20-Oct-19.
//

#ifndef THE_BOX_THEBOX_MODBUS_H
#define THE_BOX_THEBOX_MODBUS_H

#include <Automaton.h>
#include <SoftwareSerial.h>

extern Atm_step puzzle_controller;

extern void modbus_setup();
extern void modbus_loop();
extern void modbus_set(word event, word value);

extern SoftwareSerial SerialDebug; // RX, TX

//////////////// registers of THE-BOX ///////////////////
enum {
    // The first register starts at address 0
    ACTIONS,      // Always present, used for incoming actions

    // Any registered events, denoted by 'triggered_by_register' in rs485_node of Lua script, 1 and up
    COMPLETE,
    EMPTY_UPLOAD,
    INCOMPLETE_UPLOAD,

    TOTAL_ERRORS     // leave this one, error counter
};

#endif //THE_BOX_THEBOX_MODBUS_H
