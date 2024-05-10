#pragma once

#include <Preferences.h>
#include "crsf.h"
#include "ads.h"
#include "ui_buttons.h"

#define STICK_HIGH 15000
#define STICK_LOW 0
#define STICK_CENTER 7500

#define BAR_X 40
#define BAR_Y 100
#define BAR_WIDTH 30
#define BAR_HEIGHT 80

typedef struct
{
  int aileronMax;
  int aileronMin;
  int aileronCenter;
  bool aileronReversed = false;
  bool aileronCalibrated = false;
  bool aileronConnected = false;

  int elevatorMax;
  int elevatorMin;
  int elevatorCenter;
  bool elevatorReversed = false;
  bool elevatorCalibrated = false;
  bool elevatorConnected = false;

  int throttleMax;
  int throttleMin;
  int throttleCenter;
  bool throttleReversed = false;
  bool throttleCalibrated = false;
  bool throttleConnected = false;

  int rudderMax;
  int rudderMin;
  int rudderCenter;
  bool rudderReversed = false;
  bool rudderCalibrated = false;
  bool rudderConnected = false;

} stick_calibration_t;

class Prefs
{
private:
  Preferences preferences;
  ADSreader ads_reader;
  stick_calibration_t sticks;
  stick_calibration_t oldSticks;

  boolean saved_stick_calibration = false;
  int stickCalibrationCounter = 0;

public:
  bool init(rc_input_t *rc_input, UI_input_t *UIinput);
  bool validateStickCalibration(stick_calibration_t sticks);
  bool calibrateStickValues(rc_input_t *rc_input, UI_input_t *UIinput);
  bool checkCalibration(rc_input_t *rc_input, UI_input_t *UIinput);
  void readStickCalibrationPrefs();
  void saveSticks();
  void oledOutput(int line, char *tmp);
};
