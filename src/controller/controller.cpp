#include "controller/controller.h"

controller::controller(rws_wifi *wifi, rws_ntp *ntp, rws_syslog *syslog, rws_pubsubclient *mqtt)
{
    _wifiMulti = wifi;
    _ntp       = ntp;
    _syslog    = syslog;
    _mqtt      = mqtt;

    //Initial operation mode
    int OperationMode = 0;

    _light = new rgbled(ONBOARD_LED_RED, ONBOARD_LED_GREEN, ONBOARD_LED_BLUE);
}

controller::~controller()
{
    delete _light;
}

void controller::setup(void)
{
    setup_serial();
    setup_wifi();
    setup_ntp();
    setup_syslog();
    setup_mqtt();

    _light->set_delay_ms(333);
}

void controller::loop(void)
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

    WiFi.mode(WIFI_STA);

    // We start by connecting to a WiFi network
    _wifiMulti->check_connection();
}

void controller::setup_ntp(void)
{
    // We start getting time from ntp
    _ntp->begin();
}

void controller::setup_syslog(void)
{
    //Syslog already usable
}

void controller::setup_mqtt(void)
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


bool controller::check_all_conditions(void)
{
    bool rc = true;
    
    rc = rc & _wifiMulti->connected();
    rc = rc & _mqtt->connected();
    rc = rc & _ntp->update();

    return rc;
}

void controller::mqtt_callback(char* topic, uint8_t* payload, unsigned int length) {
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




void controller::operating(void)
{
    switch (sm.get_step())
    {
        case N000_INIT_STEP:
            sm.set_next_step(N010_CHECK_PUMP_NOT_ACTIVE);
            break;
            
        case N010_CHECK_PUMP_NOT_ACTIVE:
            if (true)
                sm.set_next_step(N020_CHECK_DST_LEVEL_BELOW_MAX);
            else
                sm.set_next_step(N080_CHECK_ONLY_START_PUMP);
            break;
            
        case N020_CHECK_DST_LEVEL_BELOW_MAX:
            if (true)
                sm.set_next_step(N030_CHECK_SRC_LEVEL_OVER_MIN);
            else
                sm.set_next_step(N999_END);
            break;
            
        case N030_CHECK_SRC_LEVEL_OVER_MIN:
            if (true)
                sm.set_next_step(N040_CHECK_PUMP_READY);
            else
                sm.set_next_step(N999_END);
            break;
            
        case N040_CHECK_PUMP_READY:
            if (true)
                sm.set_next_step(N050_CHECK_START_PUMP);
            else
                sm.set_next_step(N999_END);
            break;
            
        case N050_CHECK_START_PUMP:
            sm.set_next_step(N060_WAIT_PUMP_STARTED);
            break;
            
        case N060_WAIT_PUMP_STARTED:
            sm.set_next_step(N070_STORE_START_TIME);
            break;
            
        case N070_STORE_START_TIME:
            sm.set_next_step(N080_CHECK_ONLY_START_PUMP);
            break;
            
        case N080_CHECK_ONLY_START_PUMP:
            sm.set_next_step(N090_CHECK_SRC_LEVEL_DECREASING);
            break;
            
        case N090_CHECK_SRC_LEVEL_DECREASING:
            if (true)
                sm.set_next_step(N100_CHECK_DST_LEVEL_INCREASING);
            else
                sm.set_next_step(N200_REPORT_ERROR);
            break;
            
        case N100_CHECK_DST_LEVEL_INCREASING:
            if (true)
                sm.set_next_step(N110_CHECK_SRC_LEVEL_OVER_MIN);
            else
                sm.set_next_step(N200_REPORT_ERROR);
            break;
            
        case N110_CHECK_SRC_LEVEL_OVER_MIN:
            if (true)
                sm.set_next_step(N120_CHECK_DST_LEVEL_BELOW_MAX);
            else
                sm.set_next_step(N300_CHECK_STOP_PUMP);
            break;
            
        case N120_CHECK_DST_LEVEL_BELOW_MAX:
            if (true)
                sm.set_next_step(N130_CHECK_NO_STOP_DEMAND_FROM_USER);
            else
                sm.set_next_step(N300_CHECK_STOP_PUMP);
            break;
            
        case N130_CHECK_NO_STOP_DEMAND_FROM_USER:
            if (true)
                sm.set_next_step(N140_CHECK_PUMP_DURATION_BELOW_MAX);
            else
                sm.set_next_step(N300_CHECK_STOP_PUMP);
            break;
            
        case N140_CHECK_PUMP_DURATION_BELOW_MAX:
            if (true)
                sm.set_next_step(N200_REPORT_ERROR);
            else
                sm.set_next_step(N300_CHECK_STOP_PUMP);
            break;
            
        case N200_REPORT_ERROR:
            sm.set_next_step(N300_CHECK_STOP_PUMP);
            break;
            
        case N300_CHECK_STOP_PUMP:
            sm.set_next_step(N310_CHECK_PUMP_STOPPED);
            break;
            
        case N310_CHECK_PUMP_STOPPED:
            sm.set_next_step(N320_STORE_PUMP_DATA);
            break;
            
        case N320_STORE_PUMP_DATA:
            sm.set_next_step(N999_END);
            break;
            
        case N999_END:
            sm.set_next_step(N000_INIT_STEP);
            break;
    }
}