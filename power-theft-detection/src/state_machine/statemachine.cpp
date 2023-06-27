#include <Arduino.h>
#include "statemachine.h"
#include "tft_display/ui_display.h"
#include <EEPROM.h>

uint32_t customerChannelID = 0;

UIuserSetupState volatile projectCurrentSetupState = UI_HOME_SECURITY_SETUP_TEXT;

static int  readFromEEPROM( void ); 

void setupFlow( void )
{
    Serial.println("Entering Setup flow");
    switch (projectCurrentSetupState)
    {
    case UI_HOME_SECURITY_SETUP_TEXT:
        displaySetupText();
        delay( UI_SETUP_DELAY_TEXT );
        projectCurrentSetupState = UI_HOME_SECURITY_SETUP_TOUCH_CALIBRATION;
        Serial.println("Displaying UI_HOME_SECURITY_SETUP_TEXT");
        break;
    
    case UI_HOME_SECURITY_SETUP_CHANNEL_ID_TEXT:
        displaySetupTextInsertChannelID();
        delay( UI_SETUP_DELAY_TEXT );
        projectCurrentSetupState = UI_HOME_SECURITY_SETUP_CHANNEL_ID_KEYPAD;
        Serial.println("Displaying UI_HOME_SECURITY_SETUP_CHANNEL_ID_TEXT");
        break;
    
    case UI_HOME_SECURITY_SETUP_TOUCH_CALIBRATION:
        touch_calibrate();
        delay( UI_SETUP_DELAY_TEXT );
        projectCurrentSetupState = UI_HOME_SECURITY_SETUP_CHANNEL_ID_TEXT;
        Serial.println("Displaying UI_HOME_SECURITY_SETUP_TOUCH_CALIBRATION ");
        break;

    case UI_HOME_SECURITY_SETUP_CHANNEL_ID_KEYPAD:
        uiSetupDisplayDrawKeypad();
        projectCurrentSetupState = UI_HOME_SECURITY_SETUP_CHANNEL_ID_KEYPAD_LOOP;
        break;
    
    case UI_HOME_SECURITY_SETUP_CHANNEL_ID_KEYPAD_LOOP:
        uiSetupDisplayKeypadLoop();
        customerChannelID = readFromEEPROM();
        if ( ( customerChannelID >= 100000 && customerChannelID <= 999999 ) &&  ( 99 == EEPROM.read(6) ) )
        {
            projectCurrentSetupState = UI_HOME_SECURITY_SETUP_TEXT;
            EEPROM.end();
        }
        Serial.println("[DEBUG] customerChannelID: " + String(customerChannelID));
        break;
     
    default:
        break;
    }
}

void dashboardFlow( void )
{

}

int readFromEEPROM() 
{
    int value = 0;
    for (int i = 5; i >= 0; i--) {
    byte digit = EEPROM.read(i);
    value = value * 10 + digit;
    }
    return value;
}