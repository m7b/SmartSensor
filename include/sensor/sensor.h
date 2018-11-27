#ifndef SENSOR_H
#define SENSOR_H

//#include <Arduino.h>
#include <rws_wifi.h>
#include <rws_ntp.h>
#include <rws_syslog.h>
#include <rws_pubsubclient.h>
#include <statemachine.h>

#include "sensor/barrel.h"

#include "sensor/settings/settings.h"

#include "common/mqtt_comm.h"

enum stages
{
    PERMANENT_MEASSURE = 48, //'0'
    INTERVAL_MEASURE__5_SEK, //'1'
    INTERVAL_MEASURE_10_SEK, //'2'
    INTERVAL_MEASURE__5_MIN, //'3'
    DEEP_SLEEP_20_SEK,       //'4'
    DEEP_SLEEP__5_MIN        //'5'
};

class sensor : public statemachine
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
        
        unsigned long _start_time;
        unsigned long _ds_time;
    
        //Initial operation mode
        int OperationMode;

        void setup_serial(void);

        void setup_wifi(void);
        void setup_ntp(void);
        void setup_syslog(void);
        void setup_mqtt(void);

        bool check_all_conditions(void);

        void mqtt_callback(char* topic, uint8_t* payload, unsigned int length);
        
        void operating(int operation_mode);
};


STEP_DEF(N000_INIT_STEP,        "N000: Init step");    
STEP_DEF(N010_START_TIMEOUT,    "N010: Start timeout");
STEP_DEF(N020_START_MEASSURE,   "N020: Start meassure");
STEP_DEF(N030_REPORT_MEASSURE,  "N030: Report meassure");
STEP_DEF(N040_WAIT_TIMEOUT,     "N040: Wait timeout, depending on operation mode");
STEP_DEF(N050_START_TIMEOUT_DS, "N050: Start timeout for deep sleep");
STEP_DEF(N060_WAIT_TIMEOUT_DS,  "N060: Wait timeout for deep sleep");
STEP_DEF(N070_ENTER_DS,         "N070: Enter deep sleep");
STEP_DEF(N080_WAIT_DS,          "N080: Wait until deep sleep has performed, CPU stops working");

#endif // SENSOR_H