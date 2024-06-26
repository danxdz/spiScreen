#include "Arduino.h"
#include "config.h"
#include "gpio.h"
#include "ui_buttons.h"
#include "crsf.h"
#include "uart.h"
#include "ads.h"

void initGpio()
{

  // analogReference(EXTERNAL);
  pinMode(ANALOG_IN_PIN_AUX1, INPUT_PULLDOWN);
  pinMode(ANALOG_IN_PIN_AUX2, INPUT_PULLUP);
  // pinMode(DIGITAL_PIN_LED, OUTPUT);//LED
  // pinMode(DIGITAL_PIN_BUZZER, OUTPUT);//
  // digitalWrite(DIGITAL_PIN_BUZZER, LOW);
  // batteryVoltage=7.0;

#if defined(TARGET_ESP32_S)
  analogReadResolution(12); // 4096
#endif
#if defined(TARGET_PICO32)
  analogReadResolution(12); // 4096
#endif
}

const int numButtons = 5;                                        // Número total de botões
double buttonThresholds[numButtons] = {0.5, 0.8, 1.1, 1.7, 2.8}; // Limiares dos botões
double ReadVoltage(byte pin)
{
  double reading = analogRead(pin); // Reference voltage is 3v3 so maximum reading is 3v3 = 4095 in range 0 to 4095
  //dbout.println(reading);
  if (reading < 1 || reading > 4095)
    return 0;
  // return -0.000000000009824 * pow(reading,3) + 0.000000016557283 * pow(reading,2) + 0.000854596860691 * reading + 0.065440348345433;
  return -0.000000000000016 * pow(reading, 4) + 0.000000000118171 * pow(reading, 3) - 0.000000301211691 * pow(reading, 2) + 0.001109019271794 * reading + 0.034143524634089;
} // Added an improved polynomial, use either, comment out as required

int mapToButton(double value)
{
  if (value <= buttonThresholds[0])
  {
    return 0; // value is below first threshold, return 0
  }
  else if (value >= buttonThresholds[numButtons - 1])
  {
    return numButtons; // value is above top threshold, return numButtons
  }
  else
  {
    for (int i = 0; i < numButtons - 1; i++)
    {
      if (value >= buttonThresholds[i] && value < buttonThresholds[i + 1])
      {
        return i + 1; // Valor entre os limiares, retorna o botão correspondente
      }
    }
  }
  return -1; // Valor inválido
}

int readUIkeypad (UI_input_t *UI_input)
{
  //delay(150);
  double buttonValue = ReadVoltage(UI_KEY_PAD);
  //dbout.println(buttonValue, 1);

  UI_input->key = mapToButton(buttonValue);

   
  //dbout.printf(" :: %.3f", buttonValue);
  //dbout.printf(" :: %i\n", UI_input->key);
  return UI_input->key;
}

void gpioReadInputs(rc_input_t *rc_input)
{
  rc_input->aileron = analogRead(ANALOG_IN_PIN_AILERON);
  rc_input->elevator = analogRead(ANALOG_IN_PIN_ELEVATOR);
  rc_input->throttle = analogRead(ANALOG_IN_PIN_THROTTLE);
  rc_input->rudder = analogRead(ANALOG_IN_PIN_RUDDER);
  //rc_input->aux1 = analogRead(ANALOG_IN_PIN_AUX1);
  //rc_input->aux2 = analogRead(ANALOG_IN_PIN_AUX2);
  // rc_input->aux3     = analogRead(ANALOG_IN_PIN_AUX3);
  // rc_input->aux4     = analogRead(ANALOG_IN_PIN_AUX4);
}

void gpioMixer(rc_input_t *rc_input)
{
  rc_input->aux1 = (rc_input->aux1 >= 2047) ? 4095 : 0;
}
