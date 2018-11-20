#include "rws_ntp.h"
#include "../../include/settings/smartsensor_settings.h"

TimeChangeRule rCEST = {CEST_ABBREV, CEST_WEEK, CEST_DOW, CEST_MONTH, CEST_HOUR, CEST_OFFSET};
TimeChangeRule rCET  = {CET_ABBREV,  CET_WEEK,  CET_DOW,  CET_MONTH,  CET_HOUR,  CET_OFFSET};

/**
 * @brief Construct a new smartsensor::rws_ntp object
 * 
 */
rws_ntp::rws_ntp()
: NTPClient(ntpUDP, NTP_SERVER, NTP_OFFSET_S, NTP_UPDATE_INTERVAL_MS), Timezone(rCEST, rCET)
{
}

/**
 * @brief Destroy the smartsensor::rws_ntp object
 * 
 */
rws_ntp::~rws_ntp()
{
}


void rws_ntp::test(void)
{
    time_t utc = getEpochTime();
    time_t local = toLocal(utc);

    Serial.println(); 
    printDateTime(utc, "UTC"); 
    printDateTime(local , "local");
}


std::string rws_ntp::get_local_datetime(void)
{
    time_t t = toLocal(getEpochTime());
    char buf[32]; 
    char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer) 
    strcpy(m, monthShortStr(month(t))); 
    sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d", 
    hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t)); 
    return buf;
}

std::string rws_ntp::get_local_datetime(time_t utc)
{
    time_t t = toLocal(utc);
    char buf[32]; 
    char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer) 
    strcpy(m, monthShortStr(month(t))); 
    sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d", 
    hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t)); 
    return buf;
}

void rws_ntp::printDateTime(time_t t, const char *tz)
{ 
    char buf[32]; 
    char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer) 
    strcpy(m, monthShortStr(month(t))); 
    sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d %s", 
    hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t), tz); 
    Serial.println(buf); 
} 