#include "twilio_sms.h"
#include "credentials/credentials.h"

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

    success = twilio->send_message(number, from_number, smsMessage, response);
    
    if (success) {
    Serial.println("Sent message successfully!");
    } else {
    Serial.println(response);
    }

    return success;
}

bool sendPowerTheftDetected( void )
{


}

bool sendHouseIntruder( void )
{

}

bool sendFireDetected( void )
{

}