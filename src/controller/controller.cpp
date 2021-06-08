#include "controller/controller.h"

//Weekly Alarm stuff
extern void callbackPlain(void);

//NTP stuff; Called by time.h handler
extern time_t getNtpTime(void);

controller::controller(rws_wifi *wifi, rws_ntp *ntp, rws_syslog *syslog, rws_pubsubclient *mqtt, rws_webupdate *webUpd, ArduinoOTAClass *ota, TimeAlarmsClass *timealarms)
: statemachine(N000_INIT_STEP)
{
    _wifiMulti   = wifi;
    _ntp         = ntp;
    _syslog      = syslog;
    _webUpdate   = webUpd;
    _mqtt        = mqtt;
    _ota         = ota;
    _alarm       = timealarms;
    
    _sens_src_online = false;
    _sens_dst_online = false;

    _function_mode_src_req = 0;
    _function_mode_src_ack = 0;
    _function_mode_dst_req = 0;
    _function_mode_dst_ack = 0;

    _light = new rgbled(ONBOARD_LED_RED, ONBOARD_LED_GREEN, ONBOARD_LED_BLUE);
    _pump_1  = new pump(PUMP_1_OUTPUT, PUMP_1_INPUT, false, 30000);  //gießen 30sek
    _pump_2  = new pump(PUMP_2_OUTPUT, PUMP_2_INPUT, false, 300000); //hoch pumpen 5min

    _src_barrel_present = true;
    _dst_barrel_present = true;

    _alarm_occurred = false;
}

controller::~controller()
{
    delete _light;
    delete _pump_1;
    delete _pump_2;
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

    setup_timealarms();
    _pump_1->setup();
    _pump_2->setup();

    _light->set_delay_ms(333);

    print_stm_steps();
}

void controller::loop(void)
{
    //update time from NTP on demand, see NTP_UPDATE_INTERVAL_MS
    _ntp->loop();
    timeStatus();

    //check and renew WiFi connection
    _wifiMulti->check_connection();
    //MDNS.update();

    //check and renew MQTT connection
    _mqtt->check_connection();

    //Web-Update functionality
    _webUpdate->loop();

    //OTA update functionality
    _ota->handle();
    
    _pump_1->loop();
    _pump_2->loop();

    //check all conditions are ok
    if (!check_all_conditions())
        return;

    //keep mqtt alive
    _mqtt->loop();

    //test utc time
    //_ntp->test(3000);

    //control rgb led
    _light->loop();

    //Time Alarms stuff
    _alarm->delay(0);  //manage time callbacks of all alarms

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

    //MDNS.begin(DEVICE_HOSTNAME);

    // We start by connecting to a WiFi network
    _wifiMulti->check_connection();
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
    setSyncProvider(getNtpTime);
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
    _ota->onStart([&]() {
        String type;
        if (_ota->getCommand() == U_FLASH) {
            type = "sketch";
        } else { // U_FS
            type = "filesystem";
        }

        // NOTE: if updating FS this would be the place to unmount FS using FS.end()
        Serial.println("Start updating " + type);
    });

    _ota->onEnd([]() {
        Serial.println("\nEnd");
    });

    _ota->onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });

    _ota->onError([](ota_error_t error) {
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

    _ota->begin();
}


void controller::setup_timealarms(void)
{
//  _alarm->alarmRepeat(8,30,0, MorningAlarm);  // 8:30am every day
    _alarm->alarmRepeat(14,00,0, callbackPlain);  // 8:30am every day
    _alarm->alarmRepeat(14,02,0, callbackPlain);  // 8:32am every day
    _alarm->alarmRepeat(14,04,0, callbackPlain);  // 8:34am every day
}


bool controller::check_all_conditions(void)
{
    bool rc = true;
    
    rc = rc & _wifiMulti->connected();
//TODO    rc = rc & _mqtt->connected();
//This prevents using hardware buttons
//even no mqtt connetcion is available.
    rc = rc & _ntp->update();

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
                case 0:
                    Serial.printf("  - Function mode ack received from sensor_src: %d\r\n", payload[0]);
                    _function_mode_src_ack = payload[0];
                    break;

                case 1:
                    Serial.printf("  - Function mode ack received from sensor_dst: %d\r\n", payload[0]);
                    _function_mode_dst_ack = payload[0];
                    break;

                case 100:
                    Serial.printf("  - RAW_CM from sensor_src: %s\r\n", payload_to_string(payload, length).c_str());
                    break;

                case 101:
                    Serial.printf("  - CM from sensor_src: %s\r\n", payload_to_string(payload, length).c_str());
                    break;

                case 102:
                    Serial.printf("  - PERCENT from sensor_src: %s\r\n", payload_to_string(payload, length).c_str());
                    break;

                case 103:
                    Serial.printf("  - TIMESTAMP from sensor_src: %s\r\n", payload_to_string(payload, length).c_str());
                    break;

                case 104:
                    Serial.printf("  - LOC_TIMESTAMP from sensor_src: %s\r\n", payload_to_string(payload, length).c_str());
                    break;

                case 105:
                    Serial.printf("  - SENS_LAST_WILL_TOPIC (Status) from sensor_src: %s\r\n", payload_to_string(payload, length).c_str());
                    _sens_src_online = (payload_to_string(payload, length).compare("Online") == 0);
                    break;

                case 200:
                    Serial.printf("  - RAW_CM from sensor_dst: %s\r\n", payload_to_string(payload, length).c_str());
                    break;

                case 201:
                    Serial.printf("  - CM from sensor_dst: %s\r\n", payload_to_string(payload, length).c_str());
                    break;

                case 202:
                    Serial.printf("  - PERCENT from sensor_dst: %s\r\n", payload_to_string(payload, length).c_str());
                    break;

                case 203:
                    Serial.printf("  - TIMESTAMP from sensor_dst: %s\r\n", payload_to_string(payload, length).c_str());
                    break;

                case 204:
                    Serial.printf("  - LOC_TIMESTAMP from sensor_dst: %s\r\n", payload_to_string(payload, length).c_str());
                    break;

                case 205:
                    Serial.printf("  - SENS_LAST_WILL_TOPIC (Status) from sensor_dst: %s\r\n", payload_to_string(payload, length).c_str());
                    _sens_dst_online = (payload_to_string(payload, length).compare("Online") == 0);
                    break;

                case 500:
                    Serial.printf("  - MANUAL_PUMP_REQ from dashboard: %s\r\n", payload_to_string(payload, length).c_str());
                    _light->set_enable(payload[0] == '1');
                    if (payload[0] == '1')
                        _pump_1->set_on_demand();
                    else
                        _pump_1->set_off_demand();
                        
                    _mqtt->publish(MANUAL_PUMP_ACKNOWLEDGE, payload_to_string(payload, length));
                    break;

                case 501:
                    Serial.printf("  - MANUAL_VALVE_REQ from dashboard: %s\r\n", payload_to_string(payload, length).c_str());
                    if (payload[0] == '1')
                        _pump_2->set_on_demand();
                    else
                        _pump_2->set_off_demand();
                    _mqtt->publish(MANUAL_VALVE_ACKNOWLEDGE, payload_to_string(payload, length));
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
            if (_alarm_occurred)
            {
                set_next_step(N010_START_TIMEOUT_FOR_ACTIVATION);
            }
            break;

        case N010_START_TIMEOUT_FOR_ACTIVATION:
            _start_time = millis();
            set_next_step(N020_WAIT_TIMEOUT_FOR_ACTIVATION);
            break;

        case N020_WAIT_TIMEOUT_FOR_ACTIVATION:
            if (get_duration_ms(_start_time) >= 2500)
            {
                //Test analog input
                _mqtt->publish(VAL_AMBIENT_BRIGHTNESS, analogRead(A0));

                set_next_step(N050_CHECK_START_PUMP);
            }
            break;

        case N050_CHECK_START_PUMP:
            if (true)
                set_next_step(N060_WAIT_PUMP_STARTED);
            break;
            
        case N060_WAIT_PUMP_STARTED:
            if (true)
                set_next_step(N100_START_TIMEOUT_FOR_PUMPING);
            break;
            
        case N100_START_TIMEOUT_FOR_PUMPING:
            _start_time = millis();
            set_next_step(N110_WAIT_TIMEOUT_FOR_PUMPING);
            break;
            
        case N110_WAIT_TIMEOUT_FOR_PUMPING:
            if (get_duration_ms(_start_time) >= 2500)
            {
                //Test analog input
                _mqtt->publish(VAL_AMBIENT_BRIGHTNESS, analogRead(A0));

                set_next_step(N300_CHECK_STOP_PUMP);
            }
            break;
            
        case N300_CHECK_STOP_PUMP:
            if (true)
                set_next_step(N310_WAIT_PUMP_STOPPED);
            break;
            
        case N310_WAIT_PUMP_STOPPED:
            if (true)
                set_next_step(N999_END);
            break;

        case N999_END:
            _alarm_occurred = false;
            set_next_step(N000_INIT_STEP);
            break;
    }
}



void controller::set_sens_mode(FunctionModes m)
{
    if (_src_barrel_present)
    {
        _function_mode_src_req = m;
        _mqtt->publish(FUNCTION_MODE_SRC_REQUEST, _function_mode_src_req);
    }
    if (_dst_barrel_present)
    {
        _function_mode_dst_req = m;
        _mqtt->publish(FUNCTION_MODE_DST_REQUEST, _function_mode_dst_req);
    }
}


bool controller::check_sens_mode(FunctionModes m)
{
    bool rc = true;

    if (_src_barrel_present)
        rc = rc && (_function_mode_src_ack == m);

    if (_dst_barrel_present)
        rc = rc && (_function_mode_dst_ack == m);

    //Could also check if (_function_mode_src/dst_ack == _function_mode_src/dst_req)

    return rc;
}

void controller::print_stm_steps(void)
{
    Serial.println("");
    print_step_info(N000_INIT_STEP);
    print_step_info(N010_START_TIMEOUT_FOR_ACTIVATION);
    print_step_info(N020_WAIT_TIMEOUT_FOR_ACTIVATION);
    print_step_info(N050_CHECK_START_PUMP);
    print_step_info(N060_WAIT_PUMP_STARTED);
    print_step_info(N100_START_TIMEOUT_FOR_PUMPING);
    print_step_info(N110_WAIT_TIMEOUT_FOR_PUMPING);
    print_step_info(N300_CHECK_STOP_PUMP);
    print_step_info(N310_WAIT_PUMP_STOPPED);
    print_step_info(N999_END);
    Serial.println("");
}