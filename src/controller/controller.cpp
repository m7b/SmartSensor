#include "controller/controller.h"


controller::controller(rws_wifi *wifi, rws_ntp *ntp, rws_syslog *syslog, rws_pubsubclient *mqtt)
{
    _wifiMulti = wifi;
    _ntp       = ntp;
    _syslog    = syslog;
    _mqtt      = mqtt;

    //Initial operation mode
    int OperationMode = 0;

    
    _light = new rgbled(15, 12, 13);
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