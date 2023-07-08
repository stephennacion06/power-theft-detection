#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#define DONE_SET_VALUE 99
#define MAXIMUM_CHANNEL_ID_NUMBER 7

typedef enum enumUIDashboard
{
    BLANK_ERROR_MESSAGE,
    POWER_THEFT_DETECTED_UI,
    POWER_THEFT_DETECTED_PROCEED,
    FIRE_DETECTED_UI
} UIDashboardError;

// Enumerated type for the states
typedef enum {
    NOT_USED,
    UI_HOME_SECURITY_SETUP_TOUCH_CALIBRATION,
    UI_HOME_SECURITY_SETUP_TEXT,
    UI_HOME_SECURITY_SETUP_CHANNEL_ID_TEXT,
    UI_HOME_SECURITY_SETUP_CHANNEL_ID_KEYPAD,
    UI_HOME_SECURITY_SETUP_CHANNEL_ID_KEYPAD_LOOP,
    UI_HOME_SECURITY_SETUP_PLEASE_WAIT_TEXT,
    UI_HOME_SECUIRTY_SETUP_DONE_SETUP
} UIuserSetupState;

typedef enum {
    UI_DASHBOARD_SETUP,
    UI_DASHBOARD_LOOP,
    UI_DASHBOARD_POWER_THEFT_DETECTED_SETUP,
    UI_DASHBOARD_POWER_THEFT_DETECTED_LOOP,
} UIdashboardState;

typedef enum {
    TEMPORARY
} UIuserDashboardState;

extern uint32_t g_customerChannelID;
extern UIuserSetupState volatile g_projectCurrentSetupState;
extern UIuserDashboardState volatile projectCurrentDashboardState;

void setupFlow( void );
void dashboardFlow( void );

#endif  // STATE_MACHINE_H