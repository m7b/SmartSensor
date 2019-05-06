#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <rws_wifi.h>
#include <rws_ntp.h>
#include <rws_syslog.h>
#include <rws_pubsubclient.h>
#include <rws_webupdate.h>
#include <statemachine.h>

#include "controller/settings/settings.h"
#include "controller/topics.h"

#include "common/mqtt_comm.h"

#include <rgbled.h>
#define ONBOARD_LED_RED   15
#define ONBOARD_LED_GREEN 12
#define ONBOARD_LED_BLUE  13

class controller : public statemachine
{
    public:
        controller(rws_wifi *wifi, rws_ntp *ntp, rws_syslog *syslog, rws_pubsubclient *mqtt, rws_webupdate *webUpd);
        ~controller();

        void setup(void);
        void loop(void);

    private:
        rws_wifi *_wifiMulti;
        rws_ntp *_ntp;
        rws_syslog *_syslog;
        rws_webupdate *_webUpdate;
        rws_pubsubclient *_mqtt;
        
        bool _src_barrel_present;
        bool _dst_barrel_present;
        
        unsigned long _start_time;

        bool _sens_src_online;
        bool _sens_dst_online;

        uint8_t _function_mode_src_req;
        uint8_t _function_mode_src_ack;
        uint8_t _function_mode_dst_req;
        uint8_t _function_mode_dst_ack;

        void setup_serial(void);

        void setup_wifi(void);
        void setup_ntp(void);
        void setup_syslog(void);
        void setup_mqtt(void);
        void setup_webupdate(void);

        bool check_all_conditions(void);

        void mqtt_callback(char* topic, uint8_t* payload, unsigned int length);

        rgbled *_light;

        void operating(void);

        void set_sens_mode(FunctionModes m);
        bool check_sens_mode(FunctionModes m);
        void print_stm_steps(void);
};


STEP_DEF(N000_INIT_STEP,                      "N000: Init step");    
STEP_DEF(N001_START_TIMEOUT_FOR_ACTIVATION,   "N001: Start timeout for activation");
STEP_DEF(N002_WAIT_TIMEOUT_FOR_ACTIVATION,    "N002: Wait timeout for activation");
STEP_DEF(N010_CHECK_PUMP_NOT_ACTIVE,          "N010: Check pump not active");
STEP_DEF(N020_CHECK_DST_LEVEL_BELOW_MAX,      "N020: Check destination level below max");
STEP_DEF(N030_CHECK_SRC_LEVEL_OVER_MIN,       "N030: Check source level over min");
STEP_DEF(N035_CHANGE_MEAS_MODE_SENSORS,       "N035: Change meassuring mode of sensors");
STEP_DEF(N036_WAIT_MEAS_MODE_SENSORS_CHANGED, "N036: Wait meassuring mode of sensors changed");
STEP_DEF(N040_CHECK_PUMP_READY,               "N040: Check pump ready");
STEP_DEF(N050_CHECK_START_PUMP,               "N050: Check start pump");
STEP_DEF(N060_WAIT_PUMP_STARTED,              "N060: Wait pump started");
STEP_DEF(N070_STORE_START_TIME,               "N070: Store start time");
STEP_DEF(N080_CHECK_ONLY_START_PUMP,          "N080: Check only start pump");
STEP_DEF(N090_CHECK_SRC_LEVEL_DECREASING,     "N090: Check source level decreasing");
STEP_DEF(N100_CHECK_DST_LEVEL_INCREASING,     "N100: Check destination level increasing");
STEP_DEF(N110_CHECK_SRC_LEVEL_OVER_MIN,       "N110: Check source level over min");
STEP_DEF(N120_CHECK_DST_LEVEL_BELOW_MAX,      "N120: Check destination level below max");
STEP_DEF(N130_CHECK_NO_STOP_DEMAND_FROM_USER, "N130: Check no stop demand from user");
STEP_DEF(N140_CHECK_PUMP_DURATION_BELOW_MAX,  "N140: Check pump duration below max");
STEP_DEF(N200_REPORT_ERROR,                   "N200: Report error");
STEP_DEF(N300_CHECK_STOP_PUMP,                "N300: Check pump stop");
STEP_DEF(N310_CHECK_PUMP_STOPPED,             "N310: Check pump stopped");
STEP_DEF(N320_STORE_PUMP_DATA,                "N320: Store pump data");
STEP_DEF(N400_CHANGE_MEAS_MODE_SENSORS,       "N400: Change meassuring mode of sensors");
STEP_DEF(N410_WAIT_MEAS_MODE_SENSORS_CHANGED, "N410: Wait meassuring mode of sensors changed");
STEP_DEF(N999_END,                            "N999: End");

#endif // CONTROLLER_H