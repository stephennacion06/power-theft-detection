#ifndef THINGSPEAK_SECURITY_H
#define THINGSPEAK_SECURITY_H

extern uint32_t g_homeOwnerWattageMax;
extern String  g_homeOwnerAddress;
extern String  g_homeOwnerContactNumber;

extern float g_homeOwnerLatitude;
extern float g_homeOwnerLongtitude;

bool updateHomeOwnerInformation( const uint32_t targetChannelId );

#endif //THINGSPEAK_SECURITY_H