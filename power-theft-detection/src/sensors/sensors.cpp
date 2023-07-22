#include "sensors.h"
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <ZMPT101B.h>
#include "thingspeak/thingspeak_security.h"
#include "debug_serial.h"

static Adafruit_ADS1115 ads;

const float FACTOR = 30; //20A/1V from teh CT
const float multiplier = 0.00008;

#define SENSITIVITY_AC_RMS 314.0f

static ZMPT101B voltageSensor(VOLTAGE_PIN, 60.0);
static byte getI2cAddress( void );

void  currentSensorSetup( void )
{
    byte scannedAddress = getI2cAddress();
    ads.begin( scannedAddress );
    ads.setGain( GAIN_SIXTEEN );      // +/- 1.024V 1bit = 0.5mV
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

byte getI2cAddress( void )
{
    byte error, retAddress;
    int deviceCount = 0;

    Wire.begin();

    for ( byte temp_address = 1; temp_address < 127; temp_address++)
    {
        Wire.beginTransmission(temp_address);
        error = Wire.endTransmission();
        DEBUG_PRINT_LN("Scanning 0x");
        DEBUG_PRINT_LN( temp_address, HEX );
        if (error == 0)
        {
        DEBUG_PRINT("I2C device found at address 0x");
        if (temp_address < 16) 
        {
        DEBUG_PRINT("0");
        }
        DEBUG_PRINT(temp_address, HEX);
        DEBUG_PRINT_LN();
        deviceCount++;
        retAddress = temp_address;
    }
    }

    if ( ( deviceCount == 0 )  )
    {
        DEBUG_PRINT_LN("No I2C devices found\n");
        retAddress = 0x0;
    }
    else if ( ( deviceCount > 1 ) )
    {
    DEBUG_PRINT_LN("Mutltiple Device Found");
    retAddress = 0x0;
    }

    return retAddress;
}