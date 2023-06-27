#include <Arduino.h>
#include "tft_display/ui_display.h"
#include "state_machine/statemachine.h"
#include <WiFiClientSecure.h>
#include <Adafruit_I2CDevice.h>
#include <EEPROM.h>

#define EEPROM_SIZE 12

void setup(void) {
  // Initialize TFT dispaly
  initTFT();
  Serial.begin(9600);


  EEPROM.begin(EEPROM_SIZE);
  // saveToFlashChannelID( 0 );

  // TODO: Check flash if user credentials is setup
  if ( 99 != EEPROM.read(6) )
  while( UI_HOME_SECUIRTY_SETUP_DONE_SETUP != projectCurrentSetupState )
  {
    setupFlow();
  }

  // TODO: Parsed longtitude, latidue, wattage meter, client cellphone number
}

void loop() 
{

}



