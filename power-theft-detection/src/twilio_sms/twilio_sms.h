#ifndef TWILLIO_SMS_H
#define TWILLIO_SMS_H

#include <Arduino.h>
#include "twilio.hpp"

void initTwilio( void );
bool sendInitMessage( void );
bool sendPowerTheftDetected( void );
bool sendHouseIntruder( void );
bool sendFireDetected( void );

#endif //TWILLIO_SMS_H