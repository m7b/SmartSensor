#ifndef SMARTSENSOR_NTP_H
#define SMARTSENSOR_NTP_H

#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Timezone.h> 

class smartsensor_ntp : public NTPClient, Timezone
{
    public:
        smartsensor_ntp();
        ~smartsensor_ntp();

        void test(void);
        std::string get_local_datetime(void);

    private:
        WiFiUDP ntpUDP;

        void printDateTime(time_t t, const char *tz);
};

#endif // SMARTSENSOR_NTP_H