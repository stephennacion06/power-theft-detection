#include "sensors.h"
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <ZMPT101B.h>
#include "thingspeak/thingspeak_security.h"

static Adafruit_ADS1115 ads;

const float FACTOR = 30; //20A/1V from teh CT
const float multiplier = 0.00008;

static ZMPT101B voltageSensor(VOLTAGE_PIN, 60.0);

#define SENSITIVITY_AC_RMS 314.0f

void  currentSensorSetup( void )
{
    ads.setGain(GAIN_SIXTEEN);      // +/- 1.024V 1bit = 0.5mV
    ads.begin();
}

float getCurrent( void )
{
    float voltage;
    float current;
    float sum = 0;
    long time_check = millis();
    int counter = 0;

    while (millis() - time_check < 1000)
    {
        voltage = ads.readADC_Differential_0_1() * multiplier;
        current = voltage * FACTOR;
        sum += sq(current);
        counter = counter + 1;
    }

    current = sqrt(sum / counter);
    return (current);

}

void voltageSensorSetup( void )
{
      voltageSensor.setSensitivity(SENSITIVITY_AC_RMS);
}

float getVoltage( void )
{
    static uint32_t updateTime = 0; 
    float voltage;

    if (millis() - updateTime >= 1) 
    {
        voltage = voltageSensor.getRmsVoltage();;
    }
    
    return voltage;
}

bool doorSensorValue( void )
{
    return digitalRead( DOOR_SENSOR_PIN );
}

bool motionSensorValue( void )
{
    return digitalRead( MOTION_SENSOR_PIN );
}

bool fireSensorValue( void )
{
    return digitalRead( FIRE_SENSOR_PIN );
}

void powerTheftDetectionSetup( void )
{
    pinMode(RELAY_PIN, OUTPUT);
}

bool powerTheftDetection( float wattage )
{
    bool powerTheftDetected = false;
    
    if( wattage >= g_homeOwnerWattageMax)
    {
        digitalWrite(RELAY_PIN, LOW);
        powerTheftDetected = true;
    }
    else
    {
        digitalWrite(RELAY_PIN, HIGH);
    }
    return powerTheftDetected;
}