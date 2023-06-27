/*  
 Test the tft.print() viz embedded tft.write() function

 This sketch used font 2, 4, 7

 Make sure all the display driver and pin connections are correct by
 editing the User_Setup.h file in the TFT_eSPI library folder.

 #########################################################################
 ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
 #########################################################################
 */

#include <Arduino.h>
#include "tft_display/ui_display.h"
#include "state_machine/statemachine.h"
#include <WiFiClientSecure.h>
#include <Adafruit_I2CDevice.h>


void setup(void) {
  // Initialize TFT dispaly
  initTFT();

  // TODO: Check flash if user credentials is setup
  Serial.begin(9600);
  
  while( UI_HOME_SECUIRTY_SETUP_DONE_SETUP != projectCurrentSetupState )
  {
    setupFlow();
  }
}

void loop() 
{

}



