
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
#include "menus.h"

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

    //oled.printText(rcInput);
    
    //get min and max values for the 4 sticks created in preferences
    int sticks_max[4] = {preferences.sticks.aileronMax, preferences.sticks.elevatorMax, preferences.sticks.throttleMax, preferences.sticks.rudderMax};
    int sticks_min[4] = {preferences.sticks.aileronMin, preferences.sticks.elevatorMin, preferences.sticks.throttleMin, preferences.sticks.rudderMin};
    int sticks_center[4] = {preferences.sticks.aileronCenter, preferences.sticks.elevatorCenter, preferences.sticks.throttleCenter, preferences.sticks.rudderCenter};
    int sticks_connected[4] = {preferences.sticks.aileronConnected, preferences.sticks.elevatorConnected, preferences.sticks.throttleConnected, preferences.sticks.rudderConnected};
    //char 12 for take space
    char buffer[6];
    // create list with controls rcInput.aileron, rcInput.throttle, rcInput.rudder, rcInput.elevator
    int rcInputList[4] = {rcInput.aileron, rcInput.elevator, rcInput.throttle, rcInput.rudder};
    int list[4] = {0, 0, 0, 0};

    int listSize = 0;
    int valueMapped[4] = {0, 0, 0, 0};

    for (int i = 0; i < 4; i++)
    {
      if (sticks_connected[i])
      {
        // if stick is connected add value to list
        list[listSize] = i;
        listSize++;
        valueMapped[i] = map(rcInputList[i], sticks_min[i], sticks_max[i], -100, 100);
      }
    }

    for (int i = 0; i < listSize; i++)
    {
      // map each value to int 0 - 100
      //int value = map(rcInputList[list[i]], sticks_min[list[i]], sticks_max[list[i]], -100, 100);
      sprintf(buffer, "%5d", valueMapped[list[i]]);
      oled.PrintCenter(buffer, 50+(i*20), 2);
    }

    oled.PrintCenter( tmp,150, 6);

    oled.PrintCircle(120, 120, 100, 0xFFFF);

    if (entered == -1)
      { // main menu -1
        if (params_loaded < crsf_devices[0].number_of_params)
        {
          //char *load = (char *)hdr_str_cb(menuItems); // TODO: Rename hdr_str_cb to something more descriptive
          char *load = (char *)"testing";
          dbout.printf("hdr:%s\n", load);
          dbout.printf("params_loaded:%u\n", params_loaded);

          oled.PrintLoad(load);

          if (crsf_devices[0].number_of_params)
          {
            if (crsf_devices[0].address == ADDR_RADIO)
            {
              dbout.println("address:radio");
            }
            else
            {
              // dbout.printf("Menu address: 0x%x - num par: %u : next_p:%u\n", crsf_devices[0].address, crsf_devices[0].number_of_params, next_param);
              if (next_param > 0)
              {
                // next_chunk = 0;
                // delay(250);
                // CRSF_read_param(next_param, 0, ELRS_ADDRESS);
              }
            }
          }
          // delay(2000);
          // end not all params
        }
        else
        { // else (if all parameters) load main menu
          // dbout.println("main menu loaded...");
          oled.setMainMenuItems();
        }
      }
      else if (entered == -2)
      { // show idle screen
        // dbout.printf("idle screen\n");
        oled.setMainScreen(
            crsf_devices[0].name,
            LinkStatistics,
            local_info.bad_pkts,
            local_info.good_pkts);
      } // end idle screen
      else if (entered <= -10)
      { // click on mainmenu item to select option
        // db_out.printf("main select option\n");
        Oled::selectOptionMainMenu();
      }
      else if (entered >= 0)
      {
        Oled::setSubMenuItems();
      }
    } // end main loop for

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

