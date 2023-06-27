#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H
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
    TEMPORARY
} UIuserDashboardState;


extern UIuserSetupState volatile projectCurrentSetupState;
extern UIuserDashboardState volatile projectCurrentDashboardState;

void setupFlow( void );
void dashboardFlow( void );

#endif  // STATE_MACHINE_H