#ifndef SENSORS_H
#define SENSORS_H

#define VOLTAGE_PIN 32
#define DOOR_SENSOR_PIN 34 
#define MOTION_SENSOR_PIN 35
#define FIRE_SENSOR_PIN 26
#define RELAY_PIN 27

// Current Sensor
float getCurrent( void );
void  currentSensorSetup( void );

// Voltage Sensor
void voltageSensorSetup( void );
float getVoltage( void );

// Door Sensor
bool doorSensorValue( void );

// Motion Sensor
bool motionSensorValue( void );

// Fire Sensor
bool fireSensorValue( void );

// Power Theft Detection
void powerTheftDetectionSetup( void );
bool powerTheftDetection( float wattage, bool powerOn );

#endif // SENSORS_H