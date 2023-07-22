#ifndef SENSORS_H
#define SENSORS_H

#define VOLTAGE_PIN A4
#define DOOR_SENSOR_PIN 26
#define MOTION_SENSOR_PIN 15
#define FIRE_SENSOR_PIN 25
#define RELAY_PIN 16

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
bool powerTheftDetection( float wattage );

#endif // SENSORS_H