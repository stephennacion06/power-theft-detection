#ifndef THINGSPEAK_SECURITY_H
#define THINGSPEAK_SECURITY_H

extern uint32_t g_homeOwnerWattageMax;
extern String  g_homeOwnerAddress;
extern String  g_homeOwnerContactNumber;

extern String g_homeOwnerLatitude;
extern String g_homeOwnerLongtitude;

bool updateHomeOwnerInformation( const uint32_t targetChannelId );

bool thingSpeakSetup( void );

void thingSpeakTransmit( float wattage, bool fireDetected, bool fireTheftDetect );

#endif //THINGSPEAK_SECURITY_H