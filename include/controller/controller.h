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
        
        bool _condition_lost;
        std::string _condition_lost_time;
        std::string _wlan_status;

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

        bool check_all_conditions(void);

        void mqtt_callback(char* topic, uint8_t* payload, unsigned int length);

        rgbled *_light;
        pump   *_pump_1;
        pump   *_pump_2;

        void operating(void);

        void print_stm_steps(void);

        unsigned long _start;
        uint16_t _step;
};


STEP_DEF(N000_INIT_STEP, "N000: Init step");
STEP_DEF(N010_WAIT_STEP, "N010: Wait timeout");
STEP_DEF(N999_END,       "N999: End");

#endif // CONTROLLER_H