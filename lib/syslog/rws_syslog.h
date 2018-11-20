#ifndef RWS_SYSLOG_H
#define RWS_SYSLOG_H

#include <WiFiUdp.h>
#include <Syslog.h>

class rws_syslog : public Syslog
{
    public:
        rws_syslog(const char* server, uint16_t port, const char* deviceHostname = SYSLOG_NILVALUE, const char* appName = SYSLOG_NILVALUE, uint16_t priDefault = LOG_KERN, uint8_t protocol = SYSLOG_PROTO_IETF);
        ~rws_syslog();

    private:
        WiFiUDP syslogUDP;
};

#endif // RWS_SYSLOG_H