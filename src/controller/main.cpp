#include <Arduino.h>
#include <rws_wifi.h>
#include <rws_ntp.h>
#include <rws_syslog.h>
#include <rws_pubsubclient.h>
#include "settings/smartsensor_settings.h"

rws_wifi wifiMulti;
rws_ntp ntp(NTP_SERVER, NTP_OFFSET_S, NTP_UPDATE_INTERVAL_MS);
rws_syslog syslog(SYSLOG_SERVER, SYSLOG_PORT, DEVICE_HOSTNAME, APP_NAME, LOG_KERN);
rws_pubsubclient mqtt(MQTT_SERVER, MQTT_PORT);

//Initial operation mode
int OperationMode = 0;

void test_fct_callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    Serial.println();

    for(auto el: topics_to_subscribe)
    {
        if (strcmp(topic, mqtt.add_root_topic(el.second).c_str()) == 0)
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

/**
 * @brief Setup the serial object
 * 
 */
void setup_serial(void) {
    Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);

#if DEBUG == ON
    // Take time for opening serial window
    delay(10000);
#endif
}

/**
 * @brief Setup the wifi object
 * 
 */
void setup_wifi(void) {
    // Add WiFi connection credentials
    for(auto entry : wifi_access_credentials) 
        wifiMulti.addAP(entry.first, entry.second);

    WiFi.mode(WIFI_STA);

    // We start by connecting to a WiFi network
    wifiMulti.check_connection();
}

/**
 * @brief Setup the ntp object
 * 
 */
void setup_ntp(void) {
    //Set dst/std rules
    TimeChangeRule rCEST = {CEST_ABBREV, CEST_WEEK, CEST_DOW, CEST_MONTH, CEST_HOUR, CEST_OFFSET};
    TimeChangeRule rCET  = {CET_ABBREV,  CET_WEEK,  CET_DOW,  CET_MONTH,  CET_HOUR,  CET_OFFSET};
    ntp.setRules(rCEST, rCET);

    // We start getting time from ntp
    ntp.begin();
}

/**
 * @brief Setup the mqtt object
 * 
 */
void setup_mqtt(void) {
    //Set 1st and 2nd level topic
    mqtt.set_1st_2nd_level_topic(TOP_LEVEL_TOPIC, LOCATION_NAME_SENSOR);

    //Set topics to subscribe
    mqtt.set_topics_to_subscribe(&topics_to_subscribe);

    //Set callback function
    mqtt.setCallback(test_fct_callback);

    // We start by connecting to MQTT server
    mqtt.check_connection();
}

/**
 * @brief Check all conditions ok
 * 
 * @return true 
 * @return false 
 */
bool check_all_conditions(void) {
    bool rc = true;
    
    rc = rc & wifiMulti.connected();
    rc = rc & mqtt.connected();
    //rc = rc & ntp.update();

    return rc;
}

/**
 * @brief Setup the arduino programm
 * 
 * put your setup code here, to run once.
 */
void setup(void) {
    // Set up serial interface
    setup_serial();

    // Setup Wifi
    setup_wifi();

    // Setup NTP
    //setup_ntp();

    // Setup MQTT
    setup_mqtt();
    
    randomSeed(micros());
}

/**
 * @brief Loop through the arduino programm
 * 
 * put your main code here, to run repeatedly.
 */
void loop(void) {
    //update time from NTP on demand, see NTP_UPDATE_INTERVAL_MS
    //ntp.update();

    //check and renew WiFi connection
    wifiMulti.check_connection();

    //check and renew MQTT connection
    mqtt.check_connection();

    //check all conditions are ok
    if (!check_all_conditions())
        return;

    //keep mqtt alive
    mqtt.loop();
}