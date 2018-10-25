#ifndef SMARTSENSOR_SYSLOG_H
#define SMARTSENSOR_SYSLOG_H

#include <WiFiUdp.h>
#include <Syslog.h>

// Syslog server connection info (DSM -> Protocol-Center)
#define SYSLOG_SERVER "192.168.178.36"
#define SYSLOG_PORT 514
#define DEVICE_HOSTNAME "my-device"
#define APP_NAME "smartsensor"

class smartsensor_syslog : public Syslog
{
    public:
        smartsensor_syslog();
        ~smartsensor_syslog();

        

    private:

        WiFiUDP syslogUDP;
};

#endif // SMARTSENSOR_SYSLOG_H