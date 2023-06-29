#include <Arduino.h>
#include "tft_display/ui_display.h"
#include "state_machine/statemachine.h"
#include "thingspeak/thingspeak_security.h"
#include "twilio_sms/twilio_sms.h"
#include <WiFiClientSecure.h>
#include <Adafruit_I2CDevice.h>
#include <EEPROM.h>
#include <WiFiManager.h>

#define EEPROM_SIZE 12
#define DONE_SET_VALUE 99

static bool wifiRes;
//WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
static WiFiManager wm;

// NOTE: Run "pio run -t erase" to erase flash memory 
static int readFlashChannelID( void );

void setup(void) 
{
  // Initialize TFT dispaly
  initTFT();

  // Initiialize Serial Communication UART 0
  Serial.begin(9600);

  EEPROM.begin(EEPROM_SIZE);
  
  // NOTE: reset settings - wipe stored credentials for testing
  //wm.resetSettings();

  Serial.println("Starting to Connect");
  displaySetupConnectToWifi();
  wifiRes = wm.autoConnect("HomeSecurity Setup","password"); // password protected ap

  if(!wifiRes) 
  {
      // TODO: Display Text Failed to connect restarting
      displaySetupConnectToWifiFailed();
      delay(UI_SETUP_DELAY_TEXT);
      ESP.restart();
  } 
  else 
  {
      //if you get here you have connected to the WiFi    
      // TODO: Display Text Succesfully connected
      displaySetupConnectToWifiPassed();
      delay(UI_SETUP_DELAY_TEXT);
      
      int setFlag = EEPROM.read( MAXIMUM_CHANNEL_ID_NUMBER );
      g_customerChannelID = readFlashChannelID();

      if ( ( DONE_SET_VALUE !=  setFlag ) )
      {
        while( UI_HOME_SECUIRTY_SETUP_DONE_SETUP != g_projectCurrentSetupState )
        {
          setupFlow();
        }
      }

    // Initialize Twillio 
    initTwilio();
  }

  bool successUpdate = false;
  // TODO: update ui to extracting information from Thingspeak
  displayThingspeakExtraction();
  delay(UI_SETUP_DELAY_TEXT);
  while ( false == successUpdate )
  {
    successUpdate = updateHomeOwnerInformation( g_customerChannelID );
  }
  displayThingspeakExtractionInformation();

  // TODO: SMS test run all

  sendInitMessage();

}

void loop() 
{

}

static int readFlashChannelID( void ) 
{
    int value = 0;
    for (int i = MAXIMUM_CHANNEL_ID_NUMBER -1; i >= 0; i--) {
    byte digit = EEPROM.read(i);
    value = value * 10 + digit;
    }
    return value;
}



