#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <ArduinoOTA.h>

#include <rws_wifi.h>
#include <rws_ntp.h>
#include <rws_syslog.h>
#include <rws_asyncmqttclient.h>
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

#include <OneButton.h>
#include <pump.h>
#define PUMP_1_INPUT       5
#define PUMP_1_OUTPUT     14
#define PUMP_2_INPUT       4
#define PUMP_2_OUTPUT     16

class controller : public statemachine
{
    public:
        controller(rws_wifi *wifi, rws_ntp *ntp, rws_syslog *syslog, rws_asyncmqttclient *mqtt, rws_webupdate *webUpd);
        ~controller();

        void setup(void);
        void loop(void);

    private:
        rws_wifi *_wifiMulti;
        rws_ntp *_ntp;
        rws_syslog *_syslog;
        rws_webupdate *_webUpdate;
        rws_asyncmqttclient *_mqtt;

        OneButton *_pump_1_button;
        OneButton *_pump_2_button;
        
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

        //MQTT callbacks
        void onMqttConnect(bool sessionPresent);
        void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
        void onMqttSubscribe(uint16_t packetId, uint8_t qos);
        void onMqttUnsubscribe(uint16_t packetId);
        void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
        void onMqttPublish(uint16_t packetId);

        //Pump callbacks
        void pump1_on_callback(void);
        void pump1_off_callback(void);
        void pump1_ls_on_callback(void);
        void pump1_ls_off_callback(void);
        void pump2_on_callback(void);
        void pump2_off_callback(void);
        void pump2_ls_on_callback(void);
        void pump2_ls_off_callback(void);

        //Button callbacks
        void pump1_btn_click(void);
        void pump2_btn_click(void);
        void pump1_btn_long_press_stop(void);
        void pump2_btn_long_press_stop(void);

        rgbled *_light;
        pump   *_pump_1;
        pump   *_pump_2;

        void operating(void);

        void print_stm_steps(void);

        unsigned long _start;
};


STEP_DEF(N000_INIT_STEP, "N000: Init step");
STEP_DEF(N010_WAIT_STEP, "N010: Wait timeout");
STEP_DEF(N999_END,       "N999: End");

#endif // CONTROLLER_H