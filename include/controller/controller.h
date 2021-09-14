#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <ArduinoOTA.h>

#include <rws_wifi.h>
#include <rws_ntp.h>
#include <rws_syslog.h>
#include <rws_pubsubclient.h>
#include <rws_webupdate.h>
#include <statemachine.h>

#include <SPI.h>
#include <TimeAlarms.h>

#include "controller/settings/settings.h"
#include "controller/topics.h"

#include "common/mqtt_comm.h"

#include <rgbled.h>
#define ONBOARD_LED_RED   15
#define ONBOARD_LED_GREEN 12
#define ONBOARD_LED_BLUE  13

#include <pump.h>
#define PUMP_1_INPUT       5
#define PUMP_1_OUTPUT       14
#define PUMP_2_INPUT       4
#define PUMP_2_OUTPUT       16

class controller : public statemachine
{
    public:
        controller(rws_wifi *wifi, rws_ntp *ntp, rws_syslog *syslog, rws_pubsubclient *mqtt, rws_webupdate *webUpd, TimeAlarmsClass *timealarms);
        ~controller();

        void setup(void);
        void loop(void);

        bool _alarm_occurred;

    private:
        rws_wifi *_wifiMulti;
        rws_ntp *_ntp;
        rws_syslog *_syslog;
        rws_webupdate *_webUpdate;
        rws_pubsubclient *_mqtt;
        TimeAlarmsClass *_alarm;
        
        bool _src_barrel_present;
        bool _dst_barrel_present;
        
        bool _condition_lost;
        std::string _condition_lost_time;
        
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
        void setup_otaupdate(void);
        void setup_timealarms(void);

        bool check_all_conditions(void);

        void mqtt_callback(char* topic, uint8_t* payload, unsigned int length);

        rgbled *_light;
        pump   *_pump_1;
        pump   *_pump_2;

        void operating(void);

        void set_sens_mode(FunctionModes m);
        bool check_sens_mode(FunctionModes m);
        void print_stm_steps(void);
};


STEP_DEF(N000_INIT_STEP,                      "N000: Init step");    
STEP_DEF(N010_START_TIMEOUT_FOR_ACTIVATION,   "N010: Start timeout for activation");
STEP_DEF(N020_WAIT_TIMEOUT_FOR_ACTIVATION,    "N020: Wait timeout for activation");
STEP_DEF(N050_CHECK_START_PUMP,               "N050: Check start pump");
STEP_DEF(N060_WAIT_PUMP_STARTED,              "N060: Wait pump started");
STEP_DEF(N100_START_TIMEOUT_FOR_PUMPING,      "N100: Start timeout for pumping");
STEP_DEF(N110_WAIT_TIMEOUT_FOR_PUMPING,       "N110: Wait timeout for pumping");
STEP_DEF(N300_CHECK_STOP_PUMP,                "N300: Check pump stop");
STEP_DEF(N310_WAIT_PUMP_STOPPED,              "N310: Wait pump stopped");
STEP_DEF(N999_END,                            "N999: End");

#endif // CONTROLLER_H