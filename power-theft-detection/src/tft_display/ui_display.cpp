#include <Arduino.h>
#include "ui_display.h"
#include <TFT_eSPI.h> // Graphics and font library for ILI9341 driver chip
#include <SPI.h>
#include <TFT_eWidget.h>  // Widget library
#include <EEPROM.h>
#include "state_machine/statemachine.h"
#include "thingspeak/thingspeak_security.h"
#include "twilio_sms/twilio_sms.h"
#include "sensors/sensors.h"
#include "Free_Fonts.h" // Include the header file attached to this sketch
#include <Ticker.h>
#include "debug_serial.h"

static TFT_eSPI tft = TFT_eSPI();  // Invoke library
static MeterWidget  wattageWidget  = MeterWidget(&tft);
static ButtonWidget lockSensorButton = ButtonWidget(&tft);
static ButtonWidget motionSensorButton = ButtonWidget(&tft);
static ButtonWidget fireSensorButton = ButtonWidget(&tft);
static ButtonWidget relayButton = ButtonWidget(&tft);
static ButtonWidget powerTheftButton = ButtonWidget(&tft);

// Dashboard
static unsigned long updateTime = 0;  
static unsigned long updateTimeStatus = 0;
static unsigned long transmitTime = 0;

// Button Widget
static bool doorButtonIsEnanbled = false;
static bool motionButtonIsEnabled = false;
static bool fireButtonIsEnabled = false;
static bool relayButtonIsEnabled = false;
static unsigned long doorMillis = 0;
static unsigned long motionMillis = 0;
static unsigned long fireMillis = 0;
static unsigned long doorTwillioMillis = 0;
static unsigned long motionTwillioMillis = 0;
static unsigned long fireTwillioMillis = 0;
#define UPDATE_STATUS_TIME 60000 // 1 minute
#define UPDATE_TWILLIO_TIME 60000 // 1 minute
#define TOUCHSCREEN_CHECK_INTERVAL 1

#define BUTTON_W 80
#define BUTTON_H 42
#define STARTING_H 60
#define SPACE_H 5

#define BUTTON_X 0
#define DOOR_BUTTON_Y   ( ( tft.height() / 2 ) - BUTTON_H + STARTING_H)
#define MOTION_BUTTON_Y ( ( tft.height() / 2 ) + STARTING_H + SPACE_H )
#define FIRE_BUTTON_Y   ( ( tft.height() / 2 ) + BUTTON_H + STARTING_H +  ( SPACE_H * 2 ) )

#define RELAY_BUTTON_H 42
#define RELAY_BUTTON_W 100
#define RELAY_BUTTON_PADDING_RIGHT 10
#define RELAY_BUTTON_X  ( ( tft.width() / 2 ) - 45)
#define RELAY_BUTTON_Y   DOOR_BUTTON_Y - ( SPACE_H + RELAY_BUTTON_H )

// Status
#define STATUS_X_POSITION BUTTON_W + 5
#define DOOR_STATUS_Y   ( DOOR_BUTTON_Y   + BUTTON_H/2 )
#define MOTION_STATUS_Y ( MOTION_BUTTON_Y + BUTTON_H/2 )
#define FIRE_STATUS_Y   ( FIRE_BUTTON_Y   + BUTTON_H/2 )

#define CENTER_TEXT_PADDING 30

// Create an array of button instances to use in for() loops
// This is more useful where large numbers of buttons are employed
ButtonWidget* btn[] = {&lockSensorButton , &motionSensorButton, &fireSensorButton, &relayButton};
uint8_t buttonCount = sizeof(btn) / sizeof(btn[0]);

// Create 15 keys for the keypad
static char keyLabel[15][5] = {"New", "Del", "Send", "1", "2", "3", "4", "5", "6", "7", "8", "9", ".", "0", "#" };
static uint16_t keyColor[15] = {TFT_RED, TFT_DARKGREY, TFT_DARKGREEN,
                         TFT_BLUE, TFT_BLUE, TFT_BLUE,
                         TFT_BLUE, TFT_BLUE, TFT_BLUE,
                         TFT_BLUE, TFT_BLUE, TFT_BLUE,
                         TFT_BLUE, TFT_BLUE, TFT_BLUE
                        };
// Number length, buffer for storing it and character index
#define NUM_LEN 12
static char numberBuffer[NUM_LEN + 1] = "";
static uint8_t numberIndex = 0;

// Invoke the TFT_eSPI button class and create all the button objects
TFT_eSPI_Button key[15];


/* -------------------- Private Function Declaration -------------------------------- */
static void drawKeypad( void );
static void status(const char *msg);
static void wattageWidgetSetup( void );
static void buttonWidgetSetup( void );
static void lockSensorButtonPressAction(void);
static void motionSensorButtonPressAction(void);
static void fireSensorButtonPressAction(void);
static void powerTheftButtonPressAction(void);
static void lockSensorButtonReleaseAction(void);
static void motionSensorButtonReleaseAction(void);
static void fireSensorButtonReleaseAction(void);
static void relayButtonReleaseAction(void);
static void relayButtonPressAction(void);
static void touchscreencheck(void);
static void statusDoorSetup(void);
static void statusMotionSetup(void);
static void statusFireSetup(void);
static void updateStatusDoorSetup(void);
static void updateStatusMotionSetup(void);
static bool updateStatusFireSetup(void);
/* -------------------- Public Function Definition -------------------------------- */
void initTFT( void )
{
    tft.init();
    tft.setRotation(2);
    tft.fillScreen(TFT_BLACK);
}

void displaySetupTextInsertChannelID( void )
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
    // Set the font colour to be yellow with no background, set to font 7
    tft.setTextColor(TFT_WHITE); tft.setTextFont(4);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("PLEASE INSERT", tft.width()/2,tft.height()/2-CENTER_TEXT_PADDING);
    tft.drawString("HOME OWNER", tft.width()/2,tft.height()/2);
    tft.drawString("CHANNEL ID", tft.width()/2,tft.height()/2+CENTER_TEXT_PADDING);
}

void displaySetupConnectToWifi( void )
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
  // Set the font colour to be yellow with no background, set to font 7
    tft.setTextColor(TFT_WHITE); tft.setTextFont(4);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("CONNECTING", tft.width()/2,tft.height()/2-CENTER_TEXT_PADDING);
    tft.drawString("TO", tft.width()/2,tft.height()/2);
    tft.drawString("WIFI", tft.width()/2,tft.height()/2+CENTER_TEXT_PADDING);
}

void displaySetupConnectToWifiFailed( void )
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
  // Set the font colour to be yellow with no background, set to font 7
    tft.setTextColor(TFT_WHITE); tft.setTextFont(4);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("CONNECTING", tft.width()/2,tft.height()/2-CENTER_TEXT_PADDING);
    tft.drawString("TO", tft.width()/2,tft.height()/2);
    tft.drawString("WIFI", tft.width()/2,tft.height()/2+CENTER_TEXT_PADDING);
    tft.drawString("FAILED!", tft.width()/2,tft.height()/2+CENTER_TEXT_PADDING*2);
    tft.drawString("RESTARTING!!", tft.width()/2,tft.height()/2+CENTER_TEXT_PADDING*3);
}

void displaySetupConnectToWifiPassed( void )
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
  // Set the font colour to be yellow with no background, set to font 7
    tft.setTextColor(TFT_WHITE); tft.setTextFont(4);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("CONNECTING", tft.width()/2,tft.height()/2-CENTER_TEXT_PADDING);
    tft.drawString("TO", tft.width()/2,tft.height()/2);
    tft.drawString("WIFI", tft.width()/2,tft.height()/2+CENTER_TEXT_PADDING);
    tft.drawString("SUCCESFUL!", tft.width()/2,tft.height()/2+CENTER_TEXT_PADDING*2);
}

void displayThingspeakExtraction( void )
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
  // Set the font colour to be yellow with no background, set to font 7
    tft.setTextColor(TFT_WHITE); tft.setTextFont(4);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("LOADING", tft.width()/2,tft.height()/2-CENTER_TEXT_PADDING);
    tft.drawString("HOMEOWNER", tft.width()/2,tft.height()/2);
    tft.drawString("INFORMATION...", tft.width()/2,tft.height()/2+CENTER_TEXT_PADDING);
}

void displayThingspeakExtractionInformation( void )
{
    // Set the font colour to be yellow with no background, set to font 7
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
  // Set the font colour to be yellow with no background, set to font 7
    tft.setTextColor(TFT_WHITE); tft.setTextFont(2);
    tft.setTextDatum(TC_DATUM);
    tft.drawString("HOME OWNER INFORMATION:",  tft.width()/2, 10);
    tft.setTextDatum(TL_DATUM);
    tft.drawString("Maximum Wattage: " + String(g_homeOwnerWattageMax) + "W",  5, CENTER_TEXT_PADDING);
    tft.drawString("Address: " + String(g_homeOwnerAddress),  5, CENTER_TEXT_PADDING*2);
    tft.drawString("Contact Number:" + String(g_homeOwnerContactNumber), 5, CENTER_TEXT_PADDING*3);
    tft.drawString("Station Number:" + String(g_basteStationContactNumber), 5, CENTER_TEXT_PADDING*4);
}

void displaySetupText( void )
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextSize(1);
  // Set the font colour to be yellow with no background, set to font 7
    tft.setTextColor(TFT_WHITE); tft.setTextFont(4);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("HOME SECURITY", tft.width()/2,tft.height()/2);
    tft.drawString("SETUP", tft.width()/2,tft.height()/2 + CENTER_TEXT_PADDING);
}

void touch_calibrate( void )
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // check file system exists
  if (!SPIFFS.begin()) {
    DEBUG_PRINT_LN("Formating file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  // check if calibration file exists and size is correct
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    DEBUG_PRINT_LN("[DEBUG]: Calibration File Exist file");
    if (REPEAT_CAL)
    {
      // Delete if we want to re-calibrate
      SPIFFS.remove(CALIBRATION_FILE);
    }
    else
    {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    // calibration data valid
    tft.setTouch(calData);
    DEBUG_PRINT_LN("[DEBUG]: Calibration data Valid");
  } else {
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // store data
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}

void uiSetupDisplayDrawKeypad( void )
{
    // Clear the screen
  tft.fillScreen(TFT_BLACK);

  // Draw keypad background
  tft.fillRect(0, 0, 240, 320, TFT_DARKGREY);

    // Draw number display area and frame
  tft.fillRect(DISP_X, DISP_Y, DISP_W, DISP_H, TFT_BLACK);
  tft.drawRect(DISP_X, DISP_Y, DISP_W, DISP_H, TFT_WHITE);

  // Draw keypad
  drawKeypad();
}

void uiSetupDisplayKeypadLoop( void )
{
  uint16_t t_x = 0, t_y = 0; // To store the touch coordinates

  // Pressed will be set true is there is a valid touch on the screen
  bool pressed = tft.getTouch(&t_x, &t_y);

  // / Check if any key coordinate boxes contain the touch coordinates
  for (uint8_t b = 0; b < 15; b++) {
    if (pressed && key[b].contains(t_x, t_y)) {
      key[b].press(true);  // tell the button it is pressed
    } else {
      key[b].press(false);  // tell the button it is NOT pressed
    }
  }

  // Check if any key has changed state
  for (uint8_t b = 0; b < 15; b++) {

    if (b < 3) tft.setFreeFont(LABEL1_FONT);
    else tft.setFreeFont(LABEL2_FONT);

    if (key[b].justReleased()) key[b].drawButton();     // draw normal

    if (key[b].justPressed()) {
      key[b].drawButton(true);  // draw invert

      // if a numberpad button, append the relevant # to the numberBuffer
      if (b >= 3) {
        if (numberIndex < NUM_LEN) {
          numberBuffer[numberIndex] = keyLabel[b][0];
          numberIndex++;
          numberBuffer[numberIndex] = 0; // zero terminate
        }
        status(""); // Clear the old status
      }

      // Del button, so delete last char
      if (b == 1) {
        numberBuffer[numberIndex] = 0;
        if (numberIndex > 0) {
          numberIndex--;
          numberBuffer[numberIndex] = 0;//' ';
        }
        status(""); // Clear the old status
      }

      if (b == 2) {
        // NOTE: SEND BUTTON HANDLER
        int converterNumber = std::stoi(numberBuffer);

        if (converterNumber >= validNumberMin && converterNumber <= validNumberMax)
        {
          status("Setting Client Channel ID");
          DEBUG_PRINT_LN("[DEBUG] ConverterNumber Value: " + String(converterNumber));
          // Update flag address MAXIMUM_CHANNEL_ID_NUMBER
          
          EEPROM.write(MAXIMUM_CHANNEL_ID_NUMBER, DONE_SET_VALUE);;
          EEPROM.commit();
          
          saveToFlashChannelID(converterNumber);
        } else {
          status("ERROR! Invalid Client Channel ID");
        }
      }
      // we dont really check that the text field makes sense
      // just try to call
      if (b == 0) {
        status("Value cleared");
        numberIndex = 0; // Reset index to 0
        numberBuffer[numberIndex] = 0; // Place null in buffer
      }

      // Update the number display field
      tft.setTextDatum(TL_DATUM);        // Use top left corner as text coord datum
      tft.setFreeFont(&FreeSans18pt7b);  // Choose a nicefont that fits box
      tft.setTextColor(DISP_TCOLOR);     // Set the font colour

      // Draw the string, the value returned is the width in pixels
      int xwidth = tft.drawString(numberBuffer, DISP_X + 4, DISP_Y + 12);

      // Now cover up the rest of the line up by drawing a black rectangle.  No flicker this way
      // but it will not work with italic or oblique fonts due to character overlap.
      tft.fillRect(DISP_X + 4 + xwidth, DISP_Y + 1, DISP_W - xwidth - 5, DISP_H - 2, TFT_BLACK);

      delay(10); // UI debouncing
    }
  }
}

void saveToFlashChannelID(int value) 
{
    for (int i = 0; i < MAXIMUM_CHANNEL_ID_NUMBER; i++) 
    {
    byte digit = value % 10;
    EEPROM.write(i, digit);
    value /= 10;
    }
    EEPROM.commit();
}

void dashboardSetup( void )
{
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW); tft.setTextFont(2);
  tft.setTextSize(1);

  DEBUG_PRINT_LN("Setting Current Sensor");
  currentSensorSetup();
  DEBUG_PRINT_LN("Setting Voltage Sensor");
  voltageSensorSetup();
  
  thingSpeakSetup();

  tft.setCursor(STATUS_X_POSITION, FIRE_STATUS_Y, 1);
  tft.setTextColor(TFT_GREEN,TFT_BLACK,true);  tft.setTextSize(1);

  touch_calibrate();
  buttonWidgetSetup();
  wattageWidgetSetup();
  
  statusFireSetup();
  statusDoorSetup();
  statusMotionSetup();

  pinMode(DOOR_SENSOR_PIN, INPUT);  // Set the specified pin as input
  pinMode(FIRE_SENSOR_PIN, INPUT);  // Set the specified pin as input
  pinMode(MOTION_SENSOR_PIN, INPUT); // Set the specified pin as input

  powerTheftDetectionSetup();
}

int dashboardLoop( void )
{
  int errorMessage = (UIDashboardError) BLANK_ERROR_MESSAGE;
  float wattage = 0;
  bool powerTheftStatus = false;
  bool fireDetectedStatus = false;
  float current = 0;
  float voltage = 0;
  
  current = getCurrent();
  voltage = getVoltage();

  touchscreencheck();

  if (millis() - updateTimeStatus >= DASHBOARD_STATUS_LOOP_PERIOD) 
  {
    updateTimeStatus = millis();
    updateStatusDoorSetup();
    updateStatusMotionSetup();
    fireDetectedStatus = updateStatusFireSetup();
  }

  if (millis() - updateTime >= DASHBOARD_LOOP_PERIOD) 
  {
    updateTime = millis();
    
    wattage = current*voltage;

    powerTheftStatus = powerTheftDetection( wattage, relayButtonIsEnabled );
    
    wattageWidget.updateNeedle( wattage, 0 );
  }

  if (millis() - transmitTime >= THINGSPEAK_TRANSMIT_INTERVAL) 
  {
    transmitTime = millis();
    thingSpeakTransmit( wattage, fireDetectedStatus, powerTheftStatus );
  }

  if (powerTheftStatus)
  {
      digitalWrite(RELAY_PIN, LOW);

      errorMessage = (UIDashboardError) POWER_THEFT_DETECTED_UI;
  }
  return  errorMessage;
}

// TODO: POWERTHEFT DISPLAY
int dashboardPowerTheftDisplay( void )
{
  int returnVal = (UIDashboardError) BLANK_ERROR_MESSAGE;
  uint16_t t_x = 0, t_y = 0;
  
  bool pressed = tft.getTouch(&t_x, &t_y);

  if( pressed && powerTheftButton.contains(t_x, t_y))
  {
    returnVal = (UIDashboardError) POWER_THEFT_DETECTED_PROCEED;
  }
  delay( 50 );

  return returnVal;
}

void dashBoardPowerTheftDetectedSetup( void )
{
    relayButtonIsEnabled = false;
    tft.setCursor(tft.width()/2 - 100, tft.height()/2 - 100);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_YELLOW); tft.setTextFont(2);
    tft.setTextSize(2);
    tft.println(" POWER THEFT");
    tft.println("    DETECTED!");

    powerTheftButton.initButton(tft.width() / 2 , tft.height() / 2, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_BLACK, TFT_GREEN, "PROCEED ", 1);
    powerTheftButton.setPressAction(powerTheftButtonPressAction);
    powerTheftButton.drawSmoothButton(false, 3, TFT_BLACK); // 3 is outline width, TFT_BLACK is the surrounding background colour for anti-aliasing

    thingSpeakTransmitPowerTheftDetected();
    sendPowerTheftDetected();
}

/* -------------------- Private Function Definition -------------------------------- */
static void drawKeypad( void )
{
  // Draw the keys
  for (uint8_t row = 0; row < 5; row++)
  {
    for (uint8_t col = 0; col < 3; col++) {
      uint8_t b = col + row * 3;

      if (b < 3) tft.setFreeFont(LABEL1_FONT);
      else tft.setFreeFont(LABEL2_FONT);

      key[b].initButton(&tft, KEY_X + col * (KEY_W + KEY_SPACING_X),
                        KEY_Y + row * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        KEY_W, KEY_H, TFT_WHITE, keyColor[b], TFT_WHITE,
                        keyLabel[b], KEY_TEXTSIZE);
      key[b].drawButton();
    }
  }
}

// Print something in the mini status bar
static void status(const char *msg) {
  tft.setTextPadding(240);
  //tft.setCursor(STATUS_X, STATUS_Y);
  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  tft.setTextFont(0);
  tft.setTextDatum(TC_DATUM);
  tft.setTextSize(1);
  tft.drawString(msg, STATUS_X, STATUS_Y);
}

static void wattageWidgetSetup( void )
{
  String values[6];
  int numValues = sizeof(values) / sizeof(values[0]);
  for (int i = 0; i < numValues ; i++) 
  {
    int calculatedValue = i * (g_homeOwnerWattageMax / 5);
    values[i] = String(calculatedValue);
  }
  wattageWidget.setZones(75, 100, 50, 75, 25, 50, 0, 25); // Example here red starts at 75% and ends at 100% of full scale
  const char* firstLabel = values[0].c_str();
  const char* SecondLabel = values[1].c_str();
  const char* thirdLabel = values[2].c_str();
  const char* fourthLabel = values[3].c_str();
  const char* fifthLabel = values[4].c_str();
  wattageWidget.analogMeter(0, 0, g_homeOwnerWattageMax, "W", firstLabel, SecondLabel, thirdLabel, fourthLabel, fifthLabel);    // Draw analogue meter at 0, 0
}

static void buttonWidgetSetup( void )
{
  lockSensorButton.initButtonUL(BUTTON_X , DOOR_BUTTON_Y, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_BLACK, TFT_GREEN, " DOOR", 1);
  lockSensorButton.setPressAction(lockSensorButtonPressAction);
  lockSensorButton.setReleaseAction(lockSensorButtonReleaseAction);
  lockSensorButton.drawSmoothButton(false, 3, TFT_BLACK); // 3 is outline width, TFT_BLACK is the surrounding background colour for anti-aliasing


  motionSensorButton.initButtonUL(BUTTON_X, MOTION_BUTTON_Y, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_BLACK, TFT_GREEN, "MOTION", 1);
  motionSensorButton.setPressAction(motionSensorButtonPressAction);
  motionSensorButton.setReleaseAction(motionSensorButtonReleaseAction);
  motionSensorButton.drawSmoothButton(false, 3, TFT_BLACK); // 3 is outline width, TFT_BLACK is the surrounding background colour for anti-aliasing

  fireSensorButton.initButtonUL(BUTTON_X, FIRE_BUTTON_Y, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_BLACK, TFT_GREEN, " FIRE", 1);
  fireSensorButton.setPressAction(fireSensorButtonPressAction);
  fireSensorButton.setReleaseAction(fireSensorButtonReleaseAction);
  fireSensorButton.drawSmoothButton(false, 3, TFT_BLACK); // 3 is outline width, TFT_BLACK is the surrounding background colour for anti-aliasing

  tft.setTextSize(2);
  relayButton.initButtonUL(RELAY_BUTTON_X, RELAY_BUTTON_Y, RELAY_BUTTON_W, RELAY_BUTTON_H, TFT_WHITE, TFT_BLACK, TFT_GREEN, "POWER OFF", 1);
  relayButton.setPressAction(relayButtonPressAction);
  relayButton.setReleaseAction(relayButtonReleaseAction);
  relayButton.drawSmoothButton(false, 3, TFT_BLACK); // 3 is outline width, TFT_BLACK is the surrounding background colour for anti-aliasing

}

static void relayButtonReleaseAction(void)
{

}

static void relayButtonPressAction(void)
{
  if( relayButton.justPressed() )
  {
    tft.setTextSize(2);
    relayButton.drawSmoothButton(!relayButton.getState(), 3, TFT_BLACK, relayButton.getState() ? "POWER OFF" : "POWER ON ");
    relayButtonIsEnabled = !relayButtonIsEnabled;
    relayButton.setPressTime(millis());
  }
}

static void lockSensorButtonPressAction(void)
{
  if (lockSensorButton.justPressed())
  {
    tft.setTextSize(1);
    lockSensorButton.drawSmoothButton(!lockSensorButton.getState(), 3, TFT_BLACK, lockSensorButton.getState() ? " DOOR" : " DOOR");
    doorButtonIsEnanbled = !doorButtonIsEnanbled;
    
    if( doorButtonIsEnanbled )
    {
      doorMillis = millis();
    }

    lockSensorButton.setPressTime(millis());
  }
}

static void motionSensorButtonPressAction(void)
{
    if (motionSensorButton.justPressed())
    {
      tft.setTextSize(1);
      motionSensorButton.drawSmoothButton(!motionSensorButton.getState(), 3, TFT_BLACK, motionSensorButton.getState() ? "MOTION" : "MOTION");
      motionButtonIsEnabled = !motionButtonIsEnabled;

    if( motionButtonIsEnabled )
    {
      motionMillis = millis();
    }

    motionSensorButton.setPressTime(millis());
  }
}

static void fireSensorButtonPressAction(void)
{
    if (fireSensorButton.justPressed())
    {
      tft.setTextSize(1);
      fireSensorButton.drawSmoothButton(!fireSensorButton.getState(), 3, TFT_BLACK, fireSensorButton.getState() ? " FIRE" : " FIRE");
    
      fireButtonIsEnabled = !fireButtonIsEnabled;

      if( fireButtonIsEnabled )
      {
        fireMillis = millis();
      }

    fireSensorButton.setPressTime(millis());
  }
}

static void powerTheftButtonPressAction(void)
{

}

static void lockSensorButtonReleaseAction(void)
{

}

static void motionSensorButtonReleaseAction(void)
{

}

static void fireSensorButtonReleaseAction(void)
{

}

static void touchscreencheck(void)
{
  static uint32_t scanTime = millis();
  uint16_t t_x = 0, t_y = 0; // To store the touch coordinates
  bool pressed = tft.getTouch(&t_x, &t_y);
  if (millis() - scanTime >= TOUCHSCREEN_CHECK_INTERVAL )
  {
    scanTime = millis();

    for (uint8_t b = 0; b < buttonCount; b++)
    {
      if( pressed && btn[b]->contains(t_x, t_y))
      {
        btn[b]->press(true);
        btn[b]->pressAction();
      }     
      else
      {
        btn[b]->press(false);
      }
    }
  }
}

static void statusDoorSetup(void)
{
  tft.setCursor(STATUS_X_POSITION, DOOR_STATUS_Y, 1);
  tft.setTextColor(TFT_GREEN,TFT_BLACK,true); tft.setTextSize(1);
  tft.println("DOOR SENSOR DISABLED");
}

static void statusMotionSetup(void) 
{
  tft.setCursor(STATUS_X_POSITION, MOTION_STATUS_Y, 1);
  tft.setTextColor(TFT_GREEN,TFT_BLACK,true); tft.setTextSize(1);
  tft.println("MOTION SENSOR DISABLED");
}

static void statusFireSetup(void)
{
  tft.setCursor(STATUS_X_POSITION, FIRE_STATUS_Y, 1);
  tft.setTextColor(TFT_GREEN,TFT_BLACK,true);  tft.setTextSize(1);
  tft.println( "FIRE SENSOR DISABLED");
}

static void updateStatusDoorSetup(void)
{
  tft.setCursor(STATUS_X_POSITION, DOOR_STATUS_Y, 1);
  tft.setTextSize(1);

  if ( doorButtonIsEnanbled )
  {
    if ( ( millis() - doorMillis ) >= UPDATE_STATUS_TIME )
    {
      if ( !doorSensorValue() )
      {
        tft.setTextColor(TFT_SKYBLUE,TFT_BLACK,true);
        tft.println("DOOR SENSOR ENABLED  ");
      }

      else
      {
        tft.setTextColor(TFT_RED,TFT_BLACK,true);
        tft.println("DOOR SENSOR ACTIVATED ");
        if ( millis() - doorTwillioMillis >= UPDATE_TWILLIO_TIME ) 
        {
          doorTwillioMillis = millis();
          sendHouseIntruder();
        }
      }
    }
    else
    {
      tft.setTextColor(TFT_WHITE,TFT_BLACK,true);
      tft.println("ENABLING DOOR SENSOR ");
    }
  }
  else
  {
    tft.setTextColor(TFT_GREEN,TFT_BLACK,true); 
    tft.println("DOOR SENSOR DISABLED ");
  }
}

static void updateStatusMotionSetup(void)
{
  tft.setCursor(STATUS_X_POSITION, MOTION_STATUS_Y, 1);
  tft.setTextSize(1);

  if ( motionButtonIsEnabled )
  {
    if ( ( millis() - motionMillis ) >= UPDATE_STATUS_TIME )
    {
      if ( !motionSensorValue() )
      {
        tft.setTextColor(TFT_SKYBLUE,TFT_BLACK,true);
        tft.println("MOTION SENSOR ENABLED ");
      }

      else
      {
        tft.setTextColor(TFT_RED,TFT_BLACK,true);
        tft.println("MOTION SENSOR ACTIVE");
        if ( millis() - motionTwillioMillis >= UPDATE_TWILLIO_TIME ) 
        {
          motionTwillioMillis = millis();
          sendHouseIntruder();
        }
      }
    }
    else
    {
      tft.setTextColor(TFT_WHITE,TFT_BLACK,true);
      tft.println("ENABLING MOTION SENSOR");
    }
  }
  else
  {
    tft.setTextColor(TFT_GREEN,TFT_BLACK,true); 
    tft.println("MOTION SENSOR DISABLED");
  }
}

static bool updateStatusFireSetup(void)
{
  bool fireDetectedStatus = false;
  tft.setCursor(STATUS_X_POSITION, FIRE_STATUS_Y, 1);
  tft.setTextSize(1);

  if ( fireButtonIsEnabled )
  {
    if ( ( millis() - fireMillis ) >= UPDATE_STATUS_TIME )
    {
      if ( !fireSensorValue() )
      {
        tft.setTextColor(TFT_SKYBLUE,TFT_BLACK,true);
        tft.println("FIRE SENSOR ENABLED   ");
      }
      else
      {
        tft.setTextColor(TFT_RED,TFT_BLACK,true);
        tft.println("FIRE SENSOR ACTIVATED!");
        if ( millis() - fireTwillioMillis >= UPDATE_TWILLIO_TIME ) 
        {
          thingSpeakTransmitFireDetected();
          sendFireDetected();
          fireTwillioMillis = millis();
          fireDetectedStatus = true;
        }
      }
    }
    else
    {
      tft.setTextColor(TFT_WHITE,TFT_BLACK,true);
      tft.println("ENABLING FIRE SENSOR ");
    }
  }
  else
  {
    tft.setTextColor(TFT_GREEN,TFT_BLACK,true); 
    tft.println("FIRE SENSOR DISABLED ");
  }
  return fireDetectedStatus;
}