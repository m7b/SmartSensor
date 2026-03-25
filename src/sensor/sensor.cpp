#include "sensor/sensor.h"

sensor::sensor(ESP8266WiFiClass *wifi, rws_ntp *ntp, rws_syslog *syslog, rws_mqttclient *mqtt, rws_webupdate *webUpd)
: statemachine(N000_INIT_STEP)
{
    _wifi      = wifi;
    _ntp       = ntp;
    _syslog    = syslog;
    _webUpdate = webUpd;
    _mqtt      = mqtt;
    
    _barrel = new barrel(_ntp, _mqtt, _syslog);

    _mqtt_online = true;

    //Initial operation mode
    _FunctionModeRequest = FUNCTION_MODE_INTERVAL_MEASURE__5_MIN; //FUNCTION_MODE_DEEP_SLEEP__5_MIN;
    _FunctionModeAck     = FUNCTION_MODE_INTERVAL_MEASURE__5_MIN; //FUNCTION_MODE_DEEP_SLEEP__5_MIN;
    _FunctionMode        = FUNCTION_MODE_INTERVAL_MEASURE__5_MIN; //FUNCTION_MODE_DEEP_SLEEP__5_MIN;
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
    setup_webupdate();

    print_stm_steps();
}

void sensor::loop(void)
{
    //update time from NTP on demand, see NTP_UPDATE_INTERVAL_MS
    _ntp->update();

    //mqtt
    _mqtt->loop();

    //Web-Update functionality
    _webUpdate->loop();

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
    Serial.println("--> setup_wifi()");

    wifiConnectHandler = _wifi->onStationModeGotIP(std::bind(&sensor::onWifiConnect, this, std::placeholders::_1));
    
    _wifi->hostname(DEVICE_HOSTNAME);
    _wifi->mode(WIFI_STA);

    // Add WiFi connection credentials
    _wifi->begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.println("--> setup_wifi() done.");
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
    _syslog->log(LOG_INFO, "--> Start setup_mqtt()");
    Serial.println("--> setup_mqtt()");

    //Set topics to subscribe
    _mqtt->subscribe("WS/RWS/#", [this](const char * topic, const char * payload) {
        Serial.printf("Received topic '%s': %s\n", topic, payload);

        if (strcmp(topic, FUNCTION_MODE_REQ) == 0) {
            Serial.printf("  - Function mode request received: %d\r\n", payload[0]);
            _FunctionModeRequest = payload[0];
        }

    });

    _mqtt->begin();

    _syslog->log(LOG_INFO, "--> End setup_mqtt()");
    Serial.println("--> setup_mqtt() done.");
}


void sensor::setup_webupdate(void)
{
    _webUpdate->setup();
}


bool sensor::check_all_conditions(void)
{
    bool rc = true;
    
    rc = rc & WiFi.isConnected();
    /* Test if this is the cause of not working 
    rc = rc & _ntp->update(); */

    return rc;
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
            if (_FunctionMode != _FunctionModeRequest)
                set_next_step(N050_CHANGE_TO_REQ_FUNCTION_MODE);
            else
                set_next_step(N070_WAIT_TIMEOUT);
                
            break;

        case N050_CHANGE_TO_REQ_FUNCTION_MODE:
            _FunctionMode = _FunctionModeRequest;
            set_next_step(N060_ACK_NEW_FUNCTION_MODE);
            break;

        case N060_ACK_NEW_FUNCTION_MODE:
            _FunctionModeAck = _FunctionMode;
            //_mqtt->publish(FUNCTION_MODE_ACK, _FunctionModeAck, QOS0, RETAIN_ON);
            _mqtt->publish(FUNCTION_MODE_ACK, "2", QOS0, RETAIN_ON);

            set_next_step(N070_WAIT_TIMEOUT);
            break;

        case N070_WAIT_TIMEOUT: {
                unsigned long duration = 0;
                bool ds = false;

                switch (_FunctionMode)
                {
                    case FUNCTION_MODE_PERMANENT_MEASSURE:      duration = pm_time_200ms; break;
                    case FUNCTION_MODE_INTERVAL_MEASURE__5_SEK: duration = im_time__5sec; break;
                    case FUNCTION_MODE_INTERVAL_MEASURE_10_SEK: duration = im_time_10sec; break;
                    case FUNCTION_MODE_INTERVAL_MEASURE__1_MIN: duration = im_time__1min; break;
                    case FUNCTION_MODE_INTERVAL_MEASURE__5_MIN: duration = im_time__5min; break;
                    case FUNCTION_MODE_DEEP_SLEEP_20_SEK:       _ds_time = ds_time_20sec; ds = true; break;
                    case FUNCTION_MODE_DEEP_SLEEP__1_MIN:       _ds_time = ds_time__1min; ds = true; break;
                    case FUNCTION_MODE_DEEP_SLEEP__5_MIN:       _ds_time = ds_time__5min; ds = true; break;
                    case FUNCTION_MODE_DEEP_SLEEP_10_MIN:       _ds_time = ds_time_10min; ds = true; break;
                    case FUNCTION_MODE_DEEP_SLEEP_30_MIN:       _ds_time = ds_time_30min; ds = true; break;
                    case FUNCTION_MODE_DEEP_SLEEP__1_STD:       _ds_time = ds_time__1std; ds = true; break;
                    default:                                    duration = im_time__5sec; break;
                }

                //in case of deep sleep, perform special step
                if (ds)
                {
                    set_next_step(N080_PUBLISH_SENSOR_OFFLINE);
                }
                //else - wait timeout depending on operation mode
                else if (get_duration_ms(_start_time) >= duration)
                {
                    set_next_step(N000_INIT_STEP);
                }

                //Check function mode change request
                if (_FunctionMode != _FunctionModeRequest)
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
    _mqtt->publish(LAST_WILL_TOPIC, LAST_WILL_MESSAGE " - guided BC of Deep-Sleep", LAST_WILL_RETAIN);

    //Prevent reconnect in loop() after guided disconnection
    _mqtt_online = false;

    //disconnect from broker
    _mqtt->disconnect();
}


void sensor::do_deep_sleep(uint64_t ds_time)
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


void sensor::onWifiConnect(const WiFiEventStationModeGotIP& event)
{
    Serial.println("--> onWifiConnect()");
    //_syslog->log(LOG_INFO, "--> onWifiConnect()");
    //setup_mqtt();
}

void sensor::onWifiDisconnect(const WiFiEventStationModeDisconnected& event)
{
    Serial.println("--> onWifiDisconnect()");
}