#include "controller/controller.h"

//NTP stuff; Called by time.h handler
extern time_t _MJBgetNtpTime(void);

controller::controller(rws_wifi *wifi, rws_ntp *ntp, rws_syslog *syslog, rws_pubsubclient *mqtt, rws_webupdate *webUpd)
: statemachine(N000_INIT_STEP)
{
    _wifiMulti   = wifi;
    _ntp         = ntp;
    _syslog      = syslog;
    _webUpdate   = webUpd;
    _mqtt        = mqtt;

    _function_mode_src_req = 0;
    _function_mode_src_ack = 0;
    _function_mode_dst_req = 0;
    _function_mode_dst_ack = 0;

    _light = new rgbled(ONBOARD_LED_RED, ONBOARD_LED_GREEN, ONBOARD_LED_BLUE);
    _pump_1  = new pump(PUMP_1_OUTPUT, false,  30000);  //gießen 30sek
    _pump_2  = new pump(PUMP_2_OUTPUT, false, 300000); //hoch pumpen 5min

    _pump_1_button = new OneButton(PUMP_1_INPUT, true, true);
    _pump_2_button = new OneButton(PUMP_2_INPUT, true, true);

    _condition_lost = false;
    _condition_lost_time = "";
}

controller::~controller()
{
    delete _light;
    delete _pump_1;
    delete _pump_2;
    delete _pump_1_button;
    delete _pump_2_button;
}

void controller::setup(void)
{
    setup_serial();
    setup_wifi();
    setup_ntp();
    setup_syslog();
    setup_mqtt();
    setup_webupdate();
    setup_otaupdate();

    _pump_1->setup();
    _pump_1->setCallback_on([this] (void) { this->pump1_on_callback(); });
    _pump_1->setCallback_off([this] (void) { this->pump1_off_callback(); });
    _pump_1->setCallback_ls_on([this] (void) { this->pump1_ls_on_callback(); });
    _pump_1->setCallback_ls_off([this] (void) { this->pump1_ls_off_callback(); });

    _pump_2->setup();
    _pump_2->setCallback_on([this] (void) { this->pump2_on_callback(); });
    _pump_2->setCallback_off([this] (void) { this->pump2_off_callback(); });
    _pump_2->setCallback_ls_on([this] (void) { this->pump2_ls_on_callback(); });
    _pump_2->setCallback_ls_off([this] (void) { this->pump2_ls_off_callback(); });

    _pump_1_button->attachClick([](void *scope) { ((controller *) scope)->pump1_btn_click();}, this);
    _pump_2_button->attachClick([](void *scope) { ((controller *) scope)->pump2_btn_click();}, this);

    _pump_1_button->attachLongPressStop([](void *scope) { ((controller *) scope)->pump1_btn_long_press_stop();}, this);
    _pump_2_button->attachLongPressStop([](void *scope) { ((controller *) scope)->pump2_btn_long_press_stop();}, this);

    _light->set_delay_ms(333);

    print_stm_steps();
    
    std::string msg = "Setup done. Running version " + std::string(FIRMWARE_VERSION_DATE_TIME) + ". Begin loop() ... ";
    _syslog->log(LOG_INFO, msg.c_str());
}

void controller::loop(void)
{
    //check and renew WiFi connection
    _wifiMulti->check_connection();
    //MDNS.update();
    
    //update time from NTP on demand, see NTP_UPDATE_INTERVAL_MS
    _ntp->loop();
    timeStatus();

    //check and renew MQTT connection
    _mqtt->check_connection();

    //Web-Update functionality
    _webUpdate->loop();

    //OTA update functionality
    ArduinoOTA.handle();

    //Pumps
    _pump_1->loop();
    _pump_2->loop();

    //Buttons
    _pump_1_button->tick();
    _pump_2_button->tick();

    //check all conditions are ok
    if (!check_all_conditions())
    {
        if (!_condition_lost)
        {
            _condition_lost = true;
            _condition_lost_time = _ntp->get_local_datetime();
            
            char buf[10];
            snprintf(buf, 10, "%d", WiFi.status());
            _wlan_status = std::string(buf);
        }
        return;
    }
    if (_condition_lost)
    {
        _condition_lost = false;
        std::string msg = "In loop(): Check all conditions failed at " + _condition_lost_time + " (" + _wlan_status + ")";
        _syslog->log(LOG_INFO, msg.c_str());
    }

    //keep mqtt alive
    _mqtt->loop();

    //control rgb led
    _light->loop();

    //operation
    operating();
}



///////////////////////////////////////////////////////////////////////////////
// PRIVATE
///////////////////////////////////////////////////////////////////////////////
void controller::setup_serial(void)
{
    Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);

#if DEBUG == ON
    // Take time for opening serial window
    delay(10000);
#endif
}

void controller::setup_wifi(void)
{
    // Add WiFi connection credentials
    for(auto entry : wifi_access_credentials) 
        _wifiMulti->addAP(entry.first, entry.second);

    WiFi.hostname(DEVICE_HOSTNAME);
    WiFi.mode(WIFI_STA);

    // We start by connecting to a WiFi network
    _wifiMulti->check_connection();
    
    //MDNS.begin(DEVICE_HOSTNAME);
}

void controller::setup_ntp(void)
{
    Serial.println("We start getting time from NTP ...");
    // We start getting time from ntp
    _ntp->setup();
    
    // Wait until time retrieved
    while (!_ntp->update())
        Serial.printf(".");
    
    Serial.printf("\r\nNTP connected\r\n");

    // Setup handler for syncing system time with local time 
    Serial.println("Setup handler for syncing system time with local time");
    setSyncProvider(_MJBgetNtpTime);
    setSyncInterval(60);
}

void controller::setup_syslog(void)
{
    //Syslog already usable
}

void controller::setup_mqtt(void)
{
    //Set topics to subscribe
    _mqtt->set_topics_to_subscribe(&topics_to_subscribe);

    //Set callback function
    _mqtt->setCallback([this] (char* topic, uint8_t* payload, unsigned int length) { this->mqtt_callback(topic, payload, length); });

    // We start by connecting to MQTT server
    _mqtt->check_connection();
}


void controller::setup_webupdate(void)
{
    _webUpdate->setup();
}


void controller::setup_otaupdate(void)
{
    ArduinoOTA.setHostname(DEVICE_HOSTNAME);

    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "sketch";
        } else { // U_FS
            type = "filesystem";
        }

        // NOTE: if updating FS this would be the place to unmount FS using FS.end()
        Serial.println("Start updating " + type);
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            Serial.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
            Serial.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
            Serial.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            Serial.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
            Serial.println("End Failed");
        }
    });

    ArduinoOTA.begin();
}

bool controller::check_all_conditions(void)
{
    bool rc = true;
    
    rc = rc & _wifiMulti->connected();
    /* Test if this is the cause of not working 
    rc = rc & _ntp->update(); */

    return rc;
}

void controller::mqtt_callback(char* topic, uint8_t* payload, unsigned int length) {
    Serial.print("  - Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    Serial.println();

    for(auto el: topics_to_subscribe)
    {
        if (strcmp(topic, std::get<TP_TOP>(el)) == 0)
        {
            switch(std::get<TP_NUM>(el))
            {
                case 500:
                    Serial.printf("  - MANUAL_PUMP_REQ from dashboard: %s\r\n", payload_to_string(payload, length).c_str());
                    _light->set_enable(payload[0] == '1');
                    if (payload[0] == '1')
                        _pump_1->set_on_demand();
                    else
                        _pump_1->set_off_demand();
                        
                    //_mqtt->publish(MANUAL_PUMP_ACKNOWLEDGE, payload_to_string(payload, length));
                    break;

                case 501:
                    Serial.printf("  - MANUAL_VALVE_REQ from dashboard: %s\r\n", payload_to_string(payload, length).c_str());
                    _light->set_enable(payload[0] == '1');
                    if (payload[0] == '1')
                        _pump_2->set_on_demand();
                    else
                        _pump_2->set_off_demand();
                    
                    //_mqtt->publish(MANUAL_VALVE_ACKNOWLEDGE, payload_to_string(payload, length));
                    break;
            }
        }
    }
}

void controller::operating(void)
{
    switch (get_step())
    {
        case N000_INIT_STEP:

            _start = millis();
            //_light->set_enable(true);
            set_next_step(N010_WAIT_STEP);
            //_syslog->log(LOG_INFO, N000_INIT_STEP.descr);
            break;

        case N010_WAIT_STEP:

            if (get_duration_ms(_start) >= 1000)
            {
                _mqtt->publish(VAL_LIFE_SIGN, "Life sign! " + _ntp->get_local_datetime());
                std::string msg = "Life sign! " + _ntp->get_local_datetime();
                _syslog->log(LOG_INFO, msg.c_str());
                set_next_step(N999_END);
            }
            break;

        case N999_END:
            //_syslog->log(LOG_INFO, N999_END.descr);
            set_next_step(N000_INIT_STEP);
            break;
    }
}

void controller::print_stm_steps(void)
{
    Serial.println("");
    print_step_info(N000_INIT_STEP);
    print_step_info(N010_WAIT_STEP);
    print_step_info(N999_END);
    Serial.println("");
}

void controller::pump1_on_callback(void)
{
    _mqtt->publish(MANUAL_PUMP_ACKNOWLEDGE, "1", true);
    _syslog->log(LOG_INFO, "Pump1 on_callback() triggered.");
}

void controller::pump1_off_callback(void)
{
    _mqtt->publish(MANUAL_PUMP_ACKNOWLEDGE, "0", true);

    //Reset Dashboard request
    _mqtt->publish("WS/RWS/Dashboard/ManualPumpReq", "0", true);
    
    _syslog->log(LOG_INFO, "Pump1 off_callback() triggered.");
}

void controller::pump2_on_callback(void)
{
    _mqtt->publish(MANUAL_VALVE_ACKNOWLEDGE, "1", true);
    _syslog->log(LOG_INFO, "Pump2 on_callback() triggered.");
}

void controller::pump2_off_callback(void)
{
    _mqtt->publish(MANUAL_VALVE_ACKNOWLEDGE, "0", true);

    //Reset Dashboard request
    _mqtt->publish("WS/RWS/Dashboard/ManualValveReq", "0", true);
    
    _syslog->log(LOG_INFO, "Pump2 off_callback() triggered.");
}

void controller::pump1_btn_click(void)
{
    if (_pump_1->is_on())
        _pump_1->set_off_demand();
        
    if (_pump_1->is_off())
        _pump_1->set_on_demand();
}

void controller::pump2_btn_click(void)
{
    if (_pump_2->is_on())
        _pump_2->set_off_demand();
        
    if (_pump_2->is_off())
        _pump_2->set_on_demand();
}

void controller::pump1_btn_long_press_stop(void)
{
    _pump_1->set_ls_on_demand();
}

void controller::pump2_btn_long_press_stop(void)
{
    _pump_2->set_ls_on_demand();
}

void controller::pump1_ls_on_callback(void)
{
    _syslog->log(LOG_INFO, "Pump1 ls_on_callback() triggered.");
}

void controller::pump1_ls_off_callback(void)
{
    _syslog->log(LOG_INFO, "Pump1 ls_off_callback() triggered.");
}

void controller::pump2_ls_on_callback(void)
{
    _syslog->log(LOG_INFO, "Pump2 ls_on_callback() triggered.");
}

void controller::pump2_ls_off_callback(void)
{
    _syslog->log(LOG_INFO, "Pump2 ls_off_callback() triggered.");
}