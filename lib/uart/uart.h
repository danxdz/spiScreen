#ifndef UART_H /* include guards */
#define UART_H

#include "Arduino.h"
#include "crsf_protocol.h"

extern HardwareSerial elrs;
extern HardwareSerial dbout;

extern bool debugEnabled; 
extern bool infoEnabled; 
extern bool debugPacketsEnabled;
extern bool debugVoltageEnabled;

#endif
