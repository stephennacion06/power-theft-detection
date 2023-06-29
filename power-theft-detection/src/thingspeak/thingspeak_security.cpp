#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "thingspeak_security.h"
#include "credentials/credentials.h"

uint32_t g_homeOwnerWattageMax = 0;
String  g_homeOwnerAddress;
String  g_homeOwnerContactNumber;

float g_homeOwnerLatitude = 0;
float g_homeOwnerLongtitude = 0;

static void extractParameters( String inputString );

bool updateHomeOwnerInformation( const uint32_t targetChannelId )
{

    Serial.println(targetChannelId);
    bool returnStatus = false;

    String url = "https://api.thingspeak.com/channels.json?api_key=" + String(SECRET_WRITE_APIKEY);
    HTTPClient http;
    http.begin(url);

    // Make the GET request
    int httpResponseCode = http.GET();

  if (httpResponseCode == HTTP_CODE_OK) {
    String response = http.getString();
    Serial.println("Response: " + response);

    // Parse the JSON response
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, response);

    if (error) {
      Serial.print("JSON Parsing Error: ");
      Serial.println(error.c_str());
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
        String latitude = channel["latitude"].as<String>();
        String longitude = channel["longitude"].as<String>();

        // Print the channel details
        Serial.print("Channel ID: ");
        Serial.println(channelId);
        Serial.print("Channel Name: ");
        Serial.println(channelName);
        Serial.print("Channel Description: ");
        Serial.println(channelDescription);
        Serial.print("Latitude: ");
        Serial.println(latitude);
        Serial.print("Longtitude: ");
        Serial.println(longitude);
        Serial.println("--------Extract Information -----");
        extractParameters(channelDescription);
        returnStatus = true;
        }
    }
  } else {
    Serial.print("HTTP Request Error: ");
    Serial.println(httpResponseCode);
  }

  return returnStatus;
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

    // Trim whitespace
    wattage.trim();
    contactNumber.trim();
    address.trim();

    g_homeOwnerWattageMax = wattage.toInt();
    g_homeOwnerAddress = address;
    g_homeOwnerContactNumber = contactNumber;


    // Print the extracted parameters
    Serial.println("Maximum Wattage Use(W): " + String(g_homeOwnerWattageMax));
    Serial.println("Contact Number: " + g_homeOwnerContactNumber);
    Serial.println("Address: " + g_homeOwnerAddress);
    
}