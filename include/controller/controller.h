#ifndef CONTROLLER_H
#define CONTROLLER_H

//#include <Arduino.h>
#include <rws_wifi.h>
#include <rws_ntp.h>
#include <rws_syslog.h>
#include <rws_pubsubclient.h>
#include <statemachine.h>

#include "common/settings/settings.h"
#include "controller/settings/settings.h"

#include <rgbled.h>
#define ONBOARD_LED_RED   15
#define ONBOARD_LED_GREEN 12
#define ONBOARD_LED_BLUE  13

class controller
{
    public:
        controller(rws_wifi *wifi, rws_ntp *ntp, rws_syslog *syslog, rws_pubsubclient *mqtt);
        ~controller();

        void setup(void);
        void loop(void);

    private:
        rws_wifi *_wifiMulti;
        rws_ntp *_ntp;
        rws_syslog *_syslog;
        rws_pubsubclient *_mqtt;

        statemachine sm;
    
        //Initial operation mode
        int OperationMode;

        void setup_serial(void);

        void setup_wifi(void);
        void setup_ntp(void);
        void setup_syslog(void);
        void setup_mqtt(void);

        bool check_all_conditions(void);

        void mqtt_callback(char* topic, uint8_t* payload, unsigned int length);

        rgbled *_light;

        void operating(void);
};

#endif // CONTROLLER_H