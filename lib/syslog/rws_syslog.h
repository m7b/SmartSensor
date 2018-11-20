#ifndef RWS_SYSLOG_H
#define RWS_SYSLOG_H

#include <WiFiUdp.h>
#include <Syslog.h>

class rws_syslog : public Syslog
{
    public:
        rws_syslog();
        ~rws_syslog();

    private:
        WiFiUDP syslogUDP;
};

#endif // RWS_SYSLOG_H