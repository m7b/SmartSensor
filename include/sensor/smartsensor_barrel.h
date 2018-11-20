#ifndef SMARTSENSOR_BARREL_H
#define SMARTSENSOR_BARREL_H

#include <NewPing.h>
#include "rws_ntp.h"
#include "rws_pubsubclient.h"
#include "rws_syslog.h"

struct sFillLevel
{
    unsigned long sensor_cm;
    unsigned long cm;
    float percent;
    time_t utc;
    sFillLevel() : sensor_cm(0), cm(0), percent(.0), utc(0)
    {}
    void clear(void)
    {
        sensor_cm = 0;
        cm        = 0;
        percent   = .0;
        utc       = 0;
    }
};


class smartsensor_barrel : public NewPing
{
    public:
        smartsensor_barrel(rws_ntp *ntp, rws_pubsubclient *mqtt, rws_syslog *syslog);
        ~smartsensor_barrel();

        bool do_measure(void);
        bool do_publish(void);

        sFillLevel get_fill_level(void);
        unsigned long get_fill_level_cm(void);
        float get_fill_level_percent(void);
        std::string get_local_datetime(void);

    private:
        unsigned long max_fill_level_cm;
        sFillLevel fill_level;

        rws_ntp *ntp;
        rws_pubsubclient *mqtt;
        rws_syslog *syslog;

        bool do_publish_mqtt(void);
        bool do_publish_syslog(void);
        bool do_publish_serial(void);
};

#endif // SMARTSENSOR_BARREL_H