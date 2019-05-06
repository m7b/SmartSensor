#include "sensor/sensor.h"

//Web-Updater things---------------------
#if defined(SRC_BARREL)
    const char* dns_host_barrel = "rws-src-barrel";
#elif defined(DST_BARREL)
    const char* dns_host_barrel = "rws-dst-barrel";
#endif
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
//Web-Updater things---------------------

sensor::sensor(rws_wifi *wifi, rws_ntp *ntp, rws_syslog *syslog, rws_pubsubclient *mqtt)
: statemachine(N000_INIT_STEP)
{
    _wifiMulti = wifi;
    _ntp       = ntp;
    _syslog    = syslog;
    _mqtt      = mqtt;
    
    _barrel = new barrel(_ntp, _mqtt, _syslog);

    _mqtt_online = true;

    //Initial operation mode
    FunctionModeRequest = FUNCTION_MODE_INTERVAL_MEASURE__5_SEK;
    FunctionModeAck     = FUNCTION_MODE_INTERVAL_MEASURE__5_SEK;
    FunctionMode        = FUNCTION_MODE_INTERVAL_MEASURE__5_SEK;
}

sensor::~sensor()
{
    delete _barrel;
}

void sensor::setup(void)
{
    setup_serial();
    setup_wifi();
    setup_ntp();
    setup_syslog();
    setup_mqtt();

    print_stm_steps();

//Web-Updater things---------------------
    MDNS.begin(dns_host_barrel);

    httpServer.on("/", []() {
        httpServer.send(200, "text/plain", "Hi! I am a Sensor.");
    });

    httpUpdater.setup(&httpServer);
    httpServer.begin();

    MDNS.addService("http", "tcp", 80);
    Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\r\n", dns_host_barrel);
//Web-Updater things---------------------


    


}

void sensor::loop(void)
{
    //update time from NTP on demand, see NTP_UPDATE_INTERVAL_MS
    _ntp->update();

    //check and renew WiFi connection
    _wifiMulti->check_connection();

//Web-Updater things---------------------
    httpServer.handleClient();
    MDNS.update();
//Web-Updater things---------------------

    //check and renew MQTT connection
    if (_mqtt_online)
        _mqtt->check_connection();

    //check all conditions are ok
    if (!check_all_conditions())
    {
        return;
    }

    //operation
    operating();
}



///////////////////////////////////////////////////////////////////////////////
// PRIVATE
///////////////////////////////////////////////////////////////////////////////
void sensor::setup_serial(void)
{
    Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);

#if DEBUG == ON
    // Take time for opening serial window
    delay(10000);
#endif
}

void sensor::setup_wifi(void)
{
    _wifiMulti->set_on_con_timeout_fct([this] (void) {
        //Things to do after wlanconnection timeout occured
        action_wlan_con_timeout();
        });

    // Add WiFi connection credentials
    for(auto entry : wifi_access_credentials) 
        _wifiMulti->addAP(entry.first, entry.second);

    WiFi.mode(WIFI_STA);

    // We start by connecting to a WiFi network
    _wifiMulti->check_connection();
}

void sensor::setup_ntp(void)
{
    // We start getting time from ntp
    _ntp->begin();
}

void sensor::setup_syslog(void)
{
    //Syslog already usable
}

void sensor::setup_mqtt(void)
{
    //Set topics to subscribe
    _mqtt->set_topics_to_subscribe(&topics_to_subscribe);

    //Set callback function
    _mqtt->setCallback([this] (char* topic, uint8_t* payload, unsigned int length) {
        this->mqtt_callback(topic, payload, length);
        });

    //Set on connection pub function
    _mqtt->set_on_con_fct([this] (void) {
        //Things to do after connection is established
        set_next_step(N000_INIT_STEP);
        //Publish actual function mode acknowledge
        _mqtt->publish(FUNCTION_MODE_ACK, FunctionModeAck);
        });

    //Set on connection fail function
    _mqtt->set_on_con_failed_fct([this] (void) {
        //Things to do after mqtt broker connection failed
        action_mqtt_con_timeout();
        });
}


bool sensor::check_all_conditions(void)
{
    bool rc = true;
    
    rc = rc & _wifiMulti->connected();

    if (_mqtt_online)
        rc = rc & _mqtt->connected();

    rc = rc & _ntp->update();

    return rc;
}

void sensor::mqtt_callback(char* topic, uint8_t* payload, unsigned int length) {
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
                    Serial.printf("  - Function mode request received: %d\r\n", payload[0]);
                    FunctionModeRequest = payload[0];
                    break;

                case 1:
                    Serial.printf("  - inTopic received: %s\r\n", payload_to_string(payload, length).c_str());
                    break;
            }
        }
    }
}

void sensor::operating(void)
{
    switch (get_step())
    {
        case N000_INIT_STEP:
            //test utc time
            _ntp->test();

            {
                IPAddress ipaddr;
                const char *host_to_resolv = "bierfass.fritz.box";
                int rc = WiFi.hostByName(host_to_resolv, ipaddr);
                if ( rc == 1) {
                    Serial.printf("IP of %s is %s\r\n", host_to_resolv, ipaddr.toString().c_str());
                }
                else
                {
                    Serial.printf("IP of %s not retrieved: err code %d\r\n", host_to_resolv, rc);
                }
            }

            set_next_step(N010_START_TIMEOUT);
            break;

        case N010_START_TIMEOUT:
            _start_time = millis();
            set_next_step(N020_START_MEASSURE);
            break;

        case N020_START_MEASSURE:
            _barrel->do_measure();
            set_next_step(N030_REPORT_MEASSURE);
            break;

        case N030_REPORT_MEASSURE:
            _barrel->do_publish();
            set_next_step(N040_CHECK_FUNCTION_MODE_CHANGE_REQ);
            break;

        case N040_CHECK_FUNCTION_MODE_CHANGE_REQ:
            if (FunctionMode != FunctionModeRequest)
                set_next_step(N050_CHANGE_TO_REQ_FUNCTION_MODE);
            else
                set_next_step(N070_WAIT_TIMEOUT);
                
            break;

        case N050_CHANGE_TO_REQ_FUNCTION_MODE:
            FunctionMode = FunctionModeRequest;
            set_next_step(N060_ACK_NEW_FUNCTION_MODE);
            break;

        case N060_ACK_NEW_FUNCTION_MODE:
            FunctionModeAck = FunctionMode;
            _mqtt->publish(FUNCTION_MODE_ACK, FunctionModeAck);
            set_next_step(N070_WAIT_TIMEOUT);
            break;

        case N070_WAIT_TIMEOUT: {
                unsigned long duration = 0;

                switch (FunctionMode)
                {
                    case FUNCTION_MODE_PERMANENT_MEASSURE:      duration =    200; break;
                    case FUNCTION_MODE_INTERVAL_MEASURE__5_SEK: duration =   5000; break;
                    case FUNCTION_MODE_INTERVAL_MEASURE_10_SEK: duration =  10000; break;
                    case FUNCTION_MODE_INTERVAL_MEASURE__5_MIN: duration = 300000; break;
                    case FUNCTION_MODE_DEEP_SLEEP_20_SEK:       _ds_time = ds_time_20sec; break;
                    case FUNCTION_MODE_DEEP_SLEEP__5_MIN:       _ds_time = ds_time__5min; break;
                    default:                                    duration =   3000; break;
                }

                //in case of deep sleep, perform special step
                if ((FunctionMode == FUNCTION_MODE_DEEP_SLEEP_20_SEK) || (FunctionMode == FUNCTION_MODE_DEEP_SLEEP__5_MIN))
                {
                    set_next_step(N080_PUBLISH_SENSOR_OFFLINE);
                }
                //else - wait timeout depending on operation mode
                else if (get_duration_ms(_start_time) >= duration)
                {
                    set_next_step(N000_INIT_STEP);
                }
                
                //Check function mode change request
                if (FunctionMode != FunctionModeRequest)
                    set_next_step(N050_CHANGE_TO_REQ_FUNCTION_MODE);
            }
            break;

        case N080_PUBLISH_SENSOR_OFFLINE:
            mqtt_offline_demand();
            set_next_step(N090_START_TIMEOUT_DS);
            break;

        case N090_START_TIMEOUT_DS:
            _start_time = millis();
            set_next_step(N100_WAIT_TIMEOUT_DS);
            break;

        case N100_WAIT_TIMEOUT_DS:
            if (get_duration_ms(_start_time) >= 200)
            {
                set_next_step(N110_ENTER_DS);
            }

            break;

        case N110_ENTER_DS:
            do_deep_sleep(_ds_time);

            //Stop program until controller is switched off. 10 sec should be enaugh.
            delay(10000);
    }
}


void sensor::mqtt_offline_demand(void)
{
    //Publish offline - guided because of Deep-Sleep
    _mqtt->publish(LAST_WILL_TOPIC, LAST_WILL_MESSAGE " - guided because of Deep-Sleep", LAST_WILL_RETAIN);

    //Prevent reconnect in loop() after guided disconnection
    _mqtt_online = false;

    //disconnect from broker
    _mqtt->disconnect();
}


void sensor::do_deep_sleep(unsigned long ds_time)
{
    Serial.printf("  - Going to Deep-Sleep for %.2f seconds ...\r\n", ds_time/1000000.0);
    ESP.deepSleep(ds_time);

    //Stop program until controller is switched off. 10 sec should be enaugh.
    delay(10000);
}


void sensor::action_wlan_con_timeout(void)
{
    do_deep_sleep(ds_time__5min);
}


void sensor::action_mqtt_con_timeout(void)
{
    do_deep_sleep(ds_time__5min);
}

void sensor::print_stm_steps(void)
{
    Serial.println("");
    print_step_info(N000_INIT_STEP);
    print_step_info(N010_START_TIMEOUT);
    print_step_info(N020_START_MEASSURE);
    print_step_info(N030_REPORT_MEASSURE);
    print_step_info(N040_CHECK_FUNCTION_MODE_CHANGE_REQ);
    print_step_info(N050_CHANGE_TO_REQ_FUNCTION_MODE);
    print_step_info(N060_ACK_NEW_FUNCTION_MODE);
    print_step_info(N070_WAIT_TIMEOUT);
    print_step_info(N080_PUBLISH_SENSOR_OFFLINE);
    print_step_info(N090_START_TIMEOUT_DS);
    print_step_info(N100_WAIT_TIMEOUT_DS);
    print_step_info(N110_ENTER_DS);
    Serial.println("");
}