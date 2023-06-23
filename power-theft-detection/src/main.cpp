#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <Adafruit_I2CDevice.h>

#define RELAY_PIN 16
#define DELAY_VALUE 1000
void setup() {
  pinMode(RELAY_PIN, OUTPUT); // Set pin 13 as an output
}

void loop() {
  digitalWrite(RELAY_PIN, HIGH); // Turn on the LED
  delay(DELAY_VALUE); // Wait for 1 second (1000 milliseconds)
  digitalWrite(RELAY_PIN, LOW); // Turn off the LED
  delay(DELAY_VALUE); // Wait for 1 second
}
