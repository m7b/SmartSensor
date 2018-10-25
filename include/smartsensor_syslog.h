#ifndef SMARTSENSOR_SYSLOG_H
#define SMARTSENSOR_SYSLOG_H

#include <WiFiUdp.h>
#include <Syslog.h>

class smartsensor_syslog : public Syslog
{
    public:
        smartsensor_syslog();
        ~smartsensor_syslog();

    private:
        WiFiUDP syslogUDP;
};

#endif // SMARTSENSOR_SYSLOG_H