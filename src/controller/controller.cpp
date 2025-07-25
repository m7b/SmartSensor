#include "controller/controller.h"

//NTP stuff; Called by time.h handler
extern time_t _MJBgetNtpTime(void);

controller::controller(ESP8266WiFiClass *wifi, rws_ntp *ntp, rws_syslog *syslog, rws_mqttclient *mqtt, rws_webupdate *webUpd, rws_influxdbclient *influx)
: statemachine(N000_INIT_STEP)
{
    _wifi        = wifi;
    _ntp         = ntp;
    _syslog      = syslog;
    _webUpdate   = webUpd;
    _mqtt        = mqtt;
    _influx      = influx;

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

    // Declare Data point InfluxDB
    _sensor = new Point("controller");

    // Cycle time meassurement
    _tick = 0;
    _tick2 = 0;
    _cyc_time = 0;
}

controller::~controller()
{
    delete _light;
    delete _pump_1;
    delete _pump_2;
    delete _pump_1_button;
    delete _pump_2_button;
    delete _sensor;
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
    Serial.println("--> setup() done");
    
}

void controller::loop(void)
{
    //update time from NTP on demand, see NTP_UPDATE_INTERVAL_MS
    _ntp->loop();

    //mqtt
    _mqtt->loop();

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

    Serial.println("--> setup_serial()");

#if DEBUG == ON
    // Take time for opening serial window
    delay(10000);
#endif
}

void controller::setup_wifi(void)
{
    Serial.println("--> setup_wifi()");

    wifiConnectHandler = _wifi->onStationModeGotIP(std::bind(&controller::onWifiConnect, this, std::placeholders::_1));
    
    _wifi->hostname(DEVICE_HOSTNAME);
    _wifi->mode(WIFI_STA);

    // Add WiFi connection credentials
    _wifi->begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.println("--> setup_wifi() done.");
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
    Serial.println("--> setup_syslog()");
    Serial.println("--> setup_syslog() done.");
}

void controller::setup_mqtt(void)
{
    _syslog->log(LOG_INFO, "--> Start setup_mqtt()");
    Serial.println("--> setup_mqtt()");

    //Set topics to subscribe
    _mqtt->subscribe("WS/RWS/#", [this](const char * topic, const char * payload) {
        Serial.printf("Received topic '%s': %s\n", topic, payload);

        if (strcmp(topic, "WS/RWS/Dashboard/ManualPumpReq") == 0) {
            _light->set_enable(payload[0] == '1');
            if (payload[0] == '1')
                _pump_1->set_on_demand();
            else
                _pump_1->set_off_demand();
        }

        if (strcmp(topic, "WS/RWS/Dashboard/ManualValveReq") == 0) {
            _light->set_enable(payload[0] == '1');
            if (payload[0] == '1')
                _pump_2->set_on_demand();
            else
                _pump_2->set_off_demand();
        }

    });

    _mqtt->begin();

    _syslog->log(LOG_INFO, "--> End setup_mqtt()");
    Serial.println("--> setup_mqtt() done.");
}


void controller::setup_webupdate(void)
{
    Serial.println("--> setup_webupdate()");
    _webUpdate->setup();
    Serial.println("--> setup_webupdate() done.");
}


void controller::setup_otaupdate(void)
{
    Serial.println("--> setup_otaupdate()");
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
    Serial.println("--> setup_otaupdate() done.");
}

bool controller::check_all_conditions(void)
{
    bool rc = true;
    
    rc = rc & WiFi.isConnected();
    /* Test if this is the cause of not working 
    rc = rc & _ntp->update(); */

    return rc;
}

void controller::operating(void)
{

    if (_tick == 0) {
        _tick = micros();
    }
    else {
        _tick2 = micros();
        _cyc_time = _tick2 - _tick;
        _tick = _tick2;
    }
    
    
    switch (get_step())
    {
        case N000_INIT_STEP:

            _start = millis();
            //_light->set_enable(true);
            set_next_step(N010_WAIT_STEP);
            //_syslog->log(LOG_INFO, N000_INIT_STEP.descr);
            break;

        case N010_WAIT_STEP:

            if (get_duration_ms(_start) >= 5000)
            {
                uptime::calculateUptime();

                std::string scyc_time    = std::to_string(_cyc_time);
                std::string uptime       = std::to_string(uptime::getDays()) + "d" + std::to_string(uptime::getHours()) + "h" + std::to_string(uptime::getMinutes()) + "m" + std::to_string(uptime::getSeconds()) + "s";
                uint32_t    ifree_heap   = ESP.getFreeHeap();
                uint8_t     uiCPUfreqMhz = ESP.getCpuFreqMHz();
                int8_t      iRSSI        = WiFi.RSSI();
                uint8_t     uiChannel    = WiFi.channel();
                std::string Channel      = std::to_string(uiChannel);
                std::string RSSI         = std::to_string(iRSSI);
                std::string free_heap    = std::to_string(ifree_heap);
                std::string CPUfreqMhz   = std::to_string(uiCPUfreqMhz);
                std::string mqtt_con     = std::to_string(_mqtt->connected());
                std::string influx_con   = std::to_string(_influx->validateConnection());
                int         ibrightness  = analogRead(A0);
                std::string brightness   = std::to_string(ibrightness);

                std::string msg = _ntp->get_local_datetime() + "; uptime: " + uptime + "; CPU freq MHz: " + CPUfreqMhz + "; Cyc Time: " + scyc_time + "; FreeHeap: " + free_heap + "; RSSI: " + RSSI + "; Channel: " + Channel + "; mqtt: " + mqtt_con + "; InfluxDB: " + influx_con;
                _mqtt->publish(STATUS, msg.c_str(), QOS0, RETAIN_ON);
                _mqtt->publish(VAL_AMBIENT_BRIGHTNESS, brightness.c_str(), QOS0, RETAIN_ON);
                _syslog->log(LOG_INFO, msg.c_str());
                Serial.println(msg.c_str());

                // Clear fields for reusing the point. Tags will remain the same as set above.
                _sensor->clearFields();

                // Store measured value into point
                // Report RSSI of currently connected network
                _sensor->addField("wifi_rssi",    iRSSI);
                _sensor->addField("wifi_channel", uiChannel);
                _sensor->addField("free_heap",    ifree_heap);
                _sensor->addField("cpu_freq_mhz", uiCPUfreqMhz);
                _sensor->addField("cyc_time",     _cyc_time);
                _sensor->addField("brightness",   ibrightness); //https://elektro.turanis.de/html/prj397/index.html#ExIIIEingebauterLDR

                // Print what are we exactly writing
                Serial.print("Writing: ");
                Serial.println(_sensor->toLineProtocol());

                //Write Point
                _influx->writePoint(*_sensor);

                set_next_step(N999_END);
            }
            break;

        case N999_END:
            //_syslog->log(LOG_INFO, N999_END.descr);
            if (uptime::getDays() >= 2) {
                ESP.restart();
                delay(1000);
            }

            set_next_step(N000_INIT_STEP);
            break;
    }
}

void controller::print_stm_steps(void)
{
    Serial.println("");
    Serial.println("Begin list of state machine:");
    print_step_info(N000_INIT_STEP);
    print_step_info(N010_WAIT_STEP);
    print_step_info(N999_END);
    Serial.println("End list of state machine.");
    Serial.println("");
}

void controller::pump1_on_callback(void)
{
    _mqtt->publish(MANUAL_PUMP_ACKNOWLEDGE, "1", QOS0, RETAIN_ON);
    _syslog->log(LOG_INFO, "Pump1 on_callback() triggered.");
}

void controller::pump1_off_callback(void)
{
    _mqtt->publish(MANUAL_PUMP_ACKNOWLEDGE, "0", QOS0, RETAIN_ON);

    //Reset Dashboard request
    _mqtt->publish("WS/RWS/Dashboard/ManualPumpReq", "0", QOS0, RETAIN_ON);
    
    _syslog->log(LOG_INFO, "Pump1 off_callback() triggered.");
}

void controller::pump2_on_callback(void)
{
    _mqtt->publish(MANUAL_VALVE_ACKNOWLEDGE, "1", QOS0, RETAIN_ON);
    _syslog->log(LOG_INFO, "Pump2 on_callback() triggered.");
}

void controller::pump2_off_callback(void)
{
    _mqtt->publish(MANUAL_VALVE_ACKNOWLEDGE, "0", QOS0, RETAIN_ON);

    //Reset Dashboard request
    _mqtt->publish("WS/RWS/Dashboard/ManualValveReq", "0", QOS0, RETAIN_ON);
    
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

void controller::onWifiConnect(const WiFiEventStationModeGotIP& event)
{
    Serial.println("--> onWifiConnect()");
    //_syslog->log(LOG_INFO, "--> onWifiConnect()");
    //setup_mqtt();
}

void controller::onWifiDisconnect(const WiFiEventStationModeDisconnected& event)
{
    Serial.println("--> onWifiDisconnect()");
}