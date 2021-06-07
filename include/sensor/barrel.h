#ifndef BARREL_H
#define BARREL_H

#include <NewPing.h>
#include "rws_common.h"
#include "rws_ntp.h"
#include "rws_pubsubclient.h"
#include "rws_syslog.h"

struct sFillLevel
{
    unsigned long sensor_cm;
    unsigned long cm;
    float percent;
    time_t utc;
    float litres;
    sFillLevel() : sensor_cm(0), cm(0), percent(.0), utc(0), litres(.0)
    {}
    void clear(void)
    {
        sensor_cm = 0;
        cm        = 0;
        percent   = .0;
        utc       = 0;
        litres    = .0;
    }
};


class barrel : public NewPing
{
    public:
        barrel(rws_ntp *ntp, rws_pubsubclient *mqtt, rws_syslog *syslog);
        ~barrel();

        bool do_measure(void);
        bool do_publish(void);

        sFillLevel get_fill_level(void);
        unsigned long get_fill_level_cm(void);
        float get_fill_level_percent(void);
        std::string get_local_datetime(void);

    private:
        unsigned long max_fill_level_cm;
        float _R;
        float _r;
        sFillLevel fill_level;

        rws_ntp *ntp;
        rws_pubsubclient *mqtt;
        rws_syslog *syslog;

        float calc_Volume(void);

        bool do_publish_mqtt(void);
        bool do_publish_syslog(void);
        bool do_publish_serial(void);
};

#endif // BARREL_H