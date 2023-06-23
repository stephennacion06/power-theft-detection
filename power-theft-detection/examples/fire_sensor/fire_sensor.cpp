#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <Adafruit_I2CDevice.h>

const int pinNumber = 25;  // Replace with the desired pin number

void setup() {
  Serial.begin(9600);  // Initialize serial communication
  pinMode(pinNumber, INPUT);  // Set the specified pin as input
}

void loop() {
int adc = analogRead(pinNumber);
Serial.println(adc);

  delay(1000);  // Delay for 1 second
}