#include <Arduino.h>

const int pinNumber = 0;  // Replace with the desired pin number

void setup() {
  Serial.begin(9600);  // Initialize serial communication
  pinMode(pinNumber, INPUT);  // Set the specified pin as input
}

void loop() {
  int pinStatus = digitalRead(pinNumber);  // Read the status of the pin
  
  if (pinStatus == HIGH) {
    Serial.println("Pin is HIGH");
  } else {
    Serial.println("Pin is LOW");
  }
  
  delay(1000);  // Delay for 1 second
}