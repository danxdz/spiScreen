
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"

#include "uart.h"


#include "crsf.h"
#include "oled.h"
#include "ads.h"
#include "sticks.h"
#include "gpio.h"
#include "ui_1wire_keypad.h"

TaskHandle_t outputTaskHandler;

Prefs preferences;

ADSreader ads_reader;

Keypad keypad;

rc_input_t rcInput;

UI_input_t UIinput;

void OutputTask(void *pvParameters)
{
  // setup channels
  for (uint8_t i = 0; i < CRSF_MAX_CHANNEL; i++)
  {
    rcChannels[i] = RC_CHANNEL_MIN;
  }
  dbout.println("OutputTask started");
  for (;;)
  {
    int bt = readUIkeypad(&UIinput);
    if (bt != 0)
    {
      dbout.printf("key:%i\n", bt);
    }
    keypad.read_keypad(&UIinput);

    ads_reader.readInputs(&rcInput);
    
    //convert bt to char
    char tmp[10];
    sprintf(tmp, "%i", bt);

    oled.printText(rcInput);
    
    oled.PrintCenter( tmp,150, 6);

  }

}
    
void setup() {

  initGpio();
  
  dbout.begin(115200);
  delay(2000);

  oled.Init();
  ads_reader.init();
  preferences.init(&rcInput, &UIinput);
  dbout.println(F("initiated system"));
  
  xTaskCreatePinnedToCore(
      OutputTask,
      "OutputTask",
      10000,
      NULL,
      1,
      &outputTaskHandler,
      0); /* pin task to core 0 */
  delay(500);

}

void loop(void) {
}

