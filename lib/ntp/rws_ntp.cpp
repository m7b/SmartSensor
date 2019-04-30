#include "rws_ntp.h"

/**
 * @brief Construct a new smartsensor::rws_ntp object
 *
 */
rws_ntp::rws_ntp(const char* poolServerName, int timeOffset, int updateInterval, Timezone *tz)
: NTPClient(ntpUDP, poolServerName, timeOffset, updateInterval), tz(tz)
{
}

/**
 * @brief Destroy the smartsensor::rws_ntp object
 *
 */
rws_ntp::~rws_ntp()
{
}


void rws_ntp::setRules(TimeChangeRule dstStart, TimeChangeRule stdStart)
{
    tz->setRules(dstStart, stdStart);
}

void rws_ntp::test(void)
{
    time_t utc = getEpochTime();
    time_t local = tz->toLocal(utc);

    printDateTime(utc, "UTC");
    printDateTime(local , "local");
}

void rws_ntp::test(unsigned long delay_ms)
{
    static unsigned long start = 0;

    if (get_duration_ms(start) >= delay_ms)
    {
        start = millis();
        test();
    }
}


std::string rws_ntp::get_local_datetime(void)
{
    time_t t = tz->toLocal(getEpochTime());
    char buf[32];
    char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer)
    strcpy(m, monthShortStr(month(t)));
    sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d",
    hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t));
    return buf;
}

std::string rws_ntp::get_local_datetime(time_t utc)
{
    time_t t = tz->toLocal(utc);
    char buf[32];
    char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer)
    strcpy(m, monthShortStr(month(t)));
    sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d",
    hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t));
    return buf;
}

void rws_ntp::printDateTime(time_t t, const char *tz)
{
    char buf[48];
    char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer)
    strcpy(m, monthShortStr(month(t)));
    sprintf(buf, "  - %.2d:%.2d:%.2d %s %.2d %s %d %s",
    hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t), tz);
    Serial.println(buf);
}