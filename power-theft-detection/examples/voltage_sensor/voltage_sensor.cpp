#include <Arduino.h>
#include <ZMPT101B.h>
#include <Adafruit_I2CDevice.h>

// // ZMPT101B sensor output connected to analog pin A0
// // and the voltage source frequency is 50 Hz.

ZMPT101B voltageSensor(A4, 60.0);

#define SENSITIVITY_AC_RMS 314.0f

void setup() {
  Serial.begin(9600);
  // Change the sensitivity value based on value you got from the calibrate
  // example.
  voltageSensor.setSensitivity(SENSITIVITY_AC_RMS);
}

void loop() {
  float voltageNow = voltageSensor.getRmsVoltage();
  Serial.print(">AC Voltage (RMS): ");
  Serial.println(voltageNow);
  delay(1);
}
