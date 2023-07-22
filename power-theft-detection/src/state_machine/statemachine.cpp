#include <Arduino.h>
#include "statemachine.h"
#include "tft_display/ui_display.h"
#include <EEPROM.h>
#include "debug_serial.h"

uint32_t g_customerChannelID = 0;

UIuserSetupState volatile g_projectCurrentSetupState = UI_HOME_SECURITY_SETUP_TEXT;

UIdashboardState volatile g_projectDashboardState = UI_DASHBOARD_SETUP;

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
        DEBUG_PRINT_LN("Displaying UI_HOME_SECURITY_SETUP_TEXT");
        break;
    
    case UI_HOME_SECURITY_SETUP_CHANNEL_ID_TEXT:
        displaySetupTextInsertChannelID();
        delay( UI_SETUP_DELAY_TEXT );
        g_projectCurrentSetupState = UI_HOME_SECURITY_SETUP_CHANNEL_ID_KEYPAD;
        DEBUG_PRINT_LN("Displaying UI_HOME_SECURITY_SETUP_CHANNEL_ID_TEXT");
        break;
    
    case UI_HOME_SECURITY_SETUP_TOUCH_CALIBRATION:
        touch_calibrate();
        delay( UI_SETUP_DELAY_TEXT );
        g_projectCurrentSetupState = UI_HOME_SECURITY_SETUP_CHANNEL_ID_TEXT;
        DEBUG_PRINT_LN("Displaying UI_HOME_SECURITY_SETUP_TOUCH_CALIBRATION ");
        break;

    case UI_HOME_SECURITY_SETUP_CHANNEL_ID_KEYPAD:
        uiSetupDisplayDrawKeypad();
        g_projectCurrentSetupState = UI_HOME_SECURITY_SETUP_CHANNEL_ID_KEYPAD_LOOP;
        break;
    
    case UI_HOME_SECURITY_SETUP_CHANNEL_ID_KEYPAD_LOOP:
        uiSetupDisplayKeypadLoop();
        g_customerChannelID = readFromEEPROM();
        setFlag = EEPROM.read(MAXIMUM_CHANNEL_ID_NUMBER);
        if ( ( g_customerChannelID >= validNumberMin && g_customerChannelID <= validNumberMax ) &&  ( DONE_SET_VALUE == setFlag ) )
        {
            DEBUG_PRINT_LN(g_customerChannelID);
            DEBUG_PRINT_LN(setFlag);
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
    int dashboardStatus = (UIDashboardError) BLANK_ERROR_MESSAGE;
    switch (g_projectDashboardState)
    {
    case UI_DASHBOARD_SETUP:
        dashboardSetup();
        g_projectDashboardState = UI_DASHBOARD_LOOP;
        break;
    case UI_DASHBOARD_LOOP:
        dashboardStatus = (UIDashboardError)  dashboardLoop();
        if ( POWER_THEFT_DETECTED_UI == dashboardStatus)
        {
            g_projectDashboardState = UI_DASHBOARD_POWER_THEFT_DETECTED_SETUP;
        }
        break;
    case UI_DASHBOARD_POWER_THEFT_DETECTED_SETUP:
        dashBoardPowerTheftDetectedSetup();
        g_projectDashboardState = UI_DASHBOARD_POWER_THEFT_DETECTED_LOOP;
        break;
    case UI_DASHBOARD_POWER_THEFT_DETECTED_LOOP:
        dashboardStatus = (UIDashboardError) dashboardPowerTheftDisplay();
        if ( POWER_THEFT_DETECTED_PROCEED == dashboardStatus )
        {
            g_projectDashboardState = UI_DASHBOARD_SETUP;
        }
        break;
    default:
        break;
    }
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