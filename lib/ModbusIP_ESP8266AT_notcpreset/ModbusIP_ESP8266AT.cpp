/*
    ModbusIP_ESP8266.cpp - Source for Modbus IP ESP8266 AT Library
    Copyright (C) 2015 André Sarmento Barbosa
*/
#include "ModbusIP_ESP8266AT.h"


ModbusIP::ModbusIP() {

}

void ModbusIP::config(ESP8266 &wifi, String ssid, String password) {
    //Check errors
    //Enable serial log
    wifi.setOprToStationSoftAP();
    wifi.joinAP(ssid, password);
    wifi.enableMUX();
    wifi.startTCPServer(MODBUSIP_PORT);
    wifi.setTCPServerTimeout(MODBUSIP_TIMEOUT);
    _wifi = &wifi;
}

void ModbusIP::task(Stream &log) {

    uint8_t buffer[128] = {0};
    uint8_t mux_id;
    uint32_t len = _wifi->recv(&mux_id, buffer, sizeof(buffer), 100);

    if (len > 0) {
//        log.print(F("Received from: "));
//        log.println(mux_id);

        int i = 0;
        while (i < 7) {
            _MBAP[i] = buffer[i];
            i++;
        }

        _len = _MBAP[4] << 8 | _MBAP[5];
        _len--; // Do not count with last byte from MBAP
        if (_MBAP[2] != 0 || _MBAP[3] != 0) {
            log.println(F("Not a MODBUSIP packet"));
            return;  //Not a MODBUSIP packet
        }
        if (_len > MODBUSIP_MAXFRAME) {
            log.println(F("Length is over MODBUSIP_MAXFRAME"));
            return;  //Length is over MODBUSIP_MAXFRAME
        }

        _frame = (byte *) malloc(_len);
        i = 0;
        while (i < _len) {
            _frame[i] = buffer[7 + i];  //Forget MBAP and take just modbus pdu
            i++;
        }

/*        byte fcode = _frame[0];
        word field1 = (word) _frame[1] << 8 | (word) _frame[2];
        word field2 = (word) _frame[3] << 8 | (word) _frame[4];
        log.print(fcode);
        log.print(' ');
        log.print(field1);
        log.print(' ');
        log.println(field2);*/

        this->receivePDU(_frame);

        if (_reply != MB_REPLY_OFF) {
            //MBAP
            _MBAP[4] = (_len + 1) >> 8;     //_len+1 for last byte from MBAP
            _MBAP[5] = (_len + 1) & 0x00FF;

            buffer[4] = _MBAP[4];
            buffer[5] = _MBAP[5];

            i = 0;
            while (i < _len) {
                buffer[i + 7] = _frame[i];
                i++;
            }
            _wifi->send(mux_id, buffer, _len + 7);
        }

        if (_wifi->releaseTCP(mux_id)) {
//            log.print(F("release tcp ok: "));
//            log.println(mux_id);
        } else {
            log.print(F("release tcp err: "));
            log.println(mux_id);
        }
        free(_frame);
        _len = 0;
    }
}
