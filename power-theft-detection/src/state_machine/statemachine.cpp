#include <Arduino.h>
#include "statemachine.h"
#include "tft_display/ui_display.h"
#include <EEPROM.h>

uint32_t g_customerChannelID = 0;

UIuserSetupState volatile g_projectCurrentSetupState = UI_HOME_SECURITY_SETUP_TEXT;

static int  readFromEEPROM( void ); 

void setupFlow( void )
{
    int setFlag = 0;
    switch (g_projectCurrentSetupState)
    {
    case UI_HOME_SECURITY_SETUP_TEXT:
        displaySetupText();
        delay( UI_SETUP_DELAY_TEXT );
        g_projectCurrentSetupState = UI_HOME_SECURITY_SETUP_TOUCH_CALIBRATION;
        Serial.println("Displaying UI_HOME_SECURITY_SETUP_TEXT");
        break;
    
    case UI_HOME_SECURITY_SETUP_CHANNEL_ID_TEXT:
        displaySetupTextInsertChannelID();
        delay( UI_SETUP_DELAY_TEXT );
        g_projectCurrentSetupState = UI_HOME_SECURITY_SETUP_CHANNEL_ID_KEYPAD;
        Serial.println("Displaying UI_HOME_SECURITY_SETUP_CHANNEL_ID_TEXT");
        break;
    
    case UI_HOME_SECURITY_SETUP_TOUCH_CALIBRATION:
        touch_calibrate();
        delay( UI_SETUP_DELAY_TEXT );
        g_projectCurrentSetupState = UI_HOME_SECURITY_SETUP_CHANNEL_ID_TEXT;
        Serial.println("Displaying UI_HOME_SECURITY_SETUP_TOUCH_CALIBRATION ");
        break;

    case UI_HOME_SECURITY_SETUP_CHANNEL_ID_KEYPAD:
        uiSetupDisplayDrawKeypad();
        g_projectCurrentSetupState = UI_HOME_SECURITY_SETUP_CHANNEL_ID_KEYPAD_LOOP;
        break;
    
    case UI_HOME_SECURITY_SETUP_CHANNEL_ID_KEYPAD_LOOP:
        uiSetupDisplayKeypadLoop();
        g_customerChannelID = readFromEEPROM();
        setFlag = EEPROM.read(MAXIMUM_CHANNEL_ID_NUMBER);
        if ( ( g_customerChannelID >= 1000000 && g_customerChannelID <= 9999999 ) &&  ( DONE_SET_VALUE == setFlag ) )
        {
            Serial.println(g_customerChannelID);
            Serial.println(setFlag);
            g_projectCurrentSetupState = UI_HOME_SECUIRTY_SETUP_DONE_SETUP;
            EEPROM.end();
        }
        break;
    
    default:
        break;
    }
}

void dashboardFlow( void )
{

}

static int readFromEEPROM() 
{
    int value = 0;
    for (int i = MAXIMUM_CHANNEL_ID_NUMBER -1; i >= 0; i--) {
    byte digit = EEPROM.read(i);
    value = value * 10 + digit;
    }
    return value;
}