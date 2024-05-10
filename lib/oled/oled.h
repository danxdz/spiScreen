#ifndef OLED_H /* include guards */
#define OLED_H

#include "crsf_protocol.h"
#include "crsf.h"

#include <SPI.h>
#include <Wire.h>

#define display_DC 25
#define display_CS 13

class Oled
{

public:
  void Init();

  void testFillScreen();

  void printText(rc_input_t rcInput);

  void Clear();

  void setFont(const uint8_t font[]);
  void drawProgressBar(int x, int y, int width, int height, int value, int maxValue, int minValue, int centerValue);
  void clearProgressBar(int x, int y, int width, int height);
  void drawProgressBarFrames(int x, int y, int width, int height);

  void Println(char *tmp);
  void PrintCenter(char *tmp, uint8_t y=50, uint8_t size=1, uint16_t color= 0xFFFF, uint16_t bgcolor=0x0000);
  static void PrintRight(char *tmp);
  static void PrintRight(uint8_t y, char *tmp);
  void PrintLeft(char *tmp);
  void PrintLeft(uint8_t y, char *tmp);
  void PrintLoad(char *tmp);
  void setMainScreen(char *name, crsfLinkStatistics_t LinkStatistics, uint8_t bpkts, uint8_t gpkts);
  void setMainMenuItems();
  static void selectOptionMainMenu();
  static void setSubMenuItems();
};

extern Oled oled;
#endif
