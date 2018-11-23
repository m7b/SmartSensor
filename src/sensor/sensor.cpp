#include "sensor/sensor.h"

sensor::sensor(rws_wifi *wifi, rws_ntp *ntp, rws_syslog *syslog, rws_pubsubclient *mqtt)
{
    _wifiMulti = wifi;
    _ntp       = ntp;
    _syslog    = syslog;
    _mqtt      = mqtt;
    
    _barrel = new barrel(_ntp, _mqtt, _syslog);

    //Initial operation mode
    OperationMode = INTERVAL_MEASURE__5_SEK;
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
}

void sensor::loop(void)
{
    //update time from NTP on demand, see NTP_UPDATE_INTERVAL_MS
    _ntp->update();

    //check and renew WiFi connection
    _wifiMulti->check_connection();

    //check and renew MQTT connection
    _mqtt->check_connection();

    //check all conditions are ok
    if (!check_all_conditions())
        return;

    //keep mqtt alive
    _mqtt->loop();

    //test utc time
    _ntp->test(3000);

    //operation
    operating(OperationMode);
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
    //Set 1st and 2nd level topic
    _mqtt->set_1st_2nd_level_topic(TOP_LEVEL_TOPIC, LOCATION_NAME_SENSOR);

    //Set topics to subscribe
    _mqtt->set_topics_to_subscribe(&topics_to_subscribe);

    //Set callback function
    _mqtt->setCallback([this] (char* topic, uint8_t* payload, unsigned int length) { this->mqtt_callback(topic, payload, length); });

    // We start by connecting to MQTT server
    _mqtt->check_connection();
}


bool sensor::check_all_conditions(void)
{
    bool rc = true;
    
    rc = rc & _wifiMulti->connected();
    rc = rc & _mqtt->connected();
    rc = rc & _ntp->update();

    return rc;
}

void sensor::mqtt_callback(char* topic, uint8_t* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    Serial.println();

    for(auto el: topics_to_subscribe)
    {
        if (strcmp(topic, _mqtt->add_root_topic(el.second).c_str()) == 0)
        {
            switch(el.first)
            {
                case 0:     
                    Serial.printf("Function mode received: %d\r\n", payload[0]);
                    OperationMode = payload[0];
                    break;

                case 1:
                    Serial.printf("inTopic received: ");
                    for (unsigned int i=0; i<length; i++) {
                        Serial.print((char)payload[i]);
                    }
                    Serial.println(); 
                    break;
            }
        }
    }
}

void sensor::operating(int operation_mode)
{
    switch (get_step())
    {
        case N000_INIT_STEP:
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
            set_next_step(N040_WAIT_TIMEOUT);
            break;

        case N040_WAIT_TIMEOUT: {
                unsigned long duration = 0;

                switch (operation_mode)
                {
                    case PERMANENT_MEASSURE:      duration =    200; break;
                    case INTERVAL_MEASURE__5_SEK: duration =   5000; break;
                    case INTERVAL_MEASURE_10_SEK: duration =  10000; break;
                    case INTERVAL_MEASURE__5_MIN: duration = 300000; break;
                    case DEEP_SLEEP_20_SEK:       _ds_time = 20e6; break;
                    case DEEP_SLEEP__5_MIN:       _ds_time =  3e8; break;
                    default:                      duration =   3000; break;
                }

                //wait timeout depending on operation mode
                if (get_duration_ms(_start_time) >= duration)
                    set_next_step(N000_INIT_STEP);

                //in case of deep sleep, perform special step
                if ((operation_mode == DEEP_SLEEP_20_SEK) || (operation_mode == DEEP_SLEEP__5_MIN))
                {
                    Serial.println("Going to sleep ...");
                    set_next_step(N050_START_TIMEOUT_DS);
                }
            }
            break;

        case N050_START_TIMEOUT_DS:
            _start_time = millis();
            set_next_step(N060_WAIT_TIMEOUT_DS);
            break;

        case N060_WAIT_TIMEOUT_DS:
            if (get_duration_ms(_start_time) >= 200)
                set_next_step(N070_ENTER_DS);

            break;

        case N070_ENTER_DS:
            ESP.deepSleep(_ds_time);
            set_next_step(N080_WAIT_DS);
            break;

        case N080_WAIT_DS: //wait until deep sleep has performed. CPU stops working
            delay(20);
            break;
    }
}