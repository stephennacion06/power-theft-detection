#include "twilio_sms.h"
#include "credentials/credentials.h"
#include "thingspeak/thingspeak_security.h"
#include "debug_serial.h"

// Values from Twilio (find them on the dashboard)
const char *account_sid = ACCOUNT_SSID_TWILLIO;
const char *auth_token = ACCOUNT_AUTHENTICATION_TWILLIO;
const char *from_number = ACCOUNT_PHONE_NUMBER_TWILLIO;

static Twilio *twilio;

static String number = "+639193567298";
static String Latitude = "14.5716272";
static String Longtitude = "120.995249";
static String Address = " 2087 A-Smith Malate, Manila";

void initTwilio( void )
{
    twilio = new Twilio(account_sid, auth_token);
}

bool sendInitMessage( void )
{
    String smsMessage = "Home Security Device Activated!";
    String response;
    bool success;

    success = twilio->send_message(g_homeOwnerContactNumber, from_number, smsMessage, response);
    
    if (success) {
    DEBUG_PRINT_LN("Sent message successfully!");
    } else {
    DEBUG_PRINT_LN(response);
    }

    return success;
}

bool sendPowerTheftDetected( void )
{

    String smsMessage = "WARNING! POWERTHEFT DETECTED! LOCATED AT " + g_homeOwnerAddress + ". Google Map Location: ";
    String gpsURL = "http://www.google.com/maps/place/" + g_homeOwnerLatitude + "," + g_homeOwnerLongtitude;
    smsMessage = smsMessage + gpsURL;
    String response;
    bool success;

    DEBUG_PRINT_LN(smsMessage);

    success = twilio->send_message(g_homeOwnerContactNumber, from_number, smsMessage, response);
    
    if (success)
    {
        DEBUG_PRINT_LN("Sent message successfully!");
    } 
    else 
    {
        DEBUG_PRINT_LN(response);
    }

    success = twilio->send_message(g_basteStationContactNumber, from_number, smsMessage, response);
    if (success) 
    {
        DEBUG_PRINT_LN("Sent message successfully!");
    } 
    else 
    {
        DEBUG_PRINT_LN(response);
    }

    return success;
}

bool sendHouseIntruder( void )
{
    String smsMessage = "WARNING! LOCK/MOTION SENSOR ACTIVATED!";
    String response;
    bool success;

    DEBUG_PRINT_LN(smsMessage);

    success = twilio->send_message(g_homeOwnerContactNumber, from_number, smsMessage, response);
    
    if (success) {
    DEBUG_PRINT_LN("Sent message successfully!");
    } else {
    DEBUG_PRINT_LN(response);
    }
    return success;
}

bool sendFireDetected( void )
{
    String smsMessage = "WARNING! FIRE DETECTED AT " + g_homeOwnerAddress + ". Google Map Location: ";
    String gpsURL = "http://www.google.com/maps/place/" + g_homeOwnerLatitude + "," + g_homeOwnerLongtitude;
    smsMessage = smsMessage + gpsURL;
    String response;
    bool success;

    DEBUG_PRINT_LN(smsMessage);
    
    success = twilio->send_message(g_homeOwnerContactNumber, from_number, smsMessage, response);
    if (success) 
    {
        DEBUG_PRINT_LN("Sent message successfully!");
    } 
    else 
    {
        DEBUG_PRINT_LN(response);
    }

    success = twilio->send_message(g_basteStationContactNumber, from_number, smsMessage, response);
    if (success) 
    {
        DEBUG_PRINT_LN("Sent message successfully!");
    } 
    else 
    {
        DEBUG_PRINT_LN(response);
    }

    return success;
}