#include "Arduino.h"
#include "ads.h"
#include "ADS1X15.h"
#include "gpio.h"
#include "ui_buttons.h"
#include "config.h"
#include "uart.h"
#include "crsf_protocol.h"

ADS1115 ADS(0x48); /* Use this for the 16-bit version */    

void ADSreader::init()
{
if (infoEnabled) dbout.printf("starting ads...\n");
    

    u_int32_t i2c_addr = 0x48;
    Wire.begin();
    if (!ADS.begin())
    {
        if (infoEnabled) dbout.println("failed to initialize ADS.");
        while (1);
    }
    else
    {
        
        ADS.setGain(0);

        ADS.setGain(5);
        ADS.setMode(1);
        ADS.setDataRate(7); 
        ADS.readADC(0); 

        dbout.println("ADS initialized.");
    }

}

void ICACHE_RAM_ATTR ADSreader::readInputs(rc_input_t *rc_input)
{
    // read the analog in value:
    // converte float to int
    int16_t a0 = (int16_t)(ADS.readADC(0));
    int16_t a1 = (int16_t)(ADS.readADC(1));
    int16_t a2 = (int16_t)(ADS.readADC(2));
    int16_t a3 = (int16_t)(ADS.readADC(3));

    // map it to the range of the analog out:
    int16_t a0Scaled = a0 >> 4;
    int16_t a1Scaled = a1 >> 4;
    int16_t a2Scaled = a2 >> 4;
    int16_t a3Scaled = a3 >> 4;
  
    rc_input->aileron = a0;
    rc_input->elevator = a1;
    rc_input->throttle = a2;
    rc_input->rudder = a3;

    if (debugVoltageEnabled)
    {   
        float Voltage0 = (a0 * 0.1875) / 1000;
        float Voltage1 = (a1 * 0.1875) / 1000;
        float Voltage2 = (a2 * 0.1875) / 1000;
        float Voltage3 = (a3 * 0.1875) / 1000;
        dbout.printf("%f : %d :: %f : %d :: %f : %d :: %f : %d\n", Voltage0,a0 , Voltage1,a1,  Voltage2,a2, Voltage3,a3); 
    }
    


    //rc_input->aux1 = analogRead(ANALOG_IN_PIN_AUX1);
    //rc_input->aux2 = analogRead(ANALOG_IN_PIN_AUX2);
    // rc_input->aux3 = analogRead(ANALOG_IN_PIN_AUX3);
    // rc_input->aux4 = analogRead(ANALOG_IN_PIN_AUX4);
    delay(100);
}