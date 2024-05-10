#include "sticks.h"
#include "uart.h"
#include "crsf.h"
#include "oled.h"
#include "ui_1wire_keypad.h"
#include "ui_buttons.h"

bool Prefs::init(rc_input_t *rc_input, UI_input_t *UIinput)
{
    preferences.begin("elrs", false);
    saved_stick_calibration = false;//preferences.getBool("saved_calib", false);
    dbout.printf("saved_stick_calibration: %d\n", saved_stick_calibration);
    if (!saved_stick_calibration) 
    {
        return checkCalibration(rc_input, UIinput);
        //return false;
    }
    else return true;
}

bool Prefs::checkCalibration(rc_input_t *rc_input, UI_input_t *UIinput)
{
    // check if sticks are calibrated
    // if not calibrated, calibrate
    do
    {
        dbout.printf("Sticks not calibrated\n");
        saved_stick_calibration = false;
        calibrateStickValues(rc_input, UIinput);
    } while (validateStickCalibration(sticks) != true);

    dbout.printf("Sticks calibrated\n");

    return true;
}

void Prefs::oledOutput(int line, char *tmp)
{
    oled.Clear();
    oled.PrintCenter((char *)":: sticks calibration ::" ,12);
    oled.PrintCenter(tmp, line);
}

boolean ICACHE_RAM_ATTR Prefs::calibrateStickValues(rc_input_t *rc_input, UI_input_t *UIinput)
{

    readStickCalibrationPrefs();
    oledOutput(42, (char *)"Center sticks");

    dbout.printf("Sticks calibration\n");
    dbout.printf("Center sticks\n");

    const unsigned long calibrationDuration = 6000;
    const unsigned long calibrationEndTime = millis() + calibrationDuration;
    const unsigned long readEndTime = millis() + 2000;
    
    while (millis() < readEndTime)
    {
        ads_reader.readInputs(rc_input);

        sticks.aileronCenter = rc_input->aileron;
        sticks.elevatorCenter = rc_input->elevator;
        sticks.throttleCenter = rc_input->throttle;
        sticks.rudderCenter = rc_input->rudder;
        if (debugEnabled)
            dbout.printf("Center values: Aileron: %d, Elevator: %d, Throttle: %d, Rudder: %d\n", sticks.aileronCenter, sticks.elevatorCenter, sticks.throttleCenter, sticks.rudderCenter);
    }

    // reset the max and min values
    sticks.aileronMin = sticks.aileronCenter;
    sticks.aileronMax = sticks.aileronCenter;
    sticks.elevatorMin = sticks.elevatorCenter;
    sticks.elevatorMax = sticks.elevatorCenter;
    sticks.throttleMax = sticks.throttleCenter;
    sticks.throttleMin = sticks.throttleCenter;
    sticks.rudderMin = sticks.rudderCenter;
    sticks.rudderMax = sticks.rudderCenter;

    if (debugEnabled)
    {
        dbout.printf("Move sticks to their maximum and minimum positions\n");
    }
    oled.Clear();
    oledOutput(22, (char *)"Move sticks");

    oled.drawProgressBarFrames(BAR_X, BAR_Y, BAR_WIDTH, BAR_HEIGHT);
    oled.drawProgressBarFrames(BAR_X + BAR_WIDTH + 10, BAR_Y, BAR_WIDTH, BAR_HEIGHT);
    oled.drawProgressBarFrames(BAR_X + 3 * (BAR_WIDTH + 10), BAR_Y, BAR_WIDTH, BAR_HEIGHT);
    oled.drawProgressBarFrames(BAR_X + 2 * (BAR_WIDTH + 10), BAR_Y, BAR_WIDTH, BAR_HEIGHT);

    while (millis() < calibrationEndTime)
    {
        ads_reader.readInputs(rc_input);

        // read the values from the sticks
        int aileron = rc_input->aileron;
        int throttle = rc_input->throttle;
        int rudder = rc_input->rudder;
        int elevator = rc_input->elevator;

        // Atualizar os valores máximos e mínimos dos sticks
        sticks.aileronMax = max(sticks.aileronMax, aileron);
        sticks.elevatorMax = max(sticks.elevatorMax, elevator);
        sticks.throttleMax = max(sticks.throttleMax, throttle);
        sticks.rudderMax = max(sticks.rudderMax, rudder);

        sticks.aileronMin = min(sticks.aileronMin, aileron);
        sticks.elevatorMin = min(sticks.elevatorMin, elevator);
        sticks.throttleMin = min(sticks.throttleMin, throttle);
        sticks.rudderMin = min(sticks.rudderMin, rudder);

        //check if the min and max values are far enough apart, else set min and max to center + or - 0.5 * STICK_HIGH
        if (sticks.aileronConnected == false) {
            if (sticks.aileronMax - sticks.aileronMin < 0.2 * STICK_HIGH)
            {
                sticks.aileronMax = sticks.aileronCenter + 0.1 * STICK_HIGH;
                sticks.aileronMin = sticks.aileronCenter - 0.1 * STICK_HIGH;
                sticks.elevatorConnected = false;
            } else {
                if (sticks.aileronMax - sticks.aileronMin > 0.5 * STICK_HIGH) {
                    sticks.aileronConnected = true;
                }
            }
        }
        if (sticks.elevatorConnected == false) {
            if (sticks.elevatorMax - sticks.elevatorMin < 0.2 * STICK_HIGH)
            {
                sticks.elevatorMax = sticks.elevatorCenter + 0.1 * STICK_HIGH;
                sticks.elevatorMin = sticks.elevatorCenter - 0.1 * STICK_HIGH;
                sticks.elevatorConnected = false;
            } else {
                if (sticks.elevatorMax - sticks.elevatorMin > 0.5 * STICK_HIGH) {
                    sticks.elevatorConnected = true;
                }
            }
        }
        if (sticks.throttleConnected == false) {
            if (sticks.throttleMax - sticks.throttleMin < 0.2 * STICK_HIGH)
            {
                sticks.throttleMax = sticks.throttleCenter + 0.1 * STICK_HIGH;
                sticks.throttleMin = sticks.throttleCenter - 0.1 * STICK_HIGH;
                sticks.throttleConnected = false;
            } else {
                if (sticks.throttleMax - sticks.throttleMin > 0.5 * STICK_HIGH) {
                    sticks.throttleConnected = true;
                }
            }
        }
        if (sticks.rudderConnected == false) {
            if (sticks.rudderMax - sticks.rudderMin < 0.2 * STICK_HIGH)
            {
                sticks.rudderMax = sticks.rudderCenter + 0.1 * STICK_HIGH;
                sticks.rudderMin = sticks.rudderCenter - 0.1 * STICK_HIGH;
                sticks.rudderConnected = false;
            } else {
                if (sticks.rudderMax - sticks.rudderMin > 0.5 * STICK_HIGH) {
                    sticks.rudderConnected = true;
                }
            }
        }
       
        oled.drawProgressBar(BAR_X, BAR_Y, BAR_WIDTH, BAR_HEIGHT, aileron, sticks.aileronMax, sticks.aileronMin, sticks.aileronCenter);
        oled.drawProgressBar(BAR_X + BAR_WIDTH + 10, BAR_Y, BAR_WIDTH, BAR_HEIGHT, elevator, sticks.elevatorMax, sticks.elevatorMin, sticks.elevatorCenter);
        oled.drawProgressBar(BAR_X + 2 * (BAR_WIDTH + 10), BAR_Y, BAR_WIDTH, BAR_HEIGHT, throttle, sticks.throttleMax, sticks.throttleMin, sticks.throttleCenter);
        oled.drawProgressBar(BAR_X + 3 * (BAR_WIDTH + 10), BAR_Y, BAR_WIDTH, BAR_HEIGHT, rudder, sticks.rudderMax, sticks.rudderMin, sticks.rudderCenter);

        if (!debugEnabled)
        {
            char tmp[100];
            sprintf(tmp, "%d:M:%d:m:%d :: %d:M:%d:m:%d :: %d:M:%d:m:%d :: %d:M:%d:m:%d ", aileron, sticks.aileronMax, sticks.aileronMin, elevator, sticks.elevatorMax, sticks.elevatorMin, throttle, sticks.throttleMax, sticks.throttleMin, rudder, sticks.rudderMax, sticks.rudderMin);
            dbout.println(tmp);
        }
    }
    dbout.printf("Calibration finished\n");

    //need to count the number of sticks connected
    stickCalibrationCounter = 0;
    if (sticks.aileronConnected) stickCalibrationCounter++;
    if (sticks.elevatorConnected) stickCalibrationCounter++;
    if (sticks.throttleConnected) stickCalibrationCounter++;
    if (sticks.rudderConnected) stickCalibrationCounter++;

    dbout.printf("Connected sticks: %d\n", stickCalibrationCounter);

    oledOutput(22, (char *)"Calibration finished");

    int TEMPSTRINGLENGTH = 30;

    char tempstring[TEMPSTRINGLENGTH];
    sprintf(tempstring, "ch1: %d :: %d :: %d", sticks.aileronMin, sticks.aileronCenter, sticks.aileronMax);
    oled.PrintCenter(tempstring,32);
    sprintf(tempstring, "ch2: %d :: %d :: %d", sticks.elevatorMin, sticks.elevatorCenter, sticks.elevatorMax);
    oled.PrintCenter(tempstring,42);
    sprintf(tempstring, "ch3: %d :: %d :: %d", sticks.throttleMin, sticks.throttleCenter, sticks.throttleMax);
    oled.PrintCenter(tempstring,52);
    sprintf(tempstring, "ch4: %d :: %d :: %d", sticks.rudderMin, sticks.rudderCenter, sticks.rudderMax);
    oled.PrintCenter(tempstring,62);

    // Show the results by channel
    dbout.printf("Aileron: %d :: %d :: %d\n", sticks.aileronMin, sticks.aileronCenter, sticks.aileronMax);
    dbout.printf("Elevator: %d :: %d :: %d\n", sticks.elevatorMin, sticks.elevatorCenter, sticks.elevatorMax);
    dbout.printf("Throttle: %d :: %d :: %d\n", sticks.throttleMin, sticks.throttleCenter, sticks.throttleMax);
    dbout.printf("Rudder: %d :: %d :: %d\n", sticks.rudderMin, sticks.rudderCenter, sticks.rudderMax);

    delay(5000);

    // check calibration
    if (validateStickCalibration(sticks) != true)
    {
        dbout.printf("Calibration failed\n");
        oledOutput(22, (char *)"Calibration failed");
        delay(3000);
        oled.Clear();

        return false;
    }
    else
    {
        oledOutput(22, (char *)"Calibration success");
        oled.PrintCenter((char *)"Saving calibration",32);

        if (debugEnabled)
            dbout.printf("Calibration success\n");

        saveSticks();

        delay(3000);
        oled.Clear();

        return true;
    }
    
}

bool ICACHE_RAM_ATTR Prefs::validateStickCalibration(stick_calibration_t sticks)
{

    // dif between max and min
    int diffAileron = sticks.aileronMax - sticks.aileronMin;
    int diffElevator = sticks.elevatorMax - sticks.elevatorMin;
    int diffThrottle = sticks.throttleMax - sticks.throttleMin;
    int diffRudder = sticks.rudderMax - sticks.rudderMin;
    
    dbout.printf("diff: %d :: %d :: %d :: %d\n",sticks.aileronMax, sticks.aileronMin, sticks.aileronCenter, diffAileron);
    dbout.printf("diff: %d :: %d :: %d :: %d\n",sticks.elevatorMax, sticks.elevatorMin, sticks.elevatorCenter, diffElevator);
    dbout.printf("diff: %d :: %d :: %d :: %d\n",sticks.throttleMax, sticks.throttleMin, sticks.throttleCenter, diffThrottle);
    dbout.printf("diff: %d :: %d :: %d :: %d\n",sticks.rudderMax, sticks.rudderMin, sticks.rudderCenter, diffRudder);

    // define min diff to be valid 0-1024
    double minDiff = 500;

    // check if sticks are calibrated
    bool isAileronCalibrated = diffAileron > minDiff;
    bool isElevatorCalibrated = diffElevator > minDiff;
    bool isThrottleCalibrated = diffThrottle > minDiff;
    bool isRudderCalibrated = diffRudder > minDiff;

    // calculate the division of min / max
    double aileronDivision = (double)sticks.aileronMax / (double)sticks.aileronMin;
    double elevatorDivision = (double)sticks.elevatorMax / (double)sticks.elevatorMin;
    double throttleDivision = (double)sticks.throttleMax / (double)sticks.throttleMin;
    double rudderDivision = (double)sticks.rudderMax / (double)sticks.rudderMin;

    dbout.printf("div: %f :: %f :: %f :: %f\n", aileronDivision, elevatorDivision, throttleDivision, rudderDivision);

    // check if sticks are reversed
    bool isAileronReversed = aileronDivision > 1;
    bool isElevatorReversed = elevatorDivision > 1;
    bool isThrottleReversed = throttleDivision > 1;
    bool isRudderReversed = rudderDivision > 1;

    // check if sticks have a valid range
    bool isAileronRangeValid = (diffAileron > 0.5 * STICK_HIGH) ? true : false;
    bool isElevatorRangeValid = (diffElevator > 0.5 * STICK_HIGH) ? true : false;
    bool isThrottleRangeValid = (diffThrottle > 0.5 * STICK_HIGH) ? true : false;
    bool isRudderRangeValid = (diffRudder > 0.5 * STICK_HIGH) ? true : false;

    // caculate tolerance
    int toleranceAileron = max(diffAileron * 0.25, minDiff);
    int toleranceElevator = max(diffElevator * 0.25, minDiff);
    int toleranceThrottle = max(diffThrottle * 0.25, minDiff);
    int toleranceRudder = max(diffRudder * 0.25, minDiff);
    // check if max and min diff is valid
    bool isAileronValid = (sticks.aileronMax - sticks.aileronMin > toleranceAileron) ? true : false;
    bool isElevatorValid = (sticks.elevatorMax - sticks.elevatorMin > toleranceElevator) ? true : false;
    bool isThrottleValid = (sticks.throttleMax - sticks.throttleMin > toleranceThrottle) ? true : false;
    bool isRudderValid = (sticks.rudderMax - sticks.rudderMin > toleranceRudder) ? true : false;
    // check if center is valid
    bool isAileronCenterValid = (sticks.aileronCenter > sticks.aileronMin) && (sticks.aileronCenter < sticks.aileronMax);
    bool isElevatorCenterValid = (sticks.elevatorCenter > sticks.elevatorMin) && (sticks.elevatorCenter < sticks.elevatorMax);
    bool isThrottleCenterValid = (sticks.throttleCenter > sticks.throttleMin) && (sticks.throttleCenter < sticks.throttleMax);
    bool isRudderCenterValid = (sticks.rudderCenter > sticks.rudderMin) && (sticks.rudderCenter < sticks.rudderMax);

    // Show the results
    if (debugEnabled)
    {
        dbout.printf("Tolerance: Aileron: %d, Elevator: %d, Throttle: %d, Rudder: %d\n", toleranceAileron, toleranceElevator, toleranceThrottle, toleranceRudder);
        dbout.printf("Rudder  : cal %s :: val %s :: rev %s :: rng %s :: center :: %s \n", isRudderCalibrated ? "true" : "false", isRudderValid ? "true" : "false", isRudderReversed ? "true" : "false", isRudderRangeValid ? "true" : "false", isRudderCenterValid ? "true" : "false");
        dbout.printf("Aileron : cal %s :: val %s :: rev %s :: rng %s :: center :: %s \n", isAileronCalibrated ? "true" : "false", isAileronValid ? "true" : "false", isAileronReversed ? "true" : "false", isAileronRangeValid ? "true" : "false", isAileronCenterValid ? "true" : "false");
        dbout.printf("Elevator: cal %s :: val %s :: rev %s :: rng %s :: center :: %s \n", isElevatorCalibrated ? "true" : "false", isElevatorValid ? "true" : "false", isElevatorReversed ? "true" : "false", isElevatorRangeValid ? "true" : "false", isElevatorCenterValid ? "true" : "false");
        dbout.printf("Throttle: cal %s :: val %s :: rev %s :: rng %s :: center :: %s \n", isThrottleCalibrated ? "true" : "false", isThrottleValid ? "true" : "false", isThrottleReversed ? "true" : "false", isThrottleRangeValid ? "true" : "false", isThrottleCenterValid ? "true" : "false");


    }
    // return true if all sticks are valid and calibrated
    // return isAileronValid && isElevatorValid && isThrottleValid && isRudderValid && isAileronCalibrated && isElevatorCalibrated && isThrottleCalibrated && isRudderCalibrated;
    return isElevatorCalibrated && isElevatorValid && isElevatorCenterValid; // only elevator for now
    //isRudderValid && isRudderCalibrated && isAileronValid && isAileronCalibrated; // only ruddder for now
}

void Prefs::saveSticks()
{
    if (debugEnabled)
        dbout.printf("Saving calibration\n");

    preferences.begin("elrs", false);
    if (sticks.aileronConnected) {
        preferences.putBool("aileron_conn", true);
        preferences.putInt("aileron_max", sticks.aileronMax);
        preferences.putInt("aileron_min", sticks.aileronMin);
        preferences.putInt("aileron_center", sticks.aileronCenter);
    }
    if (sticks.elevatorConnected) {
        preferences.putBool("elevator_conn", true);
        preferences.putInt("elevator_max", sticks.elevatorMax);
        preferences.putInt("elevator_min", sticks.elevatorMin);
        preferences.putInt("elevator_center", sticks.elevatorCenter);
    }
    if (sticks.throttleConnected) {
        preferences.putBool("throttle_conn", true);
        preferences.putInt("throttle_max", sticks.throttleMax);
        preferences.putInt("throttle_min", sticks.throttleMin);
        preferences.putInt("throttle_center", sticks.throttleCenter);
    }
    if (sticks.rudderConnected) {
        preferences.putBool("rudder_conn", true);
        preferences.putInt("rudder_max", sticks.rudderMax);
        preferences.putInt("rudder_min", sticks.rudderMin);
        preferences.putInt("rudder_center", sticks.rudderCenter);
    }

    preferences.putInt("stick_num", stickCalibrationCounter);
    preferences.putBool("saved_calib", true);


    if (debugEnabled)
        dbout.printf("Current calibration saved: %d\n", saved_stick_calibration);

    preferences.end();
}

// Read saved sticks center, max and min values from preferences
void Prefs::readStickCalibrationPrefs()
{
    preferences.begin("elrs", false);

    // Ler os valores das preferências
    sticks.aileronMax = preferences.getInt("aileron_max", 0);
    sticks.elevatorMax = preferences.getInt("elevator_max", 0);
    sticks.throttleMax = preferences.getInt("throttle_max", 0);
    sticks.rudderMax = preferences.getInt("rudder_max", 0);

    sticks.aileronMin = preferences.getInt("aileron_min", 0);
    sticks.elevatorMin = preferences.getInt("elevator_min", 0);
    sticks.throttleMin = preferences.getInt("throttle_min", 0);
    sticks.rudderMin = preferences.getInt("rudder_min", 0);

    sticks.aileronCenter = preferences.getInt("aileron_center", 0);
    sticks.elevatorCenter = preferences.getInt("elevator_center", 0);
    sticks.throttleCenter = preferences.getInt("throttle_center", 0);
    sticks.rudderCenter = preferences.getInt("rudder_center", 0);

    // sticks.aileronMin = (sticks.aileronMin == 0) ? STICK_CENTER : sticks.aileronMin;
    // sticks.elevatorMin = (sticks.elevatorMin == 0) ? STICK_CENTER : sticks.elevatorMin;
    // sticks.throttleMin = (sticks.throttleMin == 0) ? STICK_CENTER : sticks.throttleMin;
    // sticks.rudderMin = (sticks.rudderMin == 0) ? STICK_CENTER : sticks.rudderMin;

    // sticks.aileronCenter = (sticks.aileronCenter == 0) ? STICK_CENTER : sticks.aileronCenter;
    // sticks.elevatorCenter = (sticks.elevatorCenter == 0) ? STICK_CENTER : sticks.elevatorCenter;
    // sticks.throttleCenter = (sticks.throttleCenter == 0) ? STICK_CENTER : sticks.throttleCenter;
    // sticks.rudderCenter = (sticks.rudderCenter == 0) ? STICK_CENTER : sticks.rudderCenter;

    // Fechar as preferências
    preferences.end();

    // Exibir os valores lidos
    if (!debugEnabled)
    {
        dbout.printf("Saved Max values: Aileron: %d, Elevator: %d, Throttle: %d, Rudder: %d\n", sticks.aileronMax, sticks.elevatorMax, sticks.throttleMax, sticks.rudderMax);
        dbout.printf("Saved Min values: Aileron: %d, Elevator: %d, Throttle: %d, Rudder: %d\n", sticks.aileronMin, sticks.elevatorMin, sticks.throttleMin, sticks.rudderMin);
        dbout.printf("Saved Center values: Aileron: %d, Elevator: %d, Throttle: %d, Rudder: %d\n", sticks.aileronCenter, sticks.elevatorCenter, sticks.throttleCenter, sticks.rudderCenter);
    }
}