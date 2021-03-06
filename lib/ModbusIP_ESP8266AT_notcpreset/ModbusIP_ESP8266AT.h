/*
    ModbusIP_ESP8266AT.h - Header for Modbus IP ESP8266 AT Library
    Copyright (C) 2015 Andr� Sarmento Barbosa
*/
#include <Arduino.h>
#include <Modbus.h>
#include <ESP8266.h>

#ifndef MODBUSIP_ESP8266AT_H
#define MODBUSIP_ESP8266AT_H

#define MODBUSIP_PORT 	  502
#define MODBUSIP_MAXFRAME 200
#define MODBUSIP_TIMEOUT    3

class ModbusIP : public Modbus {
    private:
        byte _MBAP[7];
        ESP8266* _wifi;
    public:
        ModbusIP();
        void config(ESP8266 &wifi, String ssid, String password);
        void task(Stream &log);
};

#endif //MODBUSIP_ESP8266AT_H

