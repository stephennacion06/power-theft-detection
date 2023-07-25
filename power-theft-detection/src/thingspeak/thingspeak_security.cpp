#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "thingspeak_security.h"
#include "credentials/credentials.h"
#include "state_machine/statemachine.h"
#include "ThingSpeak.h"
#include <EEPROM.h>
#include "debug_serial.h"

WiFiClient  client;

uint32_t g_homeOwnerWattageMax = 0;
String  g_homeOwnerAddress;
String  g_homeOwnerContactNumber;
String  g_basteStationContactNumber;

String g_homeOwnerLatitude;
String g_homeOwnerLongtitude;

static void extractParameters( String inputString );
static int readFlashChannelID( void );

bool updateHomeOwnerInformation( const uint32_t targetChannelId )
{

    DEBUG_PRINT_LN(targetChannelId);
    bool returnStatus = false;

    String url = "https://api.thingspeak.com/channels.json?api_key=" + String(SECRET_WRITE_APIKEY);
    HTTPClient http;
    http.begin(url);

    // Make the GET request
    int httpResponseCode = http.GET();

  if (httpResponseCode == HTTP_CODE_OK) {
    String response = http.getString();

    // Parse the JSON response
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, response);

    if (error) {
      DEBUG_PRINT("JSON Parsing Error: ");
      DEBUG_PRINT_LN(error.c_str());
      return returnStatus;
    }

    // Iterate over the channels and find the desired channel
    JsonArray channels = doc.as<JsonArray>();

    for (JsonVariant channel : channels) {
      int channelId = channel["id"].as<int>();

      if (channelId == targetChannelId) 
        {
        // Found the desired channel
        String channelName = channel["name"].as<String>();
        String channelDescription = channel["description"].as<String>();
        g_homeOwnerLatitude = channel["latitude"].as<String>();
        g_homeOwnerLongtitude = channel["longitude"].as<String>();

        // Print the channel details
        DEBUG_PRINT("Channel ID: ");
        DEBUG_PRINT_LN(channelId);
        DEBUG_PRINT("Channel Name: ");
        DEBUG_PRINT_LN(channelName);
        DEBUG_PRINT("Channel Description: ");
        DEBUG_PRINT_LN(channelDescription);
        DEBUG_PRINT("Latitude: ");
        DEBUG_PRINT_LN(g_homeOwnerLatitude);
        DEBUG_PRINT("Longtitude: ");
        DEBUG_PRINT_LN(g_homeOwnerLongtitude);
        DEBUG_PRINT_LN("--------Extract Information -----");
        extractParameters(channelDescription);
        returnStatus = true;
        }
    }
  } else {
    DEBUG_PRINT("HTTP Request Error: ");
    DEBUG_PRINT_LN(httpResponseCode);
  }

  return returnStatus;
}

void thingSpeakSetup( void )
{
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void thingSpeakTransmit( float wattage, bool fireStatus, bool powerTheftStatus )
{
  int homeOnwerChannelID = readFlashChannelID();

  ThingSpeak.setField(THINGSPEAK_WATTAGE_CHANNEL, wattage);
  ThingSpeak.setField(THINGPSEAK_FIRE_DETECTION_CHANNEL, fireStatus);
  ThingSpeak.setField(THINGSPEAK_POWER_DETECTION_CHANNEL, powerTheftStatus);

  if( fireStatus )
  {
    ThingSpeak.setStatus("FIRE DETECTED!");
  }
  
  if ( powerTheftStatus )
  {
    ThingSpeak.setStatus("POWER THEFT DETECTED!");
  }

  int x = ThingSpeak.writeFields(homeOnwerChannelID, WRITE_API_KEY);
  if(x == 200){
    DEBUG_PRINT_LN("Channel update successful.");
  }
  else{
    DEBUG_PRINT_LN("Problem updating channel. HTTP error code " + String(x));
  }
}

void thingSpeakTransmitFireDetected( void )
{
    int homeOnwerChannelID = readFlashChannelID();
    
    ThingSpeak.setField(THINGPSEAK_FIRE_DETECTION_CHANNEL, 1);

    ThingSpeak.setStatus("FIRE DETECTED!");

    int x = ThingSpeak.writeFields(homeOnwerChannelID, WRITE_API_KEY);
    if(x == 200){
      DEBUG_PRINT_LN("Channel update successful.");
    }
    else{
      DEBUG_PRINT_LN("Problem updating channel. HTTP error code " + String(x));
    }
}

void thingSpeakTransmitPowerTheftDetected( void )
{
    int homeOnwerChannelID = readFlashChannelID();
    
    ThingSpeak.setField(THINGSPEAK_POWER_DETECTION_CHANNEL, 1);

    ThingSpeak.setStatus("POWER THEFT DETECTED!");

    int x = ThingSpeak.writeFields(homeOnwerChannelID, WRITE_API_KEY);
    if(x == 200){
      DEBUG_PRINT_LN("Channel update successful.");
    }
    else{
      DEBUG_PRINT_LN("Problem updating channel. HTTP error code " + String(x));
    }
}

static void extractParameters( String inputString )
{
    // Extract maximum wattage use
    int startIndex = inputString.indexOf(":") + 2;  // Find the starting index
    int endIndex = inputString.indexOf(";", startIndex);  // Find the ending index
    String wattage = inputString.substring(startIndex, endIndex);  // Extract the wattage

    // Extract contact number
    startIndex = inputString.indexOf(":", endIndex) + 2;  // Find the starting index
    endIndex = inputString.indexOf(";", startIndex);  // Find the ending index
    String contactNumber = inputString.substring(startIndex, endIndex);  // Extract the contact number

    // Extract address
    startIndex = inputString.indexOf(":", endIndex) + 2;  // Find the starting index
    endIndex = inputString.indexOf(";", startIndex);  // Find the ending index
    String address = inputString.substring(startIndex, endIndex);  // Extract the address

    // Extract address
    startIndex = inputString.indexOf(":", endIndex) + 2;  // Find the starting index
    endIndex = inputString.indexOf(";", startIndex);  // Find the ending index
    String baseStationNumber = inputString.substring(startIndex, endIndex);  // Extract the address

    startIndex = inputString.indexOf(":", endIndex) + 2;  // Find the starting index
    endIndex = inputString.indexOf(";", startIndex);  // Find the ending index
    String accountStatus = inputString.substring(startIndex, endIndex);  // Extract the address

    // Trim whitespace
    wattage.trim();
    contactNumber.trim();
    address.trim();
    baseStationNumber.trim();
    accountStatus.trim();

    if ( accountStatus == "Disabled" )
    {
        DEBUG_PRINT_LN("Account Disabled Restarting!");
        EEPROM.write(MAXIMUM_CHANNEL_ID_NUMBER, 00);
        EEPROM.commit();
        ESP.restart();
    }

    g_homeOwnerWattageMax = wattage.toInt();
    g_homeOwnerAddress = address;
    g_homeOwnerContactNumber = contactNumber;
    g_basteStationContactNumber = baseStationNumber;


    // Print the extracted parameters
    DEBUG_PRINT_LN("Maximum Wattage Use(W): " + String(g_homeOwnerWattageMax));
    DEBUG_PRINT_LN("Contact Number: " + g_homeOwnerContactNumber);
    DEBUG_PRINT_LN("Address: " + g_homeOwnerAddress);
    DEBUG_PRINT_LN("Base Station Number: " + g_basteStationContactNumber);
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