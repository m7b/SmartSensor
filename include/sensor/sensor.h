#ifndef SENSOR_H
#define SENSOR_H

//#include <Arduino.h>
#include <rws_wifi.h>
#include <rws_ntp.h>
#include <rws_syslog.h>
#include <rws_pubsubclient.h>

#include "sensor/barrel.h"
#include "sensor/statemachine.h"

#include "sensor/settings/settings.h"

class sensor
{
    public:
        sensor(rws_wifi *wifi, rws_ntp *ntp, rws_syslog *syslog, rws_pubsubclient *mqtt);
        ~sensor();

        void setup(void);
        void loop(void);

    private:
        rws_wifi *_wifiMulti;
        rws_ntp *_ntp;
        rws_syslog *_syslog;
        rws_pubsubclient *_mqtt;

        barrel *_barrel;
        statemachine *_sm;
    
        //Initial operation mode
        int OperationMode;

        void setup_serial(void);

        void setup_wifi(void);
        void setup_ntp(void);
        void setup_syslog(void);
        void setup_mqtt(void);

        bool check_all_conditions(void);

        void mqtt_callback(char* topic, uint8_t* payload, unsigned int length);
};

#endif // SENSOR_H