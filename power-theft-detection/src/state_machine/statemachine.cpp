#include <Arduino.h>
#include "statemachine.h"
#include "tft_display/ui_display.h"

UIuserSetupState volatile projectCurrentSetupState = UI_HOME_SECURITY_SETUP_TEXT;

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
        // TODO: Check if channel id set before updating status
        // projectCurrentSetupState = UI_HOME_SECUIRTY_SETUP_DONE_SETUP;
        break;
    
    default:
        break;
    }

}

void dashboardFlow( void )
{

}