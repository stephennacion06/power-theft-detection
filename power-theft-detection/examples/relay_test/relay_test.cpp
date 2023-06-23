#include <Arduino.h>

void setup() {
  pinMode(35, OUTPUT); // Set pin 13 as an output
}

void loop() {
  digitalWrite(35, HIGH); // Turn on the LED
  delay(1000); // Wait for 1 second (1000 milliseconds)
  digitalWrite(35, LOW); // Turn off the LED
  delay(1000); // Wait for 1 second
}
