#include <Arduino.h>
#include <HardwareSerial.h>

#include "uart.h"
#include "crsf_protocol.h"

// Set 'true' to actvate debug output
bool debugEnabled = true; 
bool infoEnabled = true; 
bool debugPacketsEnabled = false; 
bool debugVoltageEnabled = false; // analog read voltage from ADS1115 (adc)
// portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

HardwareSerial dbout(0);
HardwareSerial elrs(1);