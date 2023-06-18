// EmonLibrary examples openenergymonitor.org, Licence GNU GPL V3
#include <Arduino.h>
#include "EmonLib.h"                   // Include Emon Library
#include <EEPROM.h>

EnergyMonitor emon1;                   // Create an instance

#define ADC_INPUT 34


// Force EmonLib to use 10bit ADC resolution
#define ADC_BITS    10
#define ADC_COUNTS  (1<<ADC_BITS)
#define IRMS_CALIBRATION 0.70

double receivedValue;  // Variable to store the received value

// EEPROM VARIABLES
uint8_t calibratedFlag =  0;
double calibrationValue = 0;


void setup()
{  
  Serial.begin(9600);
  EEPROM.begin( sizeof( bool ) + sizeof( double ) );
  analogReadResolution(10);

  //TODO: READ EEPROM VALUE; IF EEPROM VALUE IS NOT SET USED default value else use saved eeprom value
  uint8_t calibrationFlag = EEPROM.read(0);

  if ( calibrationFlag == 0 )
  {
    emon1.current(ADC_INPUT, IRMS_CALIBRATION);             // Current: input pin, calibration.
  }
  else if ( calibrationFlag == 1 )
  {
    calibrationValue = EEPROM.read(1);
    Serial.println("The value of Calibration is: " + String(calibrationValue));
    emon1.current(ADC_INPUT, calibrationValue);
  }
  else
  {
    emon1.current(ADC_INPUT, IRMS_CALIBRATION); 
  }


}

void loop()
{
if (Serial.available() > 0) 
{  // Check if there is data available to read
  String input = Serial.readStringUntil('\n');  // Read the incoming data as a string
  receivedValue = input.toDouble();  // Convert the string to a double and store it in the variable

  Serial.print("Received value: ");
  Serial.println(receivedValue);

  // SAVE TO EEPROM value
  EEPROM.put(1, receivedValue);
  EEPROM.commit();

  // SETTING FLAG TO TRUE
  EEPROM.put(0, 1);
  EEPROM.commit();
  
  EEPROM.end();

  // RESET MCU
  ESP.restart();
}

  double Irms = emon1.calcIrms(1480);  // Calculate Irms only
  
  Serial.print(Irms*230.0);	       // Apparent power
  Serial.print(" ");
  Serial.println(Irms,4);		       // Irms
}
