#ifndef THINGSPEAK_SECURITY_H
#define THINGSPEAK_SECURITY_H

#define THINGSPEAK_WATTAGE_CHANNEL         1
#define THINGPSEAK_FIRE_DETECTION_CHANNEL  2
#define THINGSPEAK_POWER_DETECTION_CHANNEL 3

extern uint32_t g_homeOwnerWattageMax;
extern String  g_homeOwnerAddress;
extern String  g_homeOwnerContactNumber;
extern String g_basteStationContactNumber;

extern String g_homeOwnerLatitude;
extern String g_homeOwnerLongtitude;

bool updateHomeOwnerInformation( const uint32_t targetChannelId );

void thingSpeakSetup( void );

void thingSpeakTransmit( float wattage, bool fireStatus, bool powerTheftStatus );

void thingSpeakTransmitFireDetected( void );

void thingSpeakTransmitPowerTheftDetected( void );

#endif //THINGSPEAK_SECURITY_H