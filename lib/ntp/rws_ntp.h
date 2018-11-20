#ifndef RWS_NTP_H
#define RWS_NTP_H

#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Timezone.h> 

class rws_ntp : public NTPClient
{
    public:
        rws_ntp(const char* poolServerName, int timeOffset, int updateInterval, Timezone *tz);
        ~rws_ntp();

        void setRules(TimeChangeRule dstStart, TimeChangeRule stdStart);

        void test(void);
        std::string get_local_datetime(void);
        std::string get_local_datetime(time_t utc);

    private:
        WiFiUDP ntpUDP;
        Timezone *tz;

        void printDateTime(time_t t, const char *tz);
};

#endif // RWS_NTP_H